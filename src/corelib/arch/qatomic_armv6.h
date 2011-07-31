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

#ifndef QATOMIC_ARMV6_H
#define QATOMIC_ARMV6_H

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

#ifndef Q_CC_RVCT

#ifndef Q_DATA_MEMORY_BARRIER
# define Q_DATA_MEMORY_BARRIER asm volatile("mcr     p15, 0, r0, c7, c10, 5":::"memory")
#endif
#ifndef Q_COMPILER_MEMORY_BARRIER
# define Q_COMPILER_MEMORY_BARRIER asm volatile("":::"memory")
#endif

template<> template<typename T> inline
bool QBasicAtomicOps<4>::ref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[newValue], [%[_q_value]]\n"
                 "add %[newValue], %[newValue], #1\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::deref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[newValue], [%[_q_value]]\n"
                 "sub %[newValue], %[newValue], #1\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register int result;
    asm volatile("0:\n"
                 "ldrex %[result], [%[_q_value]]\n"
                 "eors %[result], %[result], %[expectedValue]\n"
                 "strexeq %[result], %[newValue], [%[_q_value]]\n"
                 "teqeq %[result], #1\n"
                 "beq 0b\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return result == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[originalValue], [%[_q_value]]\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrex %[originalValue], [%[_q_value]]\n"
                 "add %[newValue], %[originalValue], %[valueToAdd]\n"
                 "strex %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

#if defined(__ARM_ARCH_7__) \
    || defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7R__) \
    || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_6K__)
// LDREXB, LDREXH and LDREXD are available on ARMv6K or higher

template<> struct QAtomicIntegerTraits<char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<signed char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<short> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned short> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<long long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long long> { enum { IsInteger = 1 }; };

#define Q_ATOMIC_INT8_IS_SUPPORTED
#define Q_ATOMIC_INT8_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_FETCH_AND_ADD_IS_ALWAYS_NATIVE

#define Q_ATOMIC_INT16_IS_SUPPORTED
#define Q_ATOMIC_INT16_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_FETCH_AND_ADD_IS_ALWAYS_NATIVE

