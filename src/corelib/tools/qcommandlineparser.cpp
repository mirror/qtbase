/****************************************************************************
**
** Copyright (C) 2012 author Laszlo Papp <lpapp@kde.org>
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qcommandlineparser.h"

#include <qcoreapplication.h>

QT_BEGIN_NAMESPACE

typedef QHash<QString, quint16> NameHash_t;

class QCommandLineParserPrivate
{
public:
    inline QCommandLineParserPrivate()
        : parseAfterDoubleDash(true)
    { }

    //! The command line options used for parsing
    QList<QCommandLineOption> commandLineOptionList;

    //! List of arguments found for each option.
    QList<QStringList> commandLineOptionArgumentList;

    // The maximum number of options allowed.
    static const NameHash_t::mapped_type maxOptionCount = ~ 0;

    //! Hash mapping option names to their offsets in commandLineOptionList and optionArgumentList.
    NameHash_t nameHash;

    //! Names of options found.
    QHash<quint16, QStringList> optionArgumentListHash;

    //! Names of options found.
    QStringList optionNames;

    //! Arguments which did not belong to any option.
    QStringList remainingArgumentList;

    //! Names of options which were unknown.
    QStringList unknownOptionNames;

    /*!
        Boolean variable whether or not to stop the command line argument
        parsing after the double dash occurence without any options names involved
        ('--').

        Set to \c true by default.
     */
    bool parseAfterDoubleDash;
};

/*!
    \since 5.1
    \class QCommandLineParser
    \brief The QCommandLineParser class provides a means for parsing the options
    of the command line interface.

    This parser finds options and their values on the command line.  The parser
    allows for long and short names, aliases (more than one name for the same
    option) and option arguments.

    It can parse the options passed to the program or an arbitrary list of
    options. It can subsequently return argument values it found, or a list
    of arguments that were left over.  The parser can also optionally remove
    unrecognised options from the leftover list.

    Options on the command line are recognised as starting with a dash character.
    The option "-" (single dash) is a special case, and is not treated as an option.
    By default, the parser will stop parsing once the option "--" (double dash)
    is encountered, although this behaviour can be changed.

    Short options are single letters.  The option "f" would be specified by
    passing "-f" on the command line.  Short options can be be bundled - any
    short option that does not take an argument can be immediately followed
    by another short option.  For example, if "f" has no argument, it can be
    bundled with option "b" like this: "-fb".

    Long options are more than one letter long.  The long option "foo" would
    be passed as "--foo".  Long options can not be bundled together.

    Short options that take an argument will use the remaining characters in
    the same argument.  For example, if "b" takes an argument, passing "-fbabc"
    will treat "abc" as b's argument.  If there are no more characters, the
    next argument is used - even if it starts with a dash.

    Long options are similar, but require an assignment operator to mark the
    end of the long name, such as shown here: "--bar=value".  If there is no
    assignment operator, the next argument is used - even if it starts with a
    dash.

    The parser does not support optional arguments - if an option is set to
    require an argument, one must be present.  If such an option is placed
    last and has no argument, the option will be treated as if it had not been
    specified.

    The parser does not automatically support negating or disabling long options
    by using the format "--disable-foo" or "--no-foo".  However, a caller could
    make an option with "no-foo" as one of its names, and handle the case
    explicitly themselves.

    All values of option arguments are string only.  It is up to the caller to
    convert to other types.
*/

/*!
    Constructs a command line parser object with the given arguments.
*/
QCommandLineParser::QCommandLineParser()
    : d(new QCommandLineParserPrivate)
{
#ifdef Q_OS_WIN32
    addOption(QCommandLineOption(QStringList() << QLatin1String("-h") << QLatin1String("--help") << QLatin1String("/?"), QObject::tr(QStrinag("Displays this help."))));
#else
    addOption(QCommandLineOption(QStringList() << QLatin1String("-h") << QLatin1String("--help"), QObject::tr("Displays this help.")));
#endif
}

/*!
    Add an option to look for while parsing.

    If the option contains no names or any name that is in use by a
    previously added option, adding it will fail.  Adding the option may
    also fail if memory cannot be allocated.

    There is currently a limit of 65535 options.  Subsequent additions will
    fail.

    \param option the option to add.
    \return whether the option could be added.
 */
