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
#include <qstandardpaths.h>
#include <qdebug.h>
#include <qstandardpaths.h>

//TESTED_CLASS=QStandardPaths
//TESTED_FILES=qstandardpaths.cpp

class tst_qstandardpaths : public QObject {
  Q_OBJECT

public:
    tst_qstandardpaths() {
    }
    virtual ~tst_qstandardpaths() {
    }

private slots:
    void testDefaultLocations();
    void testCustomLocations();
    void testLocateAll();
    void testDataLocation();
    void testFindExecutable();
    void testRuntimeDirectory();
    void testCustomRuntimeDirectory();

private:
    void setCustomLocations() {
        m_thisDir = QFile::decodeName(SRCDIR);
        //qDebug() << m_thisDir;
        m_thisDir.chop(1); // remove trailing slash!

        qputenv("XDG_CONFIG_HOME", QFile::encodeName(m_thisDir));
        QDir parent(m_thisDir);
        parent.cdUp();
        m_globalDir = parent.path(); // gives us a trailing slash
        qputenv("XDG_CONFIG_DIRS", QFile::encodeName(m_globalDir));
    }
    QString m_thisDir;
    QString m_globalDir;
};

void tst_qstandardpaths::testDefaultLocations()
{
#ifndef Q_OS_WIN
    qputenv("XDG_CONFIG_HOME", QByteArray());
    qputenv("XDG_CONFIG_DIRS", QByteArray());
    const QString expectedConfHome = QDir::homePath() + QString::fromLatin1("/.config");
    QCOMPARE(QStandardPaths::storageLocation(QStandardPaths::ConfigLocation), expectedConfHome);
    const QStringList confDirs = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    QCOMPARE(confDirs.count(), 2);
    QVERIFY(confDirs.contains(expectedConfHome));

    qputenv("XDG_DATA_HOME", QByteArray());
    qputenv("XDG_DATA_DIRS", QByteArray());
    const QStringList genericDataDirs = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    QCOMPARE(genericDataDirs.count(), 3);
    const QString expectedDataHome = QDir::homePath() + QString::fromLatin1("/.local/share");
    QCOMPARE(genericDataDirs.at(0), expectedDataHome);
    QCOMPARE(genericDataDirs.at(1), QString::fromLatin1("/usr/local/share"));
    QCOMPARE(genericDataDirs.at(2), QString::fromLatin1("/usr/share"));
#endif
}

void tst_qstandardpaths::testCustomLocations()
{
#ifndef Q_OS_WIN
    setCustomLocations();

    // test storageLocation()
    QCOMPARE(QStandardPaths::storageLocation(QStandardPaths::ConfigLocation), m_thisDir);

    // test locate()
    const QString thisFileName = QString::fromLatin1("tst_qstandardpaths.cpp");
    QVERIFY(QFile::exists(m_thisDir + '/' + thisFileName));
    const QString thisFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, thisFileName);
    QVERIFY(!thisFile.isEmpty());
    QVERIFY(thisFile.endsWith(thisFileName));

    const QString dir = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString::fromLatin1("../qstandardpaths"), QStandardPaths::LocateDirectory);
    QVERIFY(!dir.isEmpty());
    const QString thisDirAsFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString::fromLatin1("../qstandardpaths"));
    QVERIFY(thisDirAsFile.isEmpty()); // not a file

    const QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    QCOMPARE(dirs, QStringList() << m_thisDir << m_globalDir);
#endif
}

// We really need QTemporaryDir for this test...

void tst_qstandardpaths::testLocateAll()
{
#ifndef Q_OS_WIN
    const QStringList appsDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "applications", QStandardPaths::LocateDirectory);
    //qDebug() << appsDirs;
    foreach(const QString &dir, appsDirs)
        QVERIFY2(dir.endsWith(QLatin1String("/share/applications")), qPrintable(dir));

    setCustomLocations();
    const QStringList allFiles = QStandardPaths::locateAll(QStandardPaths::ConfigLocation, "qstandardpaths.pro");
    QCOMPARE(allFiles.first(), QString(m_thisDir + QString::fromLatin1("/qstandardpaths.pro")));
#endif
}

