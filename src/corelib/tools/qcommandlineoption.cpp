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

#include "qcommandlineoption.h"

#include "qset.h"

QT_BEGIN_NAMESPACE

class QCommandLineOptionPrivate : public QSharedData
{
public:
    inline QCommandLineOptionPrivate()
        : optionType(QCommandLineOption::NoValue)
    { }

    //! The list of names used for this option.
    QStringList nameSet;

    //! The mode used for this option.
    QCommandLineOption::OptionType optionType;

    //! The description used for this option.
    QString description;

    //! The list of default values used for this option.
    QStringList defaultValues;
};

/*!
    \since 5.2
    \class QCommandLineOption
    \brief The QCommandLineOption class defines a possible command-line option.
    \inmodule QtCore
    \ingroup shared
    \ingroup tools

    This class is used to describe an option on the command line. It allows
    different ways of defining the same option with multiple aliases possible.
    It is also used to describe how the option is used - it may be a flag (e.g. -v)
    or take an argument (e.g. -o file).
*/

/*!
    \enum QCommandLineOption::OptionType

    This enum defines the option type.

    \value NoValue The option does not have any values assigned, thus it is more
    like a boolean flag. Either --option is specified or not.

    \value OneValue The option has exactly one value. For instance "-o file".

    \sa optionType(), setOptionType()
*/

/*!
    Constructs a command line option object.
    The name and description are empty, the option doesn't take a value,
    and doesn't have a default value. Use setNames, setDescription,
    setOptionType and setDefaultValues to specify those.
*/
QCommandLineOption::QCommandLineOption()
    : d(new QCommandLineOptionPrivate)
{
}

/*!
    Constructs a command line option object with the given arguments.
    The names of the option are set to \a names and the description to \a description.
    In addition, the \a optionType can be set if the option expects a value.
    The default values for the option can be set to \a defaultValues.
*/
QCommandLineOption::QCommandLineOption(const QStringList &names, const QString &description,
                                       OptionType optionType,
                                       const QStringList &defaultValues)
    : d(new QCommandLineOptionPrivate)
{
    setNames(names);
    setOptionType(optionType);
    setDescription(description);
    setDefaultValues(defaultValues);
}

/*!
    Constructs a QCommandLineOption object that is a copy of the QCommandLineOption
    object \a other.

    \sa operator=()
*/
QCommandLineOption::QCommandLineOption(const QCommandLineOption &other):
    d(other.d)
{
}

/*!
    Destroys the command line option object.
*/
QCommandLineOption::~QCommandLineOption()
{
}

/*!
    Makes a copy of the \a other object and assigns it to this QCommandLineOption
    object.
*/
QCommandLineOption& QCommandLineOption::operator=(const QCommandLineOption &other)
{
    d = other.d;
    return *this;
}

/*!
    \fn void QCommandLineOption::swap(QCommandLineOption &other)

    Swaps option \a other with this option. This operation is very
    fast and never fails.
*/

/*!
    Returns the names set for this option.

    \sa setNames()
 */
QStringList QCommandLineOption::names() const
{
    return d->nameSet;
}

/*!
    Sets the list of names used for this option to \a names.

    The name can be either short or long. Any name in the list that is one
    character in length is a short name. Option names that start with dash
    character, have zero length or are duplicates are ignored.

    \sa names()
 */
void QCommandLineOption::setNames(const QStringList &names)
{
    foreach (const QString &name, names) {
        if (!name.isEmpty() && !name.startsWith(QLatin1Char('-')) && !name.startsWith(QLatin1Char('?'))) {
            d->nameSet.append(name);
        }
    }
}

/*!
    Sets the option type used for this option to \a optionType.

    \sa optionType()
 */
void QCommandLineOption::setOptionType(OptionType optionType)
{
    d->optionType = optionType;
}

/*!
    Returns the mode used for this option.

    \sa setOptionType()
 */
QCommandLineOption::OptionType QCommandLineOption::optionType() const
{
    return d->optionType;
}

/*!
    Sets the description used for this option to \a description.

    The description is used for instance while prompting some help output to the
    user of the application.

    \sa description()
 */
void QCommandLineOption::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns the description set for this option.

    \sa setDescription()
 */
QString QCommandLineOption::description() const
{
    return d->description;
}

/*!
    Sets the list of default values used for this option to \a defaultValues.

    The default values are used if the user of the application does not specify
    this option on the command line.

    \sa defaultValues()
 */
void QCommandLineOption::setDefaultValues(const QStringList &defaultValues)
{
    d->defaultValues = defaultValues;
}

/*!
    Returns the default values set for this option.

    \sa setDefaultValues()
 */
QStringList QCommandLineOption::defaultValues() const
{
    return d->defaultValues;
}

QT_END_NAMESPACE
