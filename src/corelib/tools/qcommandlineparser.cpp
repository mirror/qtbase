/****************************************************************************
**
** Copyright (C) 2013 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qcommandlineparser.h"

#include <qcoreapplication.h>
#include <qhash.h>

#include <stdio.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

typedef QHash<QString, int> NameHash_t;

// Special value for "not found" when doing hash lookups.
static const NameHash_t::mapped_type optionNotFound = ~0;

class QCommandLineParserPrivate
{
public:
    inline QCommandLineParserPrivate()
        : parseAfterDoubleDash(true),
          needsParsing(true)
    { }

    void parse(const QStringList &args);
    void ensureParsed(const char *method);
    QStringList aliases(const QString &name) const;

    //! The command line options used for parsing
    QList<QCommandLineOption> commandLineOptionList;

    //! Hash mapping option names to their offsets in commandLineOptionList and optionArgumentList.
    NameHash_t nameHash;

    //! Option arguments found (only for OneValue options)
    QHash<int, QStringList> optionArgumentListHash;

    //! Names of options found on the command line.
    QStringList optionNames;

    //! Arguments which did not belong to any option.
    QStringList remainingArgumentList;

    //! Names of options which were unknown.
    QStringList unknownOptionNames;

    /*
        Boolean variable whether or not to stop the command line argument
        parsing after the double dash occurrence without any options names involved
        ('--').

        Set to \c true by default.
     */
    bool parseAfterDoubleDash;

    //! True if parse() needs to be called
    bool needsParsing;
};

QStringList QCommandLineParserPrivate::aliases(const QString &optionName) const
{
    const NameHash_t::mapped_type optionOffset = nameHash.value(optionName, optionNotFound);
    if (optionOffset == optionNotFound)
        return QStringList();
    return commandLineOptionList.at(optionOffset).names();
}

/*!
    \since 5.2
    \class QCommandLineParser
    \inmodule QtCore
    \ingroup tools

    \brief The QCommandLineParser class provides a means for handling the
    command line options.

    The parser is first set up with option definitions.

    It then parses the command line arguments, to detect which options have
    actually been used, and stores them, together with option values.
    Any argument that isn't an option (i.e. doesn't start with a '-') is
    stored as a "remaining argument".

    The parser handles short names, long names, more than one name for the same
    option, and option values.

    Options on the command line are recognized as starting with a single or
    double dash character(s). The option "-" (single dash) is a special case,
    and not treated as an option. By default, the parser will stop parsing once
    the option "--" (double dash) is encountered, although this behavior can be
    changed.

    Short options are single letters. The option "v" would be specified by
    passing "-v" on the command line. Short options cannot be bundled due to the
    existing limitations, namely: certain tools have already been using the
    -longname pattern. Therefore, the bundled short options could essentially
    clash with a long option name.

    Long options are more than one letter long. The long option "verbose" would
    be passed as "--verbose" or "-verbose". Long options can not obviously be
    bundled together either.

    Short options, taking an argument, cannot use the remaining characters in
    the same argument. For example, if "v" takes an argument, passing "-vverbose"
    cannot treat "verbose" as v's argument since "vverbose" could clash with the
    equally named long option. One way to put the values is to have assignment
    operator to mark the end of the short name, as shown here: "-v=value".
    If there is no assignment operator, the next argument is used.

    Long options are similar as they also require an assignment operator to
    mark the end of the long name, such as shown here: "--verbose=value". If
    there is no assignment operator, the next argument is used - even if it
    starts with a dash.

    Using an option value is encouraged by the class instead of counting the
    option occurrences on the command line when the number of option occurrences
    would otherwise define the exact operation. For example, the class does not
    handle the option "-vvvv" passed, as expected. It considers this as a long
    option name "vvvv". The preferred usage is "-v=4" or "-v 4" in those cases.
    It is somewhat a simpler form, and does not potentially clash with the
    equally named long option names.

    The parser does not support optional arguments - if an option is set to
    require an argument, one must be present. If such an option is placed last
    and has no argument, the option will be treated as if it had not been
    specified.

    The parser does not automatically support negating or disabling long options
    by using the format "--disable-option" or "--no-option". Although, a caller
    could make an option with "no-option" as one of its names, and handle the
    case explicitly themselves.

    The value of the options can be a string, string list or just the fact
    whether or not the option is set. There are convenience methods established
    in this class for getting the value(s). It is not necessary for the caller
    to convert to other types manually.

    \sa QCommandLineOption, QCoreApplication
*/

/*!
    Constructs a command line parser object.
*/
QCommandLineParser::QCommandLineParser()
    : d(new QCommandLineParserPrivate)
{
#ifdef Q_OS_WIN32
    addOption(QCommandLineOption(QStringList() << QStringLiteral("-h") << QStringLiteral("--help") << QStringLiteral("/?"), tr("Displays this help.")));
#else
    addOption(QCommandLineOption(QStringList() << QStringLiteral("-h") << QStringLiteral("--help"), tr("Displays this help.")));
#endif
}