void tst_qstandardpaths::testDataLocation()
{
    // On all platforms, DataLocation should be GenericDataLocation / organization name / app name
    // This allows one app to access the data of another app.
    {
        const QString base = QStandardPaths::storageLocation(QStandardPaths::GenericDataLocation);
        const QString app = QStandardPaths::storageLocation(QStandardPaths::DataLocation);
        QCOMPARE(base, app);
    }
    QCoreApplication::instance()->setOrganizationName("Qt");
    QCoreApplication::instance()->setApplicationName("QtTest");
    {
        const QString base = QStandardPaths::storageLocation(QStandardPaths::GenericDataLocation);
        const QString app = QStandardPaths::storageLocation(QStandardPaths::DataLocation);
        QCOMPARE(app, base + "/Qt/QtTest");
    }
}

void tst_qstandardpaths::testFindExecutable()
{
    // Search for 'sh' on unix and 'cmd.exe' on Windows
#ifdef Q_OS_WIN
    const QString exeName = "cmd.exe";
#else
    const QString exeName = "sh";
#endif

    const QString result = QStandardPaths::findExecutable(exeName);
    QVERIFY(!result.isEmpty());
#ifdef Q_OS_WIN
    QVERIFY(result.endsWith("/cmd.exe"));
#else
    QVERIFY(result.endsWith("/bin/sh"));
#endif

    // full path as argument
    QCOMPARE(QStandardPaths::findExecutable(result), result);

    // exe no found
    QVERIFY(QStandardPaths::findExecutable("idontexist").isEmpty());
    QVERIFY(QStandardPaths::findExecutable("").isEmpty());

    // link to directory
    const QString target = QDir::tempPath() + QDir::separator() + QLatin1String("link.lnk");
    QFile::remove(target);
    QFile appFile(QCoreApplication::applicationDirPath());
    QVERIFY(appFile.link(target));
    QVERIFY(QStandardPaths::findExecutable(target).isEmpty());
    QFile::remove(target);

    // findExecutable with a relative path
#ifdef Q_OS_UNIX
    const QString pwd = QDir::currentPath();
    QDir::setCurrent("/bin");
    QStringList possibleResults;
    possibleResults << QString::fromLatin1("/bin/sh") << QString::fromLatin1("/usr/bin/sh");
    const QString sh = QStandardPaths::findExecutable("./sh");
    QVERIFY2(possibleResults.contains(sh), qPrintable(sh));
    QDir::setCurrent(pwd);
#endif
}

void tst_qstandardpaths::testRuntimeDirectory()
{
    const QString runtimeDir = QStandardPaths::storageLocation(QStandardPaths::RuntimeLocation);
    QVERIFY(!runtimeDir.isEmpty());

    // Check that it can automatically fix permissions
#ifdef Q_OS_UNIX
    QFile file(runtimeDir);
    const QFile::Permissions wantedPerms = QFile::ReadUser | QFile::WriteUser | QFile::ExeUser;
    const QFile::Permissions additionalPerms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
    QCOMPARE(file.permissions(), wantedPerms | additionalPerms);
    QVERIFY(file.setPermissions(wantedPerms | QFile::ExeGroup));
    const QString runtimeDirAgain = QStandardPaths::storageLocation(QStandardPaths::RuntimeLocation);
    QCOMPARE(runtimeDirAgain, runtimeDir);
    QCOMPARE(QFile(runtimeDirAgain).permissions(), wantedPerms | additionalPerms);
#endif
}

void tst_qstandardpaths::testCustomRuntimeDirectory()
{
#ifdef Q_OS_UNIX
    qputenv("XDG_RUNTIME_DIR", QFile::encodeName("/tmp"));
    // It's very unlikely that /tmp is 0600 or that we can chmod it
    // The call below outputs
    //   "QStandardPaths: wrong ownership on runtime directory /tmp, 0 instead of $UID"
    // but we can't reliably expect that it's owned by uid 0, I think.
    const QString runtimeDir = QStandardPaths::storageLocation(QStandardPaths::RuntimeLocation);
    QVERIFY2(runtimeDir.isEmpty(), qPrintable(runtimeDir));
#endif
}

QTEST_MAIN(tst_qstandardpaths)

#include "tst_qstandardpaths.moc"
