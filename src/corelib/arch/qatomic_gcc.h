/****************************************************************************
**
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

#ifndef QATOMIC_GCC_H
#define QATOMIC_GCC_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

template<> struct QAtomicIntegerTraits<int> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned int> { enum { IsInteger = 1 }; };

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

#define Q_ATOMIC_INT32_IS_SUPPORTED
#define Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

#define Q_ATOMIC_POINTER_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_SOMETIMES_NATIVE

template <typename T> struct QAtomicOps: QGenericAtomicOps<QAtomicOps<T> >
{
    // The GCC intrinsics all have fully-ordered memory semantics, so we define
    // only the xxxRelaxed functions. The exception is __sync_lock_and_test,
    // which has acquire semantics, so we need to define the Release and
    // Ordered versions too.

    typedef T Type;

#ifndef __ia64__
    static T loadAcquire(T &_q_value)
    {
        T tmp = _q_value;
        __sync_synchronize();
        return tmp;
    }

    static void storeRelease(T &_q_value, T newValue)
    {
        __sync_synchronize();
        _q_value = newValue;
    }
#endif

    static bool isTestAndSetNative() { return false; }
    static bool isTestAndSetWaitFree() { return false; }
    static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
    {
        return __sync_bool_compare_and_swap(&_q_value, expectedValue, newValue);
    }

    static T fetchAndStoreRelaxed(T &_q_value, T newValue)
    {
        return __sync_lock_test_and_set(&_q_value, newValue);
    }

    static T fetchAndStoreRelease(T &_q_value, T newValue)
    {
        __sync_synchronize();
        return __sync_lock_test_and_set(&_q_value, newValue);
    }

    static T fetchAndStoreOrdered(T &_q_value, T newValue)
    {
        return fetchAndStoreRelease(_q_value, newValue);
    }

    static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        return __sync_fetch_and_add(&_q_value, valueToAdd * QAtomicAdditiveType<T>::AddScale);
    }
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QATOMIC_GCC_H
