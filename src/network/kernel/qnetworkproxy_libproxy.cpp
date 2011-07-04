/****************************************************************************
**
** Copyright (C) 2011 Corentin Chary
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
 * This code is based on the qnetworkproxy_win.cpp file.
 */

#include "qnetworkproxy.h"

#ifndef QT_NO_NETWORKPROXY

#include <qurl.h>
#include <qmutex.h>
#include <qlibrary.h>

#include <private/qmutexpool_p.h>

QT_BEGIN_NAMESPACE

struct pxProxyFactory_;

typedef struct pxProxyFactory_ *(*px_proxy_factory_new_proto)(void);
typedef void (*px_proxy_factory_free_proto)(struct pxProxyFactory_ *);
typedef char **(*px_proxy_factory_get_proxies_proto)(struct pxProxyFactory_ *, const char *);

static px_proxy_factory_new_proto local_px_proxy_factory_new = 0;
static px_proxy_factory_free_proto local_px_proxy_factory_free = 0;
static px_proxy_factory_get_proxies_proto local_px_proxy_factory_get_proxies = 0;

class QLibProxy
{
public:
    QLibProxy();
    ~QLibProxy();
    bool init();

    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query);

    struct pxProxyFactory_ *proxyFactory;
    bool initialized;
};

Q_GLOBAL_STATIC(QLibProxy, systemProxy)

QLibProxy::QLibProxy()
    : proxyFactory(0), initialized(false)
{
}

QLibProxy::~QLibProxy()
{
    if (local_px_proxy_factory_free)
	local_px_proxy_factory_free(proxyFactory);
}

bool QLibProxy::init()
{
    if (initialized)
	return !!proxyFactory;

#ifndef QT_NO_LIBRARY
    // protect initialization
#ifndef QT_NO_THREAD
    QMutexLocker locker(QMutexPool::globalInstanceGet(&initialized));
    // check initialized again, since another thread may have already
    // done the initialization
    if (initialized)
	return !!proxyFactory;
#endif

    initialized = true;

    QLibrary lib(QLatin1String("proxy"));
    if (!lib.load())
	return false;

    local_px_proxy_factory_new = (px_proxy_factory_new_proto)lib.resolve("px_proxy_factory_new");
    local_px_proxy_factory_free = (px_proxy_factory_free_proto)lib.resolve("px_proxy_factory_free");
    local_px_proxy_factory_get_proxies = (px_proxy_factory_get_proxies_proto)lib.resolve("px_proxy_factory_get_proxies");

    if (local_px_proxy_factory_new && local_px_proxy_factory_free && local_px_proxy_factory_get_proxies)
	proxyFactory = local_px_proxy_factory_new();
#endif

    return !!proxyFactory;
}

QList<QNetworkProxy> QLibProxy::queryProxy(const QNetworkProxyQuery &query)
{
    QList<QNetworkProxy> qproxies;

    if (!init())
        return qproxies;

    QByteArray bytes = query.url().toString().toLocal8Bit();
    char **proxies = local_px_proxy_factory_get_proxies(proxyFactory, bytes.constData());

    if (!proxies)
        return qproxies;

    /*
     * From proxy.h:
     *
     * The format of the returned proxy strings are as follows:
     *   - http://[username:password@]proxy:port
     *   - socks://[username:password@]proxy:port
     *   - socks5://[username:password@]proxy:port
     *   - socks4://[username:password@]proxy:port
     *   - <procotol>://[username:password@]proxy:port
     *   - direct://
     * Please note that the username and password in the above URLs are optional
     * and should be use to authenticate the connection if present.
     *
     * For SOCKS proxies, when the protocol version is specified (socks4:// or
     * sock5://), it is expected that only this version is used. When only
     * socks:// is set, the client MUST try SOCKS version 5 protocol and, on
     * connection failure, fallback to SOCKS version 4.
     */

    for (int i = 0; proxies[i]; ++i) {
        QNetworkProxy proxy = QNetworkProxy::NoProxy;
        QUrl url(QString::fromLocal8Bit(proxies[i]));

        if (url.scheme() == QLatin1String("http"))
            proxy.setType(QNetworkProxy::HttpProxy);
        else if (url.scheme() == QLatin1String("socks5") || url.scheme() == QLatin1String("socks"))
            proxy.setType(QNetworkProxy::Socks5Proxy);
        else if (url.scheme() == QLatin1String("direct"))
            proxy.setType(QNetworkProxy::NoProxy);

        if (proxy.type() != QNetworkProxy::NoProxy) {
            proxy.setHostName(url.host());
            proxy.setPort(url.port());

            if (!url.userName().isEmpty())
                proxy.setUser(url.userName());

            if (!url.password().isEmpty())
                proxy.setPassword(url.password());
        }

        qproxies << proxy;

        free(proxies[i]);
    }

    free(proxies);

    return qproxies;
}

QList<QNetworkProxy> QNetworkProxyFactory::systemProxyForQuery(const QNetworkProxyQuery & query)
{
    QLibProxy *sp = systemProxy();

    if (!sp)
        return QList<QNetworkProxy>() << QNetworkProxy();

    QList<QNetworkProxy> proxies;

    proxies = sp->queryProxy(query);

    // Always end by an empty proxy
    if (proxies.isEmpty() || proxies.last().type() != QNetworkProxy::NoProxy)
        proxies.append(QNetworkProxy::NoProxy);

    return proxies;
}

QT_END_NAMESPACE

#endif // QT_NO_NETWORKPROXY
