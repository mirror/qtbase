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

#ifndef QATOMIC_X86_64_H
#define QATOMIC_X86_64_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_WAIT_FREE

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_WAIT_FREE

#define Q_ATOMIC_INT32_IS_SUPPORTED

#define Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_WAIT_FREE

#define Q_ATOMIC_INT32_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_INT32_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_INT32_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT32_FETCH_AND_ADD_IS_WAIT_FREE

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_WAIT_FREE

template<> struct QAtomicIntegerTraits<int> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned int> { enum { IsInteger = 1 }; };

template <int size> struct QBasicAtomicOps: QGenericAtomicOps<QBasicAtomicOps<size> >
{
    static inline bool isReferenceCountingNative() { return true; }
    static inline bool isReferenceCountingWaitFree() { return true; }
    template <typename T> static bool ref(T &_q_value);
    template <typename T> static bool deref(T &_q_value);

    static inline bool isTestAndSetNative() { return true; }
    static inline bool isTestAndSetWaitFree() { return true; }
    template <typename T> static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue);

    static inline bool isFetchAndStoreNative() { return true; }
    static inline bool isFetchAndStoreWaitFree() { return true; }
    template <typename T> static T fetchAndStoreRelaxed(T &_q_value, T newValue);

    static inline bool isFetchAndAddNative() { return true; }
    static inline bool isFetchAndAddWaitFree() { return true; }
    template <typename T> static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd);
};

template <typename T> struct QAtomicOps : QBasicAtomicOps<sizeof(T)>
{
    typedef T Type;
};

#if defined(Q_CC_GNU) || defined(Q_CC_INTEL)

template<> struct QAtomicIntegerTraits<char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<signed char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned char> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<short> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned short> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<long long> { enum { IsInteger = 1 }; };
template<> struct QAtomicIntegerTraits<unsigned long long> { enum { IsInteger = 1 }; };

template<> template<typename T> inline
bool QBasicAtomicOps<1>::ref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "incb %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template<typename T> inline
bool QBasicAtomicOps<2>::ref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "incw %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template<typename T> inline
bool QBasicAtomicOps<4>::ref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "incl %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template<typename T> inline
bool QBasicAtomicOps<8>::ref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "incq %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<1>::deref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "decb %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<2>::deref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "decw %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}
template<> template <typename T> inline
bool QBasicAtomicOps<4>::deref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "decl %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<8>::deref(T &_q_value)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "decq %0\n"
                 "setne %1"
                 : "=m" (_q_value), "=qm" (ret)
                 : "m" (_q_value)
                 : "memory");
    return ret != 0;
}

template<int size> template <typename T> inline
bool QBasicAtomicOps<size>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "cmpxchg %3,%2\n"
                 "sete %1\n"
                 : "=a" (newValue), "=qm" (ret), "+m" (_q_value)
                 : "r" (newValue), "0" (expectedValue)
                 : "memory");
    return ret != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<1>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    unsigned char ret;
    asm volatile("lock\n"
                 "cmpxchg %3,%2\n"
                 "sete %1\n"
                 : "=a" (newValue), "=qm" (ret), "+m" (_q_value)
                 : "q" (newValue), "0" (expectedValue)
                 : "memory");
    return ret != 0;
}

template<int size> template <typename T> inline
T QBasicAtomicOps<size>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    asm volatile("xchg %0,%1"
                 : "=r" (newValue), "+m" (_q_value)
                 : "0" (newValue)
                 : "memory");
    return newValue;
}

template<> template <typename T> inline
T QBasicAtomicOps<1>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    asm volatile("xchg %0,%1"
                 : "=q" (newValue), "+m" (_q_value)
                 : "0" (newValue)
                 : "memory");
    return newValue;
}

template<int size> template <typename T> inline
T QBasicAtomicOps<size>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    T result;
    asm volatile("lock\n"
                 "xadd %0,%1"
                 : "=r" (result), "+m" (_q_value)
                 : "0" (valueToAdd * QAtomicAdditiveType<T>::AddScale)
                 : "memory");
    return result;
}

template<> template <typename T> inline
T QBasicAtomicOps<1>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    T result;
    asm volatile("lock\n"
                 "xadd %0,%1"
                 : "=r" (result), "+m" (_q_value)
                 : "0" (valueToAdd * QAtomicAdditiveType<T>::AddScale)
                 : "memory");
    return result;
}

#define Q_ATOMIC_INT8_IS_SUPPORTED

#define Q_ATOMIC_INT8_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_REFERENCE_COUNTING_IS_WAIT_FREE

#define Q_ATOMIC_INT8_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_INT8_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_INT8_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT8_FETCH_AND_ADD_IS_WAIT_FREE

#define Q_ATOMIC_INT16_IS_SUPPORTED

#define Q_ATOMIC_INT16_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_REFERENCE_COUNTING_IS_WAIT_FREE

#define Q_ATOMIC_INT16_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_INT16_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_INT16_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT16_FETCH_AND_ADD_IS_WAIT_FREE

#define Q_ATOMIC_INT64_IS_SUPPORTED

#define Q_ATOMIC_INT64_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_REFERENCE_COUNTING_IS_WAIT_FREE

#define Q_ATOMIC_INT64_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_TEST_AND_SET_IS_WAIT_FREE

#define Q_ATOMIC_INT64_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_STORE_IS_WAIT_FREE

#define Q_ATOMIC_INT64_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT64_FETCH_AND_ADD_IS_WAIT_FREE

#else // !Q_CC_INTEL && !Q_CC_GNU

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
    Q_CORE_EXPORT int q_atomic_increment(volatile int *ptr);
    Q_CORE_EXPORT int q_atomic_decrement(volatile int *ptr);
    Q_CORE_EXPORT int q_atomic_set_int(volatile int *ptr, int newval);
    Q_CORE_EXPORT void *q_atomic_set_ptr(volatile void *ptr, void *newval);
    Q_CORE_EXPORT int q_atomic_fetch_and_add_int(volatile int *ptr, int value);
    Q_CORE_EXPORT void *q_atomic_fetch_and_add_ptr(volatile void *ptr, qptrdiff value);
} // extern "C"

template<> template<typename T> inline
bool QBasicAtomicOps<4>::ref(T &_q_value)
{
    return q_atomic_increment((int *)&_q_value) != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::deref(T &_q_value)
{
    return q_atomic_decrement((int *)&_q_value) != 0;
}

template<> template <typename T> inline
bool QBasicAtomicOps<4>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    return q_atomic_test_and_set_int((int*)&_q_value, int(expectedValue), int(newValue));
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    return T(q_atomic_set_int((int*)&_q_value, int(newValue));
}

template<> template <typename T> inline
T QBasicAtomicOps<4>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    return T(q_atomic_fetch_and_add_int((int *)&_q_value, valueToAdd * QAtomicAdditiveType<T>::AddScale));
}

template<> template <typename T> inline
bool QBasicAtomicOps<8>::testAndSetRelaxed(T &_q_value, T expectedValue, T newValue)
{
    return q_atomic_test_and_set_ptr(&_q_value, (void*)expectedValue, (void*)newValue);
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndStoreRelaxed(T &_q_value, T newValue)
{
    return T(q_atomic_set_ptr(&_q_value, (void*)newValue);
}

template<> template <typename T> inline
T QBasicAtomicOps<8>::fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
{
    return T(q_atomic_fetch_and_add_int(&_q_value, valueToAdd * QAtomicAdditiveType<T>::AddScale));
}

#endif // Q_CC_GNU || Q_CC_INTEL

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_X86_64_H