/*!
    Destroys the command line parser object.
*/
QCommandLineParser::~QCommandLineParser()
{
    delete d;
}

/*!
    Adds the option \a option to look for while parsing.

    Returns true if the option adding was successful; otherwise returns false.

    The option adding fails if there is no name attached to the option, or
    the option has a name that clashes with an option name added before.
 */
bool QCommandLineParser::addOption(const QCommandLineOption &option)
{
    QStringList optionNames = option.names();

    if (!optionNames.isEmpty()) {

        foreach (const QString &name, optionNames) {
            if (d->nameHash.contains(name))
                return false;
        }

        d->commandLineOptionList.append(option);

        const int offset = d->commandLineOptionList.size() - 1;

        foreach (const QString &name, optionNames)
            d->nameHash.insert(name, offset);

        return true;
    }

    return false;
}

/*!
    Parses the command line arguments.

    Most programs don't need to call this, a simple call to process(app) is enough.

    parse() is more low-level, and only does the parsing. The application will have to
    take care of the error handling on unknown options, using unknownOptionNames().
    This can be useful for instance to show a graphical error message in graphical programs.

    Calling parse() instead of process() can also be useful in order to ignore unknown
    options temporarily, because more option definitions will be provided later on
    (depending on one of the arguments), before calling process().

    Don't forget that \a arguments starts with the name of the executable (ignored, though).
*/
void QCommandLineParser::parse(const QStringList &arguments)
{
    d->parse(arguments);
}

/*!
    Process the command line arguments.

    This means both parsing them, and handling the builtin options,
    --version if addVersionOption was called, --help if addHelpOption was called,
    as well as aborting on unknown option names, with an error message.

    The command line is obtained from the QCoreApplication instance \a app.

    \sa QCoreApplication::arguments()
 */
void QCommandLineParser::process(const QCoreApplication &app)
{
    d->parse(app.arguments());

    if (d->unknownOptionNames.count() == 1) {
        fprintf(stderr, "Unknown option '%s'.\n", qPrintable(d->unknownOptionNames.first()));
        ::exit(1);
    }
    if (d->unknownOptionNames.count() > 1) {
        fprintf(stderr, "Unknown options: %s.\n", qPrintable(d->unknownOptionNames.join(QLatin1String(", "))));
        ::exit(1);
    }
}

void QCommandLineParserPrivate::ensureParsed(const char *method)
{
    if (needsParsing) {
        qWarning("QCommandLineParser: call process or parse before %s", method);
    }
}

/*!
    \internal

    Parse the list of arguments \a arguments.

    Returns true if the command line parsing was successful; otherwise returns
    false.

    Any results from a previous parse operation are removed. If
    \c m_bStopParsingAtDoubleDash is \c true the parser will not look for
    further options once it encounters the option "--"; this does not
    include when "--" follows an option that requires an argument.

    Options that were successfully recognized, and their arguments, are
    removed from the input list. If \c m_bRemoveUnknownLongNames is
    \c true, unrecognized options are removed and placed into a list of
    unknown option names. Anything left over is placed into a list of
    leftover arguments.

    A long option that does not take an argument will still be recognized
    if encountered in the form "--option=value". In this case, the argument
    value will be ignored.
 */
void QCommandLineParserPrivate::parse(const QStringList &args)
{
    needsParsing = false;

    const QString     doubleDashString(QStringLiteral("--"));
    const QLatin1Char dashChar('-');
    const QLatin1Char slashChar('/');
    const QLatin1Char assignChar('=');

    remainingArgumentList.clear();
    optionNames.clear();
    unknownOptionNames.clear();

    QStringList arguments = args;
    arguments.removeFirst();

    for (QStringList::const_iterator argumentIterator = arguments.begin(); argumentIterator != arguments.end() ; ++argumentIterator) {
        QString argument = *argumentIterator;

        if (argument.startsWith(doubleDashString)) {
            if (argument.length() > 2) {
                QString optionName = argument.mid(2).section(assignChar, 0, 1);

                if (nameHash.contains(optionName)) {
                    optionNames.append(optionName);
                    const NameHash_t::mapped_type optionOffset = *nameHash.constFind(optionName);
                    const QCommandLineOption::OptionType type = commandLineOptionList.at(optionOffset).optionType();

                    if (type == QCommandLineOption::OneValue) {
                        if (!argument.contains(assignChar)) {
                            ++argumentIterator;

                            if (argumentIterator != arguments.end())
                                optionArgumentListHash[optionOffset].append(*argumentIterator);
                        } else {
                            optionArgumentListHash[optionOffset].append(argument.section(assignChar, 1));
                        }
                    }
                } else {
                    unknownOptionNames.append(optionName);
                }
            }
            else {
                if (parseAfterDoubleDash == true)
                    remainingArgumentList.append(argument);
                else
                    break;
            }
        }

        else if (
#ifdef Q_OS_WIN
                argument.startsWith(slashChar) ||
#endif
                argument.startsWith(dashChar)) {
            QString optionName = argument.mid(1);
            if (!optionName.isEmpty()) {
                if (nameHash.contains(optionName)) {
                    optionNames.append(optionName);
                    const NameHash_t::mapped_type optionOffset = *nameHash.constFind(optionName);
                    const QCommandLineOption::OptionType type = commandLineOptionList.at(optionOffset).optionType();
                    if (type == QCommandLineOption::OneValue) {
                        if (!argument.contains(assignChar)) {
                            ++argumentIterator;

                            if (argumentIterator != arguments.end())
                                optionArgumentListHash[optionOffset].append(*argumentIterator);
                        } else {
                            optionArgumentListHash[optionOffset].append(argument.section(assignChar, 1));
                        }
                    }
                } else {
                    unknownOptionNames.append(optionName);
                }
            } else {
                remainingArgumentList.append(argument);
            }
        } else {
            remainingArgumentList.append(argument);
        }
    }
}

