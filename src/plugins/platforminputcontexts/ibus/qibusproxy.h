/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -N -p qibusproxy -c QIBusProxy interfaces/org.freedesktop.IBus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef QIBUSPROXY_H_1308831142
#define QIBUSPROXY_H_1308831142

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.freedesktop.IBus
 */
class QIBusProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.IBus"; }

public:
    QIBusProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~QIBusProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QDBusObjectPath> CreateInputContext(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(name);
        return asyncCallWithArgumentList(QLatin1String("CreateInputContext"), argumentList);
    }

    inline QDBusPendingReply<> Exit(bool restart)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(restart);
        return asyncCallWithArgumentList(QLatin1String("Exit"), argumentList);
    }

    inline QDBusPendingReply<QString> GetAddress()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetAddress"), argumentList);
    }

    inline QDBusPendingReply<QVariantList> ListActiveEngines()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ListActiveEngines"), argumentList);
    }

    inline QDBusPendingReply<QVariantList> ListEngines()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ListEngines"), argumentList);
    }

    inline QDBusPendingReply<QDBusVariant> Ping(const QDBusVariant &data)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(data);
        return asyncCallWithArgumentList(QLatin1String("Ping"), argumentList);
    }

    inline QDBusPendingReply<> RegisterComponent(const QDBusVariant &components)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(components);
        return asyncCallWithArgumentList(QLatin1String("RegisterComponent"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

#endif
