/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#define QT_DEPRECATED
#define QT_DISABLE_DEPRECATED_BEFORE 0
#include "private/qdataurl_p.h"
#include <QtTest/QtTest>
#include <QtCore/QDebug>

class tst_QDataUrl : public QObject
{
    Q_OBJECT

private slots:
    void nonData();
    void emptyData();
    void alreadyPercentageEncoded();
};

void tst_QDataUrl::nonData()
{
    QLatin1String data("http://test.com");
    QUrl url(data);
    QString mimeType;
    QByteArray payload;
    bool result = qDecodeDataUrl(url, mimeType, payload);
    QVERIFY(!result);
}

void tst_QDataUrl::emptyData()
{
    QLatin1String data("data:text/plain");
    QUrl url(data);
    QString mimeType;
    QByteArray payload;
    bool result = qDecodeDataUrl(url, mimeType, payload);
    QVERIFY(result);
    QCOMPARE(mimeType, QLatin1String("text/plain;charset=US-ASCII"));
    QVERIFY(payload.isNull());
}

void tst_QDataUrl::alreadyPercentageEncoded()
{
    QLatin1String data("data:text/plain,%E2%88%9A");
    QUrl url(data);
    QString mimeType;
    QByteArray payload;
    bool result = qDecodeDataUrl(url, mimeType, payload);
    QVERIFY(result);
    QCOMPARE(mimeType, QLatin1String("text/plain"));
    QCOMPARE(payload, QByteArray::fromPercentEncoding("%E2%88%9A"));
}

QTEST_MAIN(tst_QDataUrl)
#include "tst_qdataurl.moc"
