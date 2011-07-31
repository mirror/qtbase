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

#include "qplatformdefs.h"
#include "qmutex.h"
#include <qdebug.h>

#ifndef QT_NO_THREAD
#include "qatomic.h"
#include "qelapsedtimer.h"
#include "qthread.h"
#include "qmutex_p.h"

#ifndef Q_OS_LINUX
#include "private/qfreelist_p.h"
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QBasicMutex
    \brief QMutex POD
    \internal

    \ingroup thread

    - Can be used as global static object.
    - Always non-recursive
    - Do not use tryLock with timeout > 0, else you can have a leak (see the ~QMutex destructor)
*/


/*!
    \class QMutex
    \brief The QMutex class provides access serialization between threads.

    \threadsafe

    \ingroup thread

    The purpose of a QMutex is to protect an object, data structure or
    section of code so that only one thread can access it at a time
    (this is similar to the Java \c synchronized keyword). It is
    usually best to use a mutex with a QMutexLocker since this makes
    it easy to ensure that locking and unlocking are performed
    consistently.

    For example, say there is a method that prints a message to the
    user on two lines:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 0

    If these two methods are called in succession, the following happens:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 1

    If these two methods are called simultaneously from two threads then the
    following sequence could result:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 2

    If we add a mutex, we should get the result we want:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 3

    Then only one thread can modify \c number at any given time and
    the result is correct. This is a trivial example, of course, but
    applies to any other case where things need to happen in a
    particular sequence.

    When you call lock() in a thread, other threads that try to call
    lock() in the same place will block until the thread that got the
    lock calls unlock(). A non-blocking alternative to lock() is
    tryLock().

    \sa QMutexLocker, QReadWriteLock, QSemaphore, QWaitCondition
*/

/*!
    \enum QMutex::RecursionMode

    \value Recursive  In this mode, a thread can lock the same mutex
                      multiple times and the mutex won't be unlocked
                      until a corresponding number of unlock() calls
                      have been made.

    \value NonRecursive  In this mode, a thread may only lock a mutex
                         once.

    \sa QMutex()
*/

/*!
    Constructs a new mutex. The mutex is created in an unlocked state.

    If \a mode is QMutex::Recursive, a thread can lock the same mutex
    multiple times and the mutex won't be unlocked until a
    corresponding number of unlock() calls have been made. The
    default is QMutex::NonRecursive.

    \sa lock(), unlock()
*/
QMutex::QMutex(RecursionMode mode)
{
    d.store(mode == Recursive ? new QRecursiveMutexPrivate : 0);
}

/*!
    Destroys the mutex.

    \warning Destroying a locked mutex may result in undefined behavior.
*/
QMutex::~QMutex()
{
    if (isRecursive())
        delete static_cast<QRecursiveMutexPrivate *>(d.load());
    else if (d.load()) {
#ifndef Q_OS_LINUX
        if (d->possiblyUnlocked && tryLock()) { unlock(); return; }
#endif
        qWarning("QMutex: destroying locked mutex");
    }
}

/*! \fn void QMutex::lock()
    Locks the mutex. If another thread has locked the mutex then this
    call will block until that thread has unlocked it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e dead-lock when the mutex is locked recursively.

    \sa unlock()
*/

/*!\fn bool QMutex::trylock()
    Attempts to lock the mutex. If the lock was obtained, this function
    returns true. If another thread has locked the mutex, this
    function returns false immediately.

    If the lock was obtained, the mutex must be unlocked with unlock()
    before another thread can successfully lock it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e always return false when attempting to lock the mutex
    recursively.

    \sa lock(), unlock()
*/

/*! \fn bool QMutex::tryLock(int timeout)
     \overload

    Attempts to lock the mutex. This function returns true if the lock
    was obtained; otherwise it returns false. If another thread has
    locked the mutex, this function will wait for at most \a timeout
    milliseconds for the mutex to become available.

    Note: Passing a negative number as the \a timeout is equivalent to
    calling lock(), i.e. this function will wait forever until mutex
    can be locked if \a timeout is negative.

    If the lock was obtained, the mutex must be unlocked with unlock()
    before another thread can successfully lock it.

    Calling this function multiple times on the same mutex from the
    same thread is allowed if this mutex is a
    \l{QMutex::Recursive}{recursive mutex}. If this mutex is a
    \l{QMutex::NonRecursive}{non-recursive mutex}, this function will
    \e always return false when attempting to lock the mutex
    recursively.

    \sa lock(), unlock()
*/


/*! \fn void QMutex::unlock()
    Unlocks the mutex. Attempting to unlock a mutex in a different
    thread to the one that locked it results in an error. Unlocking a
    mutex that is not locked results in undefined behavior.

    \sa lock()
*/

