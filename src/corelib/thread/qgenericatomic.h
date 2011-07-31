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

#ifndef QGENERICATOMIC_H
#define QGENERICATOMIC_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#if 0
#pragma qt_sync_stop_processing
#endif

#ifdef Q_CC_GNU
// lowercase is fine, we'll undef it below
#define always_inline __attribute__((always_inline, gnu_inline))
#else
#define always_inline
#endif

template<typename T> struct QAtomicIntegerTraits { enum { IsInteger = 0 }; };

template <typename T> struct QAtomicAdditiveType
{
    typedef T AdditiveT;
    static const int AddScale = 1;
};
template <typename T> struct QAtomicAdditiveType<T *>
{
    typedef qptrdiff AdditiveT;
    static const int AddScale = sizeof(T);
};

// not really atomic...
template <typename BaseClass> struct QGenericAtomicOps
{
    template <typename T> struct AtomicType { typedef T Type; typedef T *PointerType; };

    static void acquireMemoryFence() { BaseClass::orderedMemoryFence(); }
    static void releaseMemoryFence() { BaseClass::orderedMemoryFence(); }
    static void orderedMemoryFence() { }

    template <typename T> static inline always_inline
    T load(T &_q_value)
    {
        return _q_value;
    }

    template <typename T> static inline always_inline
    void store(T &_q_value, T newValue)
    {
        _q_value = newValue;
    }

    template <typename T> static inline always_inline
    T loadAcquire(T &_q_value)
    {
        T tmp = *static_cast<volatile T *>(&_q_value);
        BaseClass::acquireMemoryFence();
        return tmp;
    }

    template <typename T> static inline always_inline
    void storeRelease(T &_q_value, T newValue)
    {
        BaseClass::releaseMemoryFence();
        *static_cast<volatile T *>(&_q_value) = newValue;
    }

    static inline bool isReferenceCountingNative()
    { return BaseClass::isFetchAndAddNative(); }
    static inline bool isReferenceCountingWaitFree()
    { return BaseClass::isFetchAndAddWaitFree(); }
    template <typename T> static inline always_inline
    bool ref(T &_q_value)
    {
        return BaseClass::fetchAndAddRelaxed(_q_value, 1) != T(-1);
    }

    template <typename T> static inline always_inline
    bool deref(T &_q_value)
    {
         return BaseClass::fetchAndAddRelaxed(_q_value, -1) != 1;
    }

#if 0
    // These functions have no default implementation
    // Archictectures must implement them
    static inline bool isTestAndSetNative();
    static inline bool isTestAndSetWaitFree();
    template <typename T> static inline
    bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue);
#endif

    template <typename T> static inline always_inline
    bool testAndSetAcquire(T &_q_value, T expectedValue, T newValue)
    {
        bool tmp = BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
        BaseClass::acquireMemoryFence();
        return tmp;
    }

    template <typename T> static inline always_inline
    bool testAndSetRelease(T &_q_value, T expectedValue, T newValue)
    {
        BaseClass::releaseMemoryFence();
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    template <typename T> static inline always_inline
    bool testAndSetOrdered(T &_q_value, T expectedValue, T newValue)
    {
        BaseClass::orderedMemoryFence();
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    static inline bool isFetchAndStoreNative() { return false; }
    static inline bool isFetchAndStoreWaitFree() { return false; }

    template <typename T> static inline always_inline
    T fetchAndStoreRelaxed(T &_q_value, T newValue)
    {
        // implement fetchAndStore on top of testAndSet
        forever {
            register T tmp = load(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, newValue))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndStoreAcquire(T &_q_value, T newValue)
    {
        T tmp = BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
        BaseClass::acquireMemoryFence();
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndStoreRelease(T &_q_value, T newValue)
    {
        BaseClass::releaseMemoryFence();
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    template <typename T> static inline always_inline
    T fetchAndStoreOrdered(T &_q_value, T newValue)
    {
        BaseClass::orderedMemoryFence();
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    static inline bool isFetchAndAddNative() { return false; }
    static inline bool isFetchAndAddWaitFree() { return false; }
    template <typename T> static inline always_inline
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        // implement fetchAndAdd on top of testAndSet
        forever {
            register T tmp = BaseClass::load(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp + valueToAdd)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndAddAcquire(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        T tmp = BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
        BaseClass::acquireMemoryFence();
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndAddRelease(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        BaseClass::releaseMemoryFence();
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }

    template <typename T> static inline always_inline
    T fetchAndAddOrdered(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd)
    {
        BaseClass::orderedMemoryFence();
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }
};

#undef always_inline

#endif // QGENERICATOMIC_H
