/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#ifndef QGLOBALSTATIC_H
#define QGLOBALSTATIC_H

#include "qglobal.h"

#if defined(QT_NO_THREAD)

template <typename T>
class QGlobalStatic
{
public:
    T *pointer;
    bool destroyed;

    inline bool set(T *p) { pointer = p; return true; }
};
#define Q_GLOBAL_BASIC_ATOMIC_INITIALIZER(p) p

#else

// forward declaration, since qatomic.h needs qglobal.h
template <typename T> class QBasicAtomicPointer;

// POD for Q_GLOBAL_STATIC
template <typename T>
class QGlobalStatic
{
public:
    QBasicAtomicPointer<T> pointer;
    bool destroyed;

    inline bool set(T* p) { return pointer.testAndSetOrdered(0, p) == 0; }
};
#define Q_GLOBAL_BASIC_ATOMIC_INITIALIZER(p) Q_BASIC_ATOMIC_INITIALIZER(p)

#endif

typedef void (*QGlobalStaticCleanUpFunction)();
class QCleanUpGlobalStatic
{
public:
    QGlobalStaticCleanUpFunction func;
    inline ~QCleanUpGlobalStatic() { func(); }
};

// Created as a function-local static to delete a QGlobalStatic<T>
template <typename T>
class QGlobalStaticDeleter
{
public:
    QGlobalStaticDeleter(QGlobalStatic<T> &_globalStatic)
        : globalStatic(_globalStatic)
    { }

    inline ~QGlobalStaticDeleter()
    {
        globalStatic.destroyed = true;
        T* p = globalStatic.pointer;
        globalStatic.pointer = 0;
        delete p;
    }
private:
    QGlobalStatic<T> &globalStatic;
};

// The main macro, which defines a class for each use of Q_GLOBAL_STATIC.
// Don't ever add a constructor/destructor, nor data members.

#define Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS) \
static QGlobalStatic<TYPE > _q_static_##NAME = { Q_GLOBAL_BASIC_ATOMIC_INITIALIZER(0), false }; \
static class NAME                                                              \
{                                                                              \
public:                                                                        \
    inline bool isDestroyed() const                                            \
    {                                                                          \
        return _q_static_##NAME.destroyed;                                     \
    }                                                                          \
    inline bool exists() const                                                 \
    {                                                                          \
        return _q_static_##NAME.pointer != 0;                                  \
    }                                                                          \
    inline operator TYPE*()                                                    \
    {                                                                          \
        return operator->();                                                   \
    }                                                                          \
    inline TYPE *operator->()                                                  \
    {                                                                          \
        if (!_q_static_##NAME.pointer) {                                       \
            if (isDestroyed()) {                                               \
                qWarning("Fatal Error: Accessed global static '%s *%s()' after destruction. " \
                         "Defined at %s:%d", #TYPE, #NAME, __FILE__, __LINE__);\
                return 0;                                                      \
            }                                                                  \
            TYPE *x = new TYPE ARGS;                                           \
            if (_q_static_##NAME.set(x)                                        \
                && _q_static_##NAME.pointer != x) {                            \
                delete x;                                                      \
            } else {                                                           \
                static QGlobalStaticDeleter<TYPE> cleanUpObject(_q_static_##NAME); \
            }                                                                  \
        }                                                                      \
        return _q_static_##NAME.pointer;                                       \
    }                                                                          \
    inline TYPE &operator*()                                                   \
    {                                                                          \
        return *operator->();                                                  \
    }                                                                          \
    static inline void destroy() /*static so it can be used in qAddPostRoutine */\
    {                                                                          \
        QGlobalStaticDeleter<TYPE> deleteNow(_q_static_##NAME);                \
    }                                                                          \
    /* Qt4 compatibility */                                                    \
    inline TYPE *operator() () {                                               \
        return *this;                                                          \
    }                                                                          \
} NAME;

#define Q_GLOBAL_STATIC(TYPE, NAME) Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ())

#endif /* QGLOBALSTATIC_H */