/*!
    \fn void QMutex::isRecursive()
    \since 5.0

    Returns true if the mutex is recursive

*/
bool QBasicMutex::isRecursive() {
    QMutexPrivate *d = this->d.load();
    if (quintptr(d) <= 0x3)
        return false;
    return d->recursive;
}


/*!
    \class QMutexLocker
    \brief The QMutexLocker class is a convenience class that simplifies
    locking and unlocking mutexes.

    \threadsafe

    \ingroup thread

    Locking and unlocking a QMutex in complex functions and
    statements or in exception handling code is error-prone and
    difficult to debug. QMutexLocker can be used in such situations
    to ensure that the state of the mutex is always well-defined.

    QMutexLocker should be created within a function where a
    QMutex needs to be locked. The mutex is locked when QMutexLocker
    is created. You can unlock and relock the mutex with \c unlock()
    and \c relock(). If locked, the mutex will be unlocked when the
    QMutexLocker is destroyed.

    For example, this complex function locks a QMutex upon entering
    the function and unlocks the mutex at all the exit points:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 4

    This example function will get more complicated as it is
    developed, which increases the likelihood that errors will occur.

    Using QMutexLocker greatly simplifies the code, and makes it more
    readable:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 5

    Now, the mutex will always be unlocked when the QMutexLocker
    object is destroyed (when the function returns since \c locker is
    an auto variable).

    The same principle applies to code that throws and catches
    exceptions. An exception that is not caught in the function that
    has locked the mutex has no way of unlocking the mutex before the
    exception is passed up the stack to the calling function.

    QMutexLocker also provides a \c mutex() member function that returns
    the mutex on which the QMutexLocker is operating. This is useful
    for code that needs access to the mutex, such as
    QWaitCondition::wait(). For example:

    \snippet doc/src/snippets/code/src_corelib_thread_qmutex.cpp 6

    \sa QReadLocker, QWriteLocker, QMutex
*/

/*!
    \fn QMutexLocker::QMutexLocker(QMutex *mutex)

    Constructs a QMutexLocker and locks \a mutex. The mutex will be
    unlocked when the QMutexLocker is destroyed. If \a mutex is zero,
    QMutexLocker does nothing.

    \sa QMutex::lock()
*/

/*!
    \fn QMutexLocker::~QMutexLocker()

    Destroys the QMutexLocker and unlocks the mutex that was locked
    in the constructor.

    \sa QMutex::unlock()
*/

/*!
    \fn QMutex *QMutexLocker::mutex() const

    Returns a pointer to the mutex that was locked in the
    constructor.
*/

/*!
    \fn void QMutexLocker::unlock()

    Unlocks this mutex locker. You can use \c relock() to lock
    it again. It does not need to be locked when destroyed.

    \sa relock()
*/

/*!
    \fn void QMutexLocker::relock()

    Relocks an unlocked mutex locker.

    \sa unlock()
*/

#ifndef Q_OS_LINUX //linux implementation is in qmutex_linux.cpp
/*!
    \internal helper for lock()
 */
bool QBasicMutex::lockInternal(int timeout)
{
    while (!fastTryLock()) {
        QMutexPrivate *d = this->d;
        if (!d) // if d is 0, the mutex is unlocked
            continue;

        if (d == dummyLocked()) {
            if (timeout == 0)
                return false;
            QMutexPrivate *newD = QMutexPrivate::allocate();
            if (!this->d.testAndSetOrdered(d, newD)) {
                //Either the mutex is already unlocked, or another thread already set it.
                newD->deref();
                continue;
            }
            d = newD;
            //the d->refCount is already 1 the deref will occurs when we unlock
        } else if (d->recursive) {
             return static_cast<QRecursiveMutexPrivate *>(d)->lock(timeout);
        }

        if (timeout == 0 && !d->possiblyUnlocked)
            return false;

        if (!d->ref())
            continue; //that QMutexPrivate was already released

        if (d != this->d) {
            //Either the mutex is already unlocked, or relocked with another mutex
            d->deref();
            continue;
        }

        int old_waiters;
        do {
            old_waiters = d->waiters;
            if (old_waiters == -QMutexPrivate::BigNumber) {
                // we are unlocking, and the thread that unlocks is about to change d to 0
                // we try to aquire the mutex by changing to dummyLocked()
                if (this->d.testAndSetAcquire(d, dummyLocked())) {
                    // Mutex aquired
                    Q_ASSERT(d->waiters == -QMutexPrivate::BigNumber || d->waiters == 0);
                    d->waiters = 0;
                    d->deref();
                    return true;
                } else {
                    Q_ASSERT(d != this->d); //else testAndSetAcquire should have succeeded
                    // Mutex is likely to bo 0, we should continue the outer-loop,
                    //  set old_waiters to the magic value of BigNumber
                    old_waiters = QMutexPrivate::BigNumber;
                    break;
                }
            }
        } while (!d->waiters.testAndSetRelaxed(old_waiters, old_waiters + 1));

        if (d != this->d) {
            // Mutex was unlocked.
            if (old_waiters != QMutexPrivate::BigNumber) {
                //we did not break the previous loop
                Q_ASSERT(d->waiters >= 1);
                d->waiters.deref();
            }
            d->deref();
            continue;
        }

        if (d->wait(timeout)) {
            if (d->possiblyUnlocked && d->possiblyUnlocked.testAndSetRelaxed(true, false))
                d->deref();
            d->derefWaiters(1);
            //we got the lock. (do not deref)
            Q_ASSERT(d == this->d);
            return true;
        } else {
            Q_ASSERT(timeout >= 0);
            //timeout
            d->derefWaiters(1);
            //There may be a race in which the mutex is unlocked right after we timed out,
            // and before we deref the waiters, so maybe the mutex is actually unlocked.
            if (!d->possiblyUnlocked.testAndSetRelaxed(false, true))
                d->deref();
            return false;
        }
    }
    Q_ASSERT(this->d);
    return true;
}