#define Q_ATOMIC_INT64_IS_SUPPORTED
#define Q_ATOMIC_INT64_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template<> template<typename T> inline
bool QBasicAtomicOps<1>::ref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexb %[newValue], [%[_q_value]]\n"
                 "add %[newValue], %[newValue], #1\n"
                 "strexb %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<1>::deref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexb %[newValue], [%[_q_value]]\n"
                 "sub %[newValue], %[newValue], #1\n"
                 "strexb %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<1>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T result;
    asm volatile("0:\n"
                 "ldrexb %[result], [%[_q_value]]\n"
                 "eors %[result], %[result], %[expectedValue]\n"
                 "strexbeq %[result], %[newValue], [%[_q_value]]\n"
                 "teqeq %[result], #1\n"
                 "beq 0b\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return result == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<1>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexb %[originalValue], [%[_q_value]]\n"
                 "strexb %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<1>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexb %[originalValue], [%[_q_value]]\n"
                 "add %[newValue], %[originalValue], %[valueToAdd]\n"
                 "strexb %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template<> template<typename T> inline
bool QBasicAtomicOps<2>::ref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexh %[newValue], [%[_q_value]]\n"
                 "add %[newValue], %[newValue], #1\n"
                 "strexh %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<2>::deref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexh %[newValue], [%[_q_value]]\n"
                 "sub %[newValue], %[newValue], #1\n"
                 "strexh %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<2>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T result;
    asm volatile("0:\n"
                 "ldrexh %[result], [%[_q_value]]\n"
                 "eors %[result], %[result], %[expectedValue]\n"
                 "strexheq %[result], %[newValue], [%[_q_value]]\n"
                 "teqeq %[result], #1\n"
                 "beq 0b\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return result == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<2>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexh %[originalValue], [%[_q_value]]\n"
                 "strexh %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<2>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexh %[originalValue], [%[_q_value]]\n"
                 "add %[newValue], %[originalValue], %[valueToAdd]\n"
                 "strexh %[result], %[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

// Explanation from GCC's source code (config/arm/arm.c) on the modifiers below:
// Whenever you use "r" (dwordVariable), you get assigned a register pair:
//  %[reg]  - lower-numbered register
//  %H[reg] - higher-numbered register
//  %Q[reg] - low part of the value
//  %R[reg] - high part of the value
// If this is a little-endian build, H and R are the same; otherwise, H and Q are the same.

template<> template<typename T> inline
bool QBasicAtomicOps<8>::ref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexd %[newValue], %H[newValue], [%[_q_value]]\n"
                 "adds %Q[newValue], %Q[newValue], #1\n"
                 "adc %R[newValue], %R[newValue], #0\n"
                 "strexd %[result], %[newValue], %H[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<8>::deref(T &_q_value)
{
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexd %[newValue], %H[newValue], [%[_q_value]]\n"
                 "subs %Q[newValue], %Q[newValue], #1\n"
                 "sbc %R[newValue], %R[newValue], #0\n"
                 "strexd %[result], %[newValue], %H[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [_q_value] "r" (&_q_value)
                 : "cc", "memory");
    return newValue != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<8>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    register T result;
    asm volatile("0:\n"
                 "ldrexd %[result], %H[result], [%[_q_value]]\n"
                 "eor %[result], %[result], %[expectedValue]\n"
                 "eor %H[result], %H[result], %H[expectedValue]\n"
                 "orrs %[result], %[result], %H[result]\n"
                 "strexdeq %[result], %[newValue], %H[newValue], [%[_q_value]]\n"
                 "teqeq %[result], #1\n"
                 "beq 0b\n"
                 : [result] "=&r" (result),
                   "+m" (_q_value)
                 : [expectedValue] "r" (expectedValue),
                   [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return quint32(result) == 0;
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    register T originalValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexd %[originalValue], %H[originalValue], [%[_q_value]]\n"
                 "strexd %[result], %[newValue], %H[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [newValue] "r" (newValue),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    register T originalValue;
    register T newValue;
    register int result;
    asm volatile("0:\n"
                 "ldrexd %[originalValue], %H[originalValue], [%[_q_value]]\n"
                 "adds %Q[newValue], %Q[originalValue], %Q[valueToAdd]\n"
                 "adc %R[newValue], %R[originalValue], %R[valueToAdd]\n"
                 "strexd %[result], %[newValue], %H[newValue], [%[_q_value]]\n"
                 "teq %[result], #0\n"
                 "bne 0b\n"
                 : [originalValue] "=&r" (originalValue),
                   [newValue] "=&r" (newValue),
                   [result] "=&r" (result),
                   "+m" (_q_value)
                 : [valueToAdd] "r" (valueToAdd * QAtomicAdditiveType<T>::AddScale),
                   [_q_value] "r" (&_q_value)
                 : "cc");
    return originalValue;
}

#endif

#else
// This is Q_CC_RVCT

// RVCT inline assembly documentation:
// http://www.keil.com/support/man/docs/armcc/armcc_chdcffdb.htm
// RVCT embedded assembly documentation:
// http://www.keil.com/support/man/docs/armcc/armcc_chddbeib.htm

#if __TARGET_ARCH_THUMB-0 < 4
// save our pragma state and switch to ARM mode (unless using Thumb2)
# pragma push
# pragma arm
#endif

#ifndef Q_DATA_MEMORY_BARRIER
# define Q_DATA_MEMORY_BARRIER __schedule_barrier()
#endif
#ifndef Q_COMPILER_MEMORY_BARRIER
# define Q_COMPILER_MEMORY_BARRIER __schedule_barrier()
#endif

inline bool QBasicAtomicInt::ref()
{
    register int newValue;
    register int result;
    retry:
    __asm {
        ldrex   newValue, [&_q_value]
        add     newValue, newValue, #1
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return newValue != 0;
}

inline bool QBasicAtomicInt::deref()
{
    register int newValue;
    register int result;
    retry:
    __asm {
        ldrex   newValue, [&_q_value]
        sub     newValue, newValue, #1
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return newValue != 0;
}

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    register int result;
    retry:
    __asm {
        ldrex   result, [&_q_value]
        eors    result, result, expectedValue
        strexeq result, newValue, [&_q_value]
        teqeq   result, #1
        beq     retry
    }
    return result == 0;
}

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    register int originalValue;
    register int result;
    retry:
    __asm {
        ldrex   originalValue, [&_q_value]
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return originalValue;
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    register int originalValue;
    register int newValue;
    register int result;
    retry:
    __asm {
        ldrex   originalValue, [&_q_value]
        add     newValue, originalValue, valueToAdd
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    register T *result;
    retry:
    __asm {
        ldrex   result, [&_q_value]
        eors    result, result, expectedValue
        strexeq result, newValue, [&_q_value]
        teqeq   result, #1
        beq     retry
    }
    return result == 0;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    register T *originalValue;
    register int result;
    retry:
    __asm {
        ldrex   originalValue, [&_q_value]
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return originalValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    register T *originalValue;
    register T *newValue;
    register int result;
    retry:
    __asm {
        ldrex   originalValue, [&_q_value]
        add     newValue, originalValue, valueToAdd * sizeof(T)
        strex   result, newValue, [&_q_value]
        teq     result, #0
        bne     retry
    }
    return originalValue;
}

#if __TARGET_ARCH_THUMB-0 < 4
# pragma pop
#endif

#endif

// common code

template <int size> inline
void QBasicAtomicOps<size>::orderedMemoryFence()
{
    Q_DATA_MEMORY_BARRIER;
}

#undef Q_DATA_MEMORY_BARRIER
#undef Q_COMPILER_MEMORY_BARRIER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_ARMV6_H
