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

#ifndef QATOMIC_MIPS_H
#define QATOMIC_MIPS_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE

#define Q_ATOMIC_INT32_IS_SUPPORTED
#define Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_ADD_IS_ALWAYS_NATIVE

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template<> struct QAtomicIntegerTraits<int> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned int> { enum { IsInteger = 1 }; };

template <int size> struct QBasicAtomicOps: QGenericAtomicOps<QBasicAtomicOps<size> >
{
    static void acquireMemoryFence();
    static void releaseMemoryFence();
    static void orderedMemoryFence();

    static inline bool isReferenceCountingNative() { return true; }
    template <typename T> static bool ref(T &_q_value);
    template <typename T> static bool deref(T &_q_value);

    static inline bool isTestAndSetNative() { return true; }
    static inline bool isTestAndSetWaitFree() { return false; }
    template <typename T> static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue);

    static inline bool isFetchAndStoreNative() { return true; }
    template <typename T> static T fetchAndStoreRelaxed(T &_q_value, T newValue);

    static inline bool isFetchAndAddNative() { return true; }
    template <typename T> static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd);
};

template <typename T> struct QAtomicOps : QBasicAtomicOps<sizeof(T)>
{
    typedef T Type;
};

#if defined(Q_CC_GNU)

#if defined(_MIPS_ARCH_MIPS1) || (defined(__mips) && __mips - 0 == 1)
# error "Sorry, the MIPS1 architecture is not supported"
# error "please set '-march=' to your architecture (e.g., -march=mips32)"
#endif

template <int size> inline
void QBasicAtomicOps<size>::acquireMemoryFence()
{
    asm volatile ("sync 0x11" ::: "memory");
}

template <int size> inline
void QBasicAtomicOps<size>::releaseMemoryFence()
{
    asm volatile ("sync 0x12" ::: "memory");
}

template <int size> inline
void QBasicAtomicOps<size>::orderedMemoryFence()
{
    asm volatile ("sync 0" ::: "memory");
}

template<> template<typename T> inline
bool QBasicAtomicOps<4>::ref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "ll %[originalValue], %[_q_value]\n"
                 "addiu %[newValue], %[originalValue], %[one]\n"
                 "sc %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [one] "i" (1)
                 : "cc", "memory");
    return originalValue != T(-1);
}

template<> template<typename T> inline
bool QBasicAtomicOps<4>::deref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "ll %[originalValue], %[_q_value]\n"
                 "addiu %[newValue], %[originalValue], %[minusOne]\n"
                 "sc %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [minusOne] "i" (-1)
                 : "cc", "memory");
    return originalValue != 1;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T result;
    register T tempValue;
    asm volatile("0:\n"
                 "ll %[result], %[_q_value]\n"
                 "xor %[result], %[result], %[expectedValue]\n"
                 "bnez %[result], 0f\n"
                 "nop\n"
                 "move %[tempValue], %[newValue]\n"
                 "sc %[tempValue], %[_q_value]\n"
                 "beqz %[tempValue], 0b\n"
                 "nop\n"
                 "0:\n"
                 : [result] "=&r" (result),
                   [tempValue] "=&r" (tempValue),
                   [_q_value] "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register T tempValue;
    asm volatile("0:\n"
                 "ll %[originalValue], %[_q_value]\n"
                 "move %[tempValue], %[newValue]\n"
                 "sc %[tempValue], %[_q_value]\n"
                 "beqz %[tempValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [tempValue] "=&r" (tempValue),
                   [_q_value] "+m" (_q_value)
                 : [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "ll %[originalValue], %[_q_value]\n"
                 "addu %[newValue], %[originalValue], %[valueToAdd]\n"
                 "sc %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale)
                 : "cc", "memory");
    return originalValue;
}

#if defined(_MIPS_ARCH_MIPS64) || defined(__mips64)

#define Q_ATOMIC_INT64_IS_SUPPORTED
#define Q_ATOMIC_INT64_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template<> struct QAtomicIntegerTraits<long long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long long > { enum { IsInteger = 1 }; };

template<> template<typename T> inline
bool QBasicAtomicOps<8>::ref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "lld %[originalValue], %[_q_value]\n"
                 "addiu %[newValue], %[originalValue], %[one]\n"
                 "scd %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [one] "i" (1)
                 : "cc", "memory");
    return originalValue != T(-1);
}

template<> template<typename T> inline
bool QBasicAtomicOps<8>::deref(T &_q_value)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "lld %[originalValue], %[_q_value]\n"
                 "addiu %[newValue], %[originalValue], %[minusOne]\n"
                 "scd %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [minusOne] "i" (-1)
                 : "cc", "memory");
    return originalValue != 1;
}

template<> template <typename T> inline
bool QBasicAtomicOps<8>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T result;
    register T tempValue;
    asm volatile("0:\n"
                 "lld %[result], %[_q_value]\n"
                 "xor %[result], %[result], %[expectedValue]\n"
                 "bnez %[result], 0f\n"
                 "nop\n"
                 "move %[tempValue], %[newValue]\n"
                 "scd %[tempValue], %[_q_value]\n"
                 "beqz %[tempValue], 0b\n"
                 "nop\n"
                 "0:\n"
                 : [result] "=&r" (result),
                   [tempValue] "=&r" (tempValue),
                   [_q_value] "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue)
                 : "cc", "memory");
    return result == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register T tempValue;
    asm volatile("0:\n"
                 "lld %[originalValue], %[_q_value]\n"
                 "move %[tempValue], %[newValue]\n"
                 "scd %[tempValue], %[_q_value]\n"
                 "beqz %[tempValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [tempValue] "=&r" (tempValue),
                   [_q_value] "+m" (_q_value)
                 : [newValue] "r" (newValue)
                 : "cc", "memory");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    asm volatile("0:\n"
                 "lld %[originalValue], %[_q_value]\n"
                 "addu %[newValue], %[originalValue], %[valueToAdd]\n"
                 "scd %[newValue], %[_q_value]\n"
                 "beqz %[newValue], 0b\n"
                 "nop\n"
                 : [originalValue] "=&r" (originalValue),
                   [_q_value] "+m" (_q_value),
                   [newValue] "=&r" (newValue)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale)
                 : "cc", "memory");
    return originalValue;
}

#endif // MIPS64

#else
# error "This compiler for MIPS is not supported"
#endif // Q_CC_GNU

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_MIPS_H