bool QCommandLineParser::addOption(const QCommandLineOption& option)
{
    QStringList optionNames = option.names();

    if (d->commandLineOptionList.size() < d->maxOptionCount && !optionNames.isEmpty()) {

        foreach (const QString& name, optionNames)
        {
            if (d->nameHash.contains(name))
            {
                return false;
            }
        }

        d->commandLineOptionList.append(option);

        const quint16 offset = (quint16)(d->commandLineOptionList.size() - 1);

        foreach (const QString& name, optionNames)
        {
            d->nameHash.insert(name, offset);
        }

        return true;
    }

    return false;
}

bool QCommandLineParser::setHelpOption(bool isHelpOption)
{
}

/*!
    Parse the command line arguments.

    The command line is obtained from the current \c QCoreApplication
    instance - it will fail if this is not available.  The first argument
    in the list is the program name and is skipped.

    This method calls <tt>parse( const QStringList & )</tt>.

    \return whether the parsing succeeded.
    \sa parse( const QStringList & )
    \sa QCoreApplication::instance()
    \sa QCoreApplication::arguments()
 */
bool QCommandLineParser::parse()
{
    QCoreApplication *pApp = QCoreApplication::instance();

    if ( NULL != pApp )
    {
        QStringList args = pApp->arguments();

        if ( ! args.isEmpty() )
        {
            args.removeFirst();
            return parse( args );
        }
    }

    return false;

}

/*!
    Parse the given arguments for options.

    Any results from a previous parse operation are removed.  If
    \c m_bStopParsingAtDoubleDash is \c true the parser will not look for
    further options once it encounters the option "--"; this does not
    include when "--" follows an option that requires an argument.

    Options that were successfully recognised, and their arguments, are
    removed from the input list.  If \c m_bRemoveUnknownLongNames is
    \c true, unrecognised options are removed and placed into a list of
    unknown option names.  Anything left over is placed into a list of
    leftover arguments.

    A long option that does not take an argument will still be recognised
    if encountered in the form "--foo=value".  In this case, the argument
    value will be ignored.

    \param arguments the list of arguments to parse.
    \return whether the parsing succeeded.
 */
bool QCommandLineParser::parse(const QStringList & arguments)
{
    QString   argument;
    QString   optionName;
    QCommandLineOption::OptionType type;
    NameHash_t::mapped_type    optionOffset;

    const QStringList emptyList;
    const QString     emptyString(QLatin1String(""));
    const QString     doubleDashString(QLatin1String("--"));
    const QLatin1Char dashChar('-');
    const QLatin1Char slashChar('/');
    const QLatin1Char assignChar('=');

    d->remainingArgumentList.clear();
    d->optionNames.clear();
    d->unknownOptionNames.clear();

    for (QStringList::const_iterator argumentIterator = arguments.begin(); argumentIterator != arguments.end() ; ++argumentIterator)
    {
        QString argument = *argumentIterator;

        if (argument.startsWith(doubleDashString))
        {
            if (argument.length() > 2)
            {
                optionName = argument.mid(2).section(assignChar, 0, 1);

                if (d->nameHash.contains(optionName))
                {
                    d->optionNames.append(optionName);
                    optionOffset = *d->nameHash.find(optionName);
                    type = d->commandLineOptionList.at(optionOffset).optionType();

                    if (type == QCommandLineOption::OneValue || type == QCommandLineOption::MultipleValues)
                    {
                        if (!argument.contains(assignChar))
                        {
                            ++argumentIterator;

                            if (argumentIterator != arguments.end())
                            {
                                d->optionArgumentListHash[optionOffset].append(*argumentIterator);
                            }
                        }

                        else
                        {
                            d->optionArgumentListHash[optionOffset].append(argument.section(assignChar, 1));
                        }
                    }
                }

                else
                {
                    d->unknownOptionNames.append(optionName);
                }

            }

            else
            {
                if (d->parseAfterDoubleDash == true)
                {
                    d->remainingArgumentList.append(argument);
                }
                else
                {
                    break;
                }
            }
        }

#ifdef Q_OS_WIN32
        else if (argument.startsWith(slashChar) || argument.startsWith(dashChar))
#else
        else if (argument.startsWith(dashChar))
#endif
        {
            QString optionName = argument.mid(1);

            if (!optionName.isEmpty())
            {
                if (d->nameHash.contains(optionName))
                {
                    d->optionNames.append(optionName);
                    optionOffset = *d->nameHash.find(optionName);
                    type = d->commandLineOptionList.at(optionOffset).optionType();

                    if (type == QCommandLineOption::OneValue || type == QCommandLineOption::MultipleValues)
                    {
                        if (!argument.contains(assignChar))
                        {
                            ++argumentIterator;

                            if (argumentIterator != arguments.end())
                            {
                                d->optionArgumentListHash[optionOffset].append(*argumentIterator);
                            }
                        }

                        else
                        {
                            d->optionArgumentListHash[optionOffset].append(argument.section(assignChar, 1));
                        }
                    }
                }

                else
                {
                    d->unknownOptionNames.append(optionName);
                }
            }

            else
            {
                d->remainingArgumentList.append(argument);
            }
        }

        else
        {
            d->remainingArgumentList.append(argument);
        }

        return true;
    }

    return false;

}


