/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <iostream>
#include <qglobalstatic.h>

class tst_QGlobalStatic: public QObject
{
    Q_OBJECT
private slots:
    void testBasic();
    void testDestroy();
    void testArgs();
    void testPostRoutine();
};

static char *argv0;

class A
{
public:
    A() : i(1) {}
    A(int val) : i(val) {}
    int i;
};

Q_GLOBAL_STATIC(A, globalA)
Q_GLOBAL_STATIC(A, globalA2)

Q_GLOBAL_STATIC_WITH_ARGS(A, globalWithArgs, (42))

class B
{
public:
    ~B() {
        // Can't use qDebug here, we have no qcoreapp therefore no current thread anymore
        // (due to the post-routine test)
        if (globalA.isDestroyed() && globalWithArgs.isDestroyed())
            std::cout << "All global statics were successfully destroyed." << std::endl;
        else
            qFatal("globalA was not destroyed yet");
    }
};

Q_GLOBAL_STATIC(B, globalB)

void tst_QGlobalStatic::testBasic()
{
    QVERIFY(globalB);

    QVERIFY(!globalA.exists());
    QVERIFY(!globalA.isDestroyed());
    A *a = globalA;
    QVERIFY(a);
    QVERIFY(globalA.exists());
    A &x = *globalA;
    QVERIFY(a == &x);
    QVERIFY(a == globalA);
    QCOMPARE(globalA->i, 1);
    QVERIFY(!globalA.isDestroyed());
}

void tst_QGlobalStatic::testDestroy()
{
    QVERIFY(!globalA2.exists());
    QVERIFY(!globalA2.isDestroyed());

    QVERIFY(globalA2);
    QVERIFY(globalA2.exists());
    QVERIFY(!globalA2.isDestroyed());
    QCOMPARE(globalA2->i, 1);

    globalA2.destroy();
    QVERIFY(!globalA2.exists());
    QVERIFY(globalA2.isDestroyed());
}

void tst_QGlobalStatic::testArgs()
{
    QVERIFY(!globalWithArgs.exists());
    QCOMPARE(globalWithArgs->i, 42); // it gets created here
    QVERIFY(globalWithArgs.exists());
}

class WithPostRoutine
{
private:
    WithPostRoutine();
    friend class globalWithPost;
};

Q_GLOBAL_STATIC(WithPostRoutine, globalWithPost)

WithPostRoutine::WithPostRoutine()
{
    qAddPostRoutine(globalWithPost.destroy);
}

void tst_QGlobalStatic::testPostRoutine()
{
    int argc = 1;
    {
        QCoreApplication app(argc, &argv0);
        QVERIFY(globalWithPost);
    }
    QVERIFY(globalWithPost.isDestroyed());
}

//QTEST_APPLESS_MAIN(tst_QGlobalStatic)
int main(int argc, char *argv[])
{
    tst_QGlobalStatic tc;
    argv0 = argv[0];
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_qglobalstatic.moc"
