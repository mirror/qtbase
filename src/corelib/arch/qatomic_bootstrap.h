/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** Copyright (C) 2011 Thiago Macieira <thiago@kde.org>
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QATOMIC_BOOTSTRAP_H
#define QATOMIC_BOOTSTRAP_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

template<> struct QAtomicIntegerTraits<int> { enum { IsInteger = 1 }; };
template <typename T> struct QAtomicOps: QGenericAtomicOps<QAtomicOps<T> >
{
    typedef T Type;

    static bool ref(T &_q_value)
    {
        return ++_q_value != 0;
    }
    static bool deref(T &_q_value)
    {
        return --_q_value != 0;
    }

    static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
    {
        if (_q_value == expectedValue) {
            _q_value = newValue;
            return true;
        }
        return false;
    }

    static T fetchAndStoreRelaxed(T &_q_value, T newValue)
    {
        T tmp = _q_value;
        _q_value = newValue;
        return tmp;
    }

    static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        T returnValue = _q_value;
        _q_value += valueToAdd;
        return returnValue;
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_BOOTSTRAP_H
