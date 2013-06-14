/****************************************************************************
**
** Copyright (C) 2013 David Faure <faure+bluesystems@kde.org>
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

#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.addHelpOption("Test helper");
    parser.setRemainingArgumentsHelpText("command");
    parser.addVersionOption();
    parser.addOption(QCommandLineOption(QStringList() << "load", "Load file from URL.", "url"));
    parser.addOption(QCommandLineOption(QStringList() << "o" << "output", "Set output file.", "file"));

    // This program supports different options depending on the "command" (first argument).
    // Call parse() to find out the remaining arguments.
    parser.parse(app.arguments());

    const QStringList args = parser.remainingArguments();
    const QString command = args.isEmpty() ? QString() : args.first();
    if (command == "resize") {
      parser.setRemainingArgumentsHelpText("resize");
      parser.addOption(QCommandLineOption(QStringList() << "size", "New size.", "size"));
      parser.process(app);
      const QString size = parser.value("size");
      printf("Resizing %s to %s and saving to %s\n", qPrintable(parser.value("load")), qPrintable(size), qPrintable(parser.value("o")));
    } else {
      // Call process again, to handle unknown options this time.
      parser.process(app);
    }

    printf("Remaining arguments: %s\n", qPrintable(parser.remainingArguments().join(",")));

    return 0;
}