/*!
    Get the argument for a given option.

    The name provided can be any long or short name of any option that was
    added with \c addOption().  All of an option's aliases are treated as
    being equivalent.  If the name is not recognised or that option was not
    present, a null string is returned.

    For options found by the parser, an empty string is returned if the
    option does not take an argument, otherwise the last argument found for
    that option is returned.

    \param name the name of the option to look for.
    \return a null string if not found, or a string representing the last
    value found for the option.
    \sa arguments()
 */

QString QCommandLineParser::argument(const QString& optionName) const
{
    QStringList argumentList = arguments(optionName);

    if (!argumentList.isEmpty())
    {
        return argumentList.last();
    }

    return QString();

}


/*!
    Get a list of arguments for a given option.

    The name provided can be any long or short name of any option that was
    added with \c addOption().  All of an option's aliases are treated as
    being equivalent.  If the name is not recognised or that option was not
    present, a null string is returned.

    For options found by the parser, the list will contain an entry for
    each time the option was encountered by the parser.  These entries
    will always be an empty string for options that do not take an argument.
    Options that do take an argument will have the list populated with the
    argument values in the order they were found.

    \param name the name of the option to look for.
    \return a list of the arguments found for the option.
    \sa argument()
 */

QStringList QCommandLineParser::arguments(const QString& optionName) const
{
    if (d->nameHash.contains(optionName))
    {
        const NameHash_t::mapped_type optionOffset = *d->nameHash.find(optionName);

        return d->optionArgumentListHash[optionOffset];
    }

    return QStringList();
}


/*!
    Get a list of left over arguments.

    These are all of the arguments that were not recognised as part of an
    option.  If \c m_bRemoveUnknownLongNames is \c true, unrecognised
    options will also have been removed.  Options with

    \return a list of left over arguments.
 */

QStringList QCommandLineParser::remainingArguments() const
{
    return d->remainingArgumentList;
}

/*!
    Get a list of option names that were found.

    This returns a list of all the recognised option names found by the
    parser, in the order in which they were found.  For any long options
    that were in the form "--foo=value", the value part will have been
    dropped.

    The names in this list do not include the preceding dash characters.
    Names may appear more than once in this list if they were encountered
    more than once by the parser.

    Any entry in the list can be used with \c getArgument() or with
    \c getArgumentList() to get any relevant arguments.

    \return a list of option names found by the parser.
 */

QStringList QCommandLineParser::optionNames() const
{
    return d->optionNames;
}

/*!
    Get a list of unknown option names.

    This list will include both long an short name options that were not
    recognised.  For any long options that were in the form "--foo=value",
    the value part will have been dropped and only the long name is added.

    The names in this list do not include the preceding dash characters.
    Names may appear more than once in this list if they were encountered
    more than once by the parser.

    \return a list of unknown option names.
    \sa optionNames()
 */

QStringList QCommandLineParser::unknownOptionNames() const
{
    return d->unknownOptionNames;
}
