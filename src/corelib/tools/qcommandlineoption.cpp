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

#include "qcommandlineoption.h"

#include "qset.h"

QT_BEGIN_NAMESPACE

class QCommandLineOptionPrivate
{
public:
    inline QCommandLineOptionPrivate()
        : optionType(QCommandLineOption::NoValue), isRequired(false)
    { }

    //! The list of names used for this option.
    QStringList nameSet;

    //! The mode used for this option.
    QCommandLineOption::OptionType optionType;

    //! Whether the option is required or optional.
    bool isRequired;

    //! The description used for this option.
    QString description;

    //! The list of default values used for this option.
    QStringList defaultValues;
};

/*!
    \since 5.1
    \class QCommandLineOption
    \brief The QCommandLineOption class provides a means for command line option

    This class is used to describe an option on the command line. It allows
    different ways of defining the same option with multiple aliases possible.
    It is also used to describe how the option is used - it may be a flag used
    once or more, or take an argument or multiple.

    \sa QCommandLineParser
*/

/*!
    \enum QCommandLineOption::optionType

    The mode used for this option to interpret the meaning of its usage.

    \value Count The option does not have any values, but the number,
    it occures, matters. Typical example for this is when the verbosity is
    increased by using the "-v" option few times.

    \value NoValue The option does not have any values assigned, thus it is more
    like a boolean behavior. If the user specifies --foobar, it turns the
    related things to that options on.

    \value OneValue The option has exactly one value accepted, so even if it
    occurs few times, the last specified value is accepted.

    \value MultipleValues The option can be passed with multiple values and each
    one will be collected from the command line into a list.


    \sa setOptionType()
*/

/*!
    Constructs a command line option object
*/
QCommandLineOption::QCommandLineOption()
    : d(new QCommandLineOptionPrivate)
{
}

/*!
    Constructs a command line option object with the given arguments.
*/
QCommandLineOption::QCommandLineOption(const QStringList& names, const QString& description,
                                        OptionType optionType, bool required,
                                        const QStringList& defaultValues)
    : d(new QCommandLineOptionPrivate)
{
    setRequired(required);
    setNames(names);
    setOptionType(optionType);
    setDescription(description);
    setDefaultValues(defaultValues);
}

QCommandLineOption::QCommandLineOption(const QCommandLineOption &other):
    d(other.d)
{
}

/*!
    Destroys the command line option object.
*/
QCommandLineOption::~QCommandLineOption()
{
    delete d;
}

QCommandLineOption&
QCommandLineOption::operator=(const QCommandLineOption &other)
{
    d = other.d;
    return *this;
}

QStringList QCommandLineOption::names() const
{
    return d->nameSet;
}

/*!
    Set the names used for this option.

    The name can be either short or long. Any name in the list that is one
    character in length is a short name. Option names starting with dash
    character, have zero length or duplicates are ignored.

    \param names a list of names used for this option.
 */
void QCommandLineOption::setNames(const QStringList& names)
{
    foreach (const QString& name, names)
    {
        if (!name.isEmpty() && !name.startsWith('-') && !name.startsWith('?')) {
            d->nameSet.append(name);
        }
    }
}

/*!
    Return the mode used for this option. The default is
    QCommandLineOption::NoValue

    Setting \a optionMode to QCommandLineOption::Count defines that the occurence
    of the option is counted. One typical example for this is when the verbosity is
    increased by using the "-v" option few times.

    Setting \a optionMode to QCommandLineOption::NoValue defines that the option
    does not have any values assigned, thus it behaves as a boolean flag. Once
    --foobar is specified on the command line, that turns the feature on.

    Setting \a optionMode to QCommandLineOption::OneValue defines that the option
    accepts only and precisely one argument meaning that if there are more
    parameters assigned, the last occurence is taken.

    Setting \a optionMode to QCommandLineOption::MultipleValues defines that the option
    can be invoked with more than one argument on the command line, and they are
    sequentially appended into a list to get them all contained.

    \param optionType the mode used for this option.
    \sa optionType()
 */
void QCommandLineOption::setOptionType(OptionType optionType)
{
    d->optionType = optionType;
}

/*!
    Return the mode used for this option. The default is
    QCommandLineOption::NoValue

    \param optionType the mode used for this option.
    \sa setOptionType()
 */
QCommandLineOption::OptionType QCommandLineOption::optionType() const
{
    return d->optionType;
}

/*!
    Set whether or not this option is required

    If this is set to true as required, the option needs to be set explicitely
    without having a default value. Otherwise if it is set to false, this option
    becomes option and the user is not obligated to define it while running the
    application.

    \param required whether this option is required or optional..
 */
void QCommandLineOption::setRequired(bool required)
{
    d->isRequired = required;
}

/*!
    Set the description used for this option.

    The description is used for instance while prompting some help output to the
    user of the application.

    \param description the description used for this option.
 */
void QCommandLineOption::setDescription(const QString& description)
{
    d->description = description;
}

/*!
   Set the default values used for this option.

   The default values are used, if the user of the application does not specify
   them explicitely via the command line or other user interface.

   \param defaultValues the the list of the default values used for this option.
 */
void QCommandLineOption::setDefaultValues(const QStringList& defaultValues)
{
    d->defaultValues = defaultValues;
}

