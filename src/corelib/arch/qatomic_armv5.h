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

#ifndef QATOMIC_ARMV5_H
#define QATOMIC_ARMV5_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_NOT_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_NOT_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_WAIT_FREE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_NOT_NATIVE

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_NOT_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_NOT_NATIVE

#ifdef QT_NO_ARM_EABI
# error "Sorry, ARM without EABI is no longer supported"
#endif
#ifndef Q_OS_LINUX
# error "Qt is misconfigured: this ARMv5 implementation is only possible on Linux"
#endif

template<> struct QAtomicIntegerTraits<int> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned int> { enum { IsInteger = 1 }; };

template <int size> struct QBasicAtomicOps: QGenericAtomicOps<QBasicAtomicOps<size> >
{
    // kernel places a restartable cmpxchg implementation at a fixed address
    template <typename T>
    static int _q_cmpxchg(T oldval, T newval, volatile T *ptr)
    {
        typedef int (* kernel_cmpxchg_t)(T oldval, T newval, volatile T *ptr);
        kernel_cmpxchg_t kernel_cmpxchg = *reinterpret_cast<kernel_cmpxchg_t>(0xffff0fc0);
        return kernel_cmpxchg(oldval, newval, ptr);
    }
    static void _q_dmb()
    {
        typedef void (* kernel_dmb_t)();
        kernel_dmb_t kernel_dmb = *reinterpret_cast<kernel_dmb_t>(0xffff0fa0);
        kernel_dmb();
    }

    static void orderedMemoryFence() { _q_dmb(); }

    template <typename T> static bool ref(T &_q_value);
    template <typename T> static bool deref(T &_q_value);

    static bool isTestAndSetNative() { return false; }
    static bool isTestAndSetWaitFree() { return false; }
    template <typename T> static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue);
    template <typename T> static T fetchAndStoreRelaxed(T &_q_value, T newValue);
    template <typename T> static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd);
};

template <typename T> struct QAtomicOps : QBasicAtomicOps<sizeof(T)>
{
    typedef T Type;
};

template<> template<typename T> inline
bool QBasicAtomicOps<4>::ref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue + 1;
    } while (_q_cmpxchg(originalValue, newValue, &_q_value) != 0);
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::deref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue - 1;
    } while (_q_cmpxchg(originalValue, newValue, &_q_value) != 0);
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T originalValue;
    do {
        originalValue = _q_value;
        if (originalValue != expectedValue)
            return false;
    } while (_q_cmpxchg(expectedValue, newValue, &_q_value) != 0);
    return true;
}

// Fetch and store for integers
#ifdef Q_CC_RVCT
template<> template <typename T> inline
__asm T QBasicAtomicOps<4>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    add r2, pc, #0
    bx r2
    arm
    swp r2,r1,[r0]
    mov r0, r2
    bx lr
    thumb
}
#else
template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    T originalValue;
    asm volatile("swp %0,%2,[%3]"
                 : "=&r"(originalValue), "=m" (_q_value)
                 : "r"(newValue), "r"(&_q_value)
                 : "cc", "memory");
    return originalValue;
}
#endif // Q_CC_RVCT

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    do {
        originalValue = _q_value;
        newValue = originalValue + valueToAdd;
    } while (_q_cmpxchg(originalValue, newValue, &_q_value) != 0);
    return originalValue;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_ARMV5_H