/*!
    Checks whether the option \a name was passed to the application.

    Returns true if the option \a name was set, false otherwise.

    This is the recommended way to check for options with no values.

    The name provided can be any long or short name of any option that was
    added with \c addOption(). All the options names are treated as being
    equivalent. If the name is not recognized or that option was not present,
    false is returned.

    Example:
    \snippet code/src_corelib_tools_qcommandlineparser.cpp 0
 */

bool QCommandLineParser::isSet(const QString &name) const
{
    d->ensureParsed("isSet");
    if (d->optionNames.contains(name))
        return true;
    foreach (const QString &optionName, d->optionNames) {
        if (d->aliases(optionName).contains(name))
            return true;
    }
    return false;
}

/*!
    Returns the option value found for the given option name \a optionName, or
    null string if not found.

    The name provided can be any long or short name of any option that was
    added with \c addOption(). All the option names are treated as being
    equivalent. If the name is not recognized or that option was not present, a
    null string is returned.

    For options found by the parser, an empty string is returned if the
    option does not take an argument, otherwise the last argument found for
    that option is returned.

    \sa arguments()
 */

QString QCommandLineParser::argument(const QString &optionName) const
{
    d->ensureParsed("argument");
    const QStringList argumentList = arguments(optionName);

    if (!argumentList.isEmpty())
        return argumentList.last();

    return QString();
}

/*!
    Returns a list of option values found for the given option name \a
    optionName, or an empty list if not found.

    The name provided can be any long or short name of any option that was
    added with \c addOption(). All the options names are treated as being
    equivalent. If the name is not recognized or that option was not present, a
    null string is returned.

    For options found by the parser, the list will contain an entry for
    each time the option was encountered by the parser. These entries
    will always be an empty string for options that do not take an argument.
    Options that do take an argument will have the list populated with the
    argument values in the order they were found.

    \sa argument()
 */

QStringList QCommandLineParser::arguments(const QString &optionName) const
{
    d->ensureParsed("arguments");
    const NameHash_t::mapped_type optionOffset = d->nameHash.value(optionName, optionNotFound);
    if (optionOffset != optionNotFound) {
        QStringList args = d->optionArgumentListHash.value(optionOffset);
        if (args.isEmpty())
            args = d->commandLineOptionList.at(optionOffset).defaultValues();
        return args;
    }

    return QStringList();
}

/*!
    Returns a list of remaining arguments.

    These are all of the arguments that were not recognized as part of an
    option.
 */

QStringList QCommandLineParser::remainingArguments() const
{
    d->ensureParsed("remainingArguments");
    return d->remainingArgumentList;
}

/*!
    Returns a list of option names that were found.

    This returns a list of all the recognized option names found by the
    parser, in the order in which they were found. For any long options
    that were in the form "--option=value", the value part will have been
    dropped.

    The names in this list do not include the preceding dash characters.
    Names may appear more than once in this list if they were encountered
    more than once by the parser.

    Any entry in the list can be used with \c getArgument() or with
    \c getArgumentList() to get any relevant arguments.
 */

QStringList QCommandLineParser::optionNames() const
{
    d->ensureParsed("optionNames");
    return d->optionNames;
}

/*!
    Returns a list of unknown option names.

    This list will include both long an short name options that were not
    recognized. For any long options that were in the form "--option=value",
    the value part will have been dropped and only the long name is added.

    The names in this list do not include the preceding dash characters.
    Names may appear more than once in this list if they were encountered
    more than once by the parser.

    \sa optionNames()
 */

QStringList QCommandLineParser::unknownOptionNames() const
{
    d->ensureParsed("unknownOptionNames");
    return d->unknownOptionNames;
}

QT_END_NAMESPACE
