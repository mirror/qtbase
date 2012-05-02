/****************************************************************************
**
** Copyright (C) 2012 author Laszlo Papp <lpapp@kde.org>
** All rights reserved.
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

#include "qcommandlineoptionvalue.h"

QT_BEGIN_NAMESPACE

class QCommandLineOptionValuePrivate
{
public:
    inline QCommandLineOptionValuePrivate()
    { }
};

/*!
    \since 5.1
    \class QCommandLineOptionValue
    \brief The QCommandLineOptionValue class provides a means for getting the
    value of a command line option

    This class is used to describe an option value on the command line. It
    allows different ways of defining the same option with multiple aliases
    possible. It is also used to describe how the option is used - it may be a
    flag used once or more, or take an argument or multiple.

    \sa QCommandLineOption
*/

/*!
    Constructs a command line option value object
*/
QCommandLineOptionValue::QCommandLineOptionValue()
    : d(new QCommandLineOptionValuePrivate)
{
}

QCommandLineOptionValue::QCommandLineOptionValue(const QCommandLineOptionValue &other):
    d(other.d)
{
}

/*!
    Destroys the command line option value object.
*/
QCommandLineOptionValue::~QCommandLineOptionValue()
{
    delete d;
}

QCommandLineOptionValue&
QCommandLineOptionValue::operator=(const QCommandLineOptionValue &other)
{
    d = other.d;
    return *this;
}

bool QCommandLineOptionValue::operator==(const QCommandLineOptionValue &other) const
{
    // TODO: Implementation needed
    return true;
}

/*!
    Returns the option value as a bool if the option value has type() Bool.

    Returns true if the variant has type() \l Bool; otherwise returns false.

    \sa toString(), toStringList()
*/
bool QVariant::toBool() const
{
    if (d.type == Bool)
        return d.data.b;

    bool res = false;
    handlerManager[d.type]->convert(&d, Bool, &res, 0);

    return res;
}

/*!
    \fn QStringList QVariant::toStringList() const

    Returns the option values as a QStringList if the option value has type()
    StringList; otherwise returns an empty list.

    \sa toBool(), toString()
*/
QStringList QVariant::toStringList() const
{
    return qVariantToHelper<QStringList>(d, handlerManager);
}

/*!
    Returns the option value as a QString if the option value has type() \l
    String; otherwise returns an empty string.

    \sa toBool(), toStringList()
*/
QString QVariant::toString() const
{
    return qVariantToHelper<QString>(d, handlerManager);
}