/*!
    \internal
*/
void QBasicMutex::unlockInternal()
{
    QMutexPrivate *d = this->d;
    Q_ASSERT(d); //we must be locked
    Q_ASSERT(d != dummyLocked()); // testAndSetRelease(dummyLocked(), 0) failed

    if (d->recursive) {
        static_cast<QRecursiveMutexPrivate *>(d)->unlock();
        return;
    }

    if (d->waiters.fetchAndAddRelease(-QMutexPrivate::BigNumber) == 0) {
        //there is no one waiting on this mutex anymore, set the mutex as unlocked (d = 0)
        if (this->d.testAndSetRelease(d, 0)) {
            if (d->possiblyUnlocked && d->possiblyUnlocked.testAndSetRelaxed(true, false))
                d->deref();
        }
        d->derefWaiters(0);
    } else {
        d->derefWaiters(0);
        //there are thread waiting, transfer the lock.
        d->wakeUp();
    }
    d->deref();
}

//The freelist managment
namespace {
struct FreeListConstants : QFreeListDefaultConstants {
    enum { BlockCount = 4, MaxIndex=0xffff };
    static const int Sizes[BlockCount];
};
const int FreeListConstants::Sizes[FreeListConstants::BlockCount] = {
    16,
    128,
    1024,
    FreeListConstants::MaxIndex - (16-128-1024)
};

typedef QFreeList<QMutexPrivate, FreeListConstants> FreeList;
Q_GLOBAL_STATIC(FreeList, freelist);
}

QMutexPrivate *QMutexPrivate::allocate()
{
    int i = freelist()->next();
    QMutexPrivate *d = &(*freelist())[i];
    d->id = i;
    Q_ASSERT(d->refCount == 0);
    Q_ASSERT(!d->recursive);
    Q_ASSERT(!d->possiblyUnlocked);
    Q_ASSERT(d->waiters == 0);
    d->refCount = 1;
    return d;
}

void QMutexPrivate::release()
{
    Q_ASSERT(!recursive);
    Q_ASSERT(refCount == 0);
    Q_ASSERT(!possiblyUnlocked);
    Q_ASSERT(waiters == 0);
    freelist()->release(id);
}

// atomically substract "value" to the waiters, and remove the QMutexPrivate::BigNumber flag
void QMutexPrivate::derefWaiters(int value)
{
    int old_waiters;
    int new_waiters;
    do {
        old_waiters = waiters;
        new_waiters = old_waiters;
        if (new_waiters < 0) {
            new_waiters += QMutexPrivate::BigNumber;
        }
        new_waiters -= value;
    } while (!waiters.testAndSetRelaxed(old_waiters, new_waiters));
}
#endif

/*!
   \internal
 */
bool QRecursiveMutexPrivate::lock(int timeout) {
    Qt::HANDLE self = QThread::currentThreadId();
    if (owner == self) {
        ++count;
        Q_ASSERT_X(count != 0, "QMutex::lock", "Overflow in recursion counter");
        return true;
    }
    bool success = true;
    if (timeout == -1) {
        mutex.lock();
    } else {
        success = mutex.tryLock(timeout);
    }

    if (success)
        owner = self;
    return success;
}

/*!
   \internal
 */
void QRecursiveMutexPrivate::unlock()
{
    if (count > 0) {
        count--;
    } else {
        owner = 0;
        mutex.unlock();
    }
}


QT_END_NAMESPACE

#endif // QT_NO_THREAD
