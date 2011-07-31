/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>

#include <qatomic.h>
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qmutex.h>
#include <qthread.h>
#include <qwaitcondition.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QMutex : public QObject
{
    Q_OBJECT

public:
    tst_QMutex();
    virtual ~tst_QMutex();

private slots:
    void tryLock();
    void lock_unlock_locked_tryLock();
    void stressTest();
    void tryLockRace();
    void qtbug16115_trylock();
    void moreStress();
};

static const int iterations = 100;

tst_QMutex::tst_QMutex()

{
}

tst_QMutex::~tst_QMutex()
{
}

QAtomicInt lockCount(0);
QMutex normalMutex, recursiveMutex(QMutex::Recursive);
QSemaphore testsTurn;
QSemaphore threadsTurn;

enum { waitTime = 100 };

void tst_QMutex::tryLock()
{
    // test non-recursive mutex
    {
        class Thread : public QThread
        {
        public:
            void run()
            {
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!normalMutex.tryLock());
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(normalMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(!normalMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                normalMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QTime timer;
                timer.start();
                QVERIFY(!normalMutex.tryLock(waitTime));
                QVERIFY(timer.elapsed() >= waitTime);
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(normalMutex.tryLock(waitTime));
                QVERIFY(timer.elapsed() <= waitTime);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                timer.start();
                QVERIFY(!normalMutex.tryLock(waitTime));
                QVERIFY(timer.elapsed() >= waitTime);
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                normalMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!normalMutex.tryLock(0));
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(normalMutex.tryLock(0));
                QVERIFY(timer.elapsed() < waitTime);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(!normalMutex.tryLock(0));
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                normalMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
            }
        };

        Thread thread;
        thread.start();

        // thread can't acquire lock
        testsTurn.acquire();
        normalMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        threadsTurn.release();

        // thread can acquire lock
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        normalMutex.unlock();
        threadsTurn.release();

        // thread can't acquire lock, timeout = waitTime
        testsTurn.acquire();
        normalMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        threadsTurn.release();

        // thread can acquire lock, timeout = waitTime
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        normalMutex.unlock();
        threadsTurn.release();

        // thread can't acquire lock, timeout = 0
        testsTurn.acquire();
        normalMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        threadsTurn.release();

        // thread can acquire lock, timeout = 0
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        normalMutex.unlock();
        threadsTurn.release();

        // wait for thread to finish
        testsTurn.acquire();
        threadsTurn.release();
        thread.wait();
    }

    // test recursive mutex
    {
        class Thread : public QThread
        {
        public:
            void run()
            {
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!recursiveMutex.tryLock());
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(recursiveMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(recursiveMutex.tryLock());
                QVERIFY(lockCount.testAndSetRelaxed(1, 2));
                QVERIFY(lockCount.testAndSetRelaxed(2, 1));
                recursiveMutex.unlock();
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                recursiveMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QTime timer;
                timer.start();
                QVERIFY(!recursiveMutex.tryLock(waitTime));
                QVERIFY(timer.elapsed() >= waitTime);
                QVERIFY(!recursiveMutex.tryLock(0));
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(recursiveMutex.tryLock(waitTime));
                QVERIFY(timer.elapsed() <= waitTime);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(recursiveMutex.tryLock(waitTime));
                QVERIFY(lockCount.testAndSetRelaxed(1, 2));
                QVERIFY(lockCount.testAndSetRelaxed(2, 1));
                recursiveMutex.unlock();
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                recursiveMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
                QVERIFY(!recursiveMutex.tryLock(0));
                QVERIFY(!recursiveMutex.tryLock(0));
                testsTurn.release();

                threadsTurn.acquire();
                timer.start();
                QVERIFY(recursiveMutex.tryLock(0));
                QVERIFY(timer.elapsed() < waitTime);
                QVERIFY(lockCount.testAndSetRelaxed(0, 1));
                QVERIFY(recursiveMutex.tryLock(0));
                QVERIFY(lockCount.testAndSetRelaxed(1, 2));
                QVERIFY(lockCount.testAndSetRelaxed(2, 1));
                recursiveMutex.unlock();
                QVERIFY(lockCount.testAndSetRelaxed(1, 0));
                recursiveMutex.unlock();
                testsTurn.release();

                threadsTurn.acquire();
            }
        };

        Thread thread;
        thread.start();

        // thread can't acquire lock
        testsTurn.acquire();
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 2));
        threadsTurn.release();

        // thread can acquire lock
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(2, 1));
        recursiveMutex.unlock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        recursiveMutex.unlock();
        threadsTurn.release();

        // thread can't acquire lock, timeout = waitTime
        testsTurn.acquire();
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 2));
        threadsTurn.release();

        // thread can acquire lock, timeout = waitTime
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(2, 1));
        recursiveMutex.unlock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        recursiveMutex.unlock();
        threadsTurn.release();

        // thread can't acquire lock, timeout = 0
        testsTurn.acquire();
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(0, 1));
        recursiveMutex.lock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 2));
        threadsTurn.release();

        // thread can acquire lock, timeout = 0
        testsTurn.acquire();
        QVERIFY(lockCount.testAndSetRelaxed(2, 1));
        recursiveMutex.unlock();
        QVERIFY(lockCount.testAndSetRelaxed(1, 0));
        recursiveMutex.unlock();
        threadsTurn.release();

        // stop thread
        testsTurn.acquire();
        threadsTurn.release();
        thread.wait();
    }
}

class mutex_Thread : public QThread
{
public:
    QMutex mutex;
    QWaitCondition cond;

    QMutex &test_mutex;

    inline mutex_Thread(QMutex &m) : test_mutex(m) { }

    void run()
    {
	test_mutex.lock();

	mutex.lock();
	for (int i = 0; i < iterations; ++i) {
	    cond.wakeOne();
	    cond.wait(&mutex);
	}
	mutex.unlock();

    	test_mutex.unlock();
    }
};

class rmutex_Thread : public QThread
{
public:
    QMutex mutex;
    QWaitCondition cond;

    QMutex &test_mutex;

    inline rmutex_Thread(QMutex &m) : test_mutex(m) { }

    void run()
    {
	test_mutex.lock();
	test_mutex.lock();
	test_mutex.lock();
	test_mutex.lock();

	mutex.lock();
	for (int i = 0; i < iterations; ++i) {
	    cond.wakeOne();
	    cond.wait(&mutex);
	}
	mutex.unlock();

    	test_mutex.unlock();
    	test_mutex.unlock();
    	test_mutex.unlock();
    	test_mutex.unlock();
    }
};

void tst_QMutex::lock_unlock_locked_tryLock()
{
    // normal mutex
    QMutex mutex;
    mutex_Thread thread(mutex);

    QMutex rmutex(QMutex::Recursive);
    rmutex_Thread rthread(rmutex);

    for (int i = 0; i < iterations; ++i) {
	// normal mutex
	QVERIFY(mutex.tryLock());
	mutex.unlock();

	thread.mutex.lock();
	thread.start();

	for (int j = 0; j < iterations; ++j) {
	    QVERIFY(thread.cond.wait(&thread.mutex, 10000));
	    QVERIFY(!mutex.tryLock());

	    thread.cond.wakeOne();
	}

	thread.mutex.unlock();

	QVERIFY(thread.wait(10000));
	QVERIFY(mutex.tryLock());

	mutex.unlock();

    	// recursive mutex
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());

	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();

	rthread.mutex.lock();
	rthread.start();

	for (int k = 0; k < iterations; ++k) {
	    QVERIFY(rthread.cond.wait(&rthread.mutex, 10000));
	    QVERIFY(!rmutex.tryLock());

	    rthread.cond.wakeOne();
	}

	rthread.mutex.unlock();

	QVERIFY(rthread.wait(10000));
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());
	QVERIFY(rmutex.tryLock());

	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
	rmutex.unlock();
    }
}

enum { one_minute = 6 * 1000, //not really one minute, but else it is too long.
       threadCount = 10 };

class StressTestThread : public QThread
{
    QTime t;
public:
    static QBasicAtomicInt lockCount;
    static QBasicAtomicInt sentinel;
    static QMutex mutex;
    static int errorCount;
    void start()
    {
        t.start();
        QThread::start();
    }
    void run()
    {
        while (t.elapsed() < one_minute) {
            mutex.lock();
            if (sentinel.ref()) ++errorCount;
            if (!sentinel.deref()) ++errorCount;
            lockCount.ref();
            mutex.unlock();
            if (mutex.tryLock()) {
                if (sentinel.ref()) ++errorCount;
                if (!sentinel.deref()) ++errorCount;
                lockCount.ref();
                mutex.unlock();
            }
        }
    }
};
QMutex StressTestThread::mutex;
QBasicAtomicInt StressTestThread::lockCount = Q_BASIC_ATOMIC_INITIALIZER(0);
QBasicAtomicInt StressTestThread::sentinel = Q_BASIC_ATOMIC_INITIALIZER(-1);
int StressTestThread::errorCount = 0;

void tst_QMutex::stressTest()
{
    StressTestThread threads[threadCount];
    for (int i = 0; i < threadCount; ++i)
        threads[i].start();
    QVERIFY(threads[0].wait(one_minute + 10000));
    for (int i = 1; i < threadCount; ++i)
        QVERIFY(threads[i].wait(10000));
    QCOMPARE(StressTestThread::errorCount, 0);
    qDebug("locked %d times", int(StressTestThread::lockCount.load()));
}

class TryLockRaceThread : public QThread
{
public:
    static QMutex mutex;

    void run()
    {
        QTime t;
        t.start();
        do {
            if (mutex.tryLock())
                mutex.unlock();
        } while (t.elapsed() < one_minute/2);
    }
};
QMutex TryLockRaceThread::mutex;

void tst_QMutex::tryLockRace()
{
    // mutex not in use, should be able to lock it
    QVERIFY(TryLockRaceThread::mutex.tryLock());
    TryLockRaceThread::mutex.unlock();

    // try to break tryLock
    TryLockRaceThread thread[threadCount];
    for (int i = 0; i < threadCount; ++i)
        thread[i].start();
    for (int i = 0; i < threadCount; ++i)
        QVERIFY(thread[i].wait());

    // mutex not in use, should be able to lock it
    QVERIFY(TryLockRaceThread::mutex.tryLock());
    TryLockRaceThread::mutex.unlock();
}

// Variable that will be protected by the mutex. Volatile so that the
// the optimiser doesn't mess with it based on the increment-then-decrement
// usage pattern.
static volatile int qtbug16115_trylock_counter;
// Counter for how many times the protected variable has an incorrect value.
static int qtbug16115_failure_count = 0;

void tst_QMutex::qtbug16115_trylock()
{
    //Used to deadlock on unix
    struct TrylockThread : QThread {
        TrylockThread(QMutex &mut) : mut(mut) {}
        QMutex &mut;
        void run() {
            for (int i = 0; i < 100000; ++i) {
                if (mut.tryLock(0)) {
                    if ((++qtbug16115_trylock_counter) != 1)
                        ++qtbug16115_failure_count;
                    if ((--qtbug16115_trylock_counter) != 0)
                        ++qtbug16115_failure_count;
                    mut.unlock();
                }
            }
        }
    };
    QMutex mut;
    TrylockThread t1(mut);
    TrylockThread t2(mut);
    TrylockThread t3(mut);
    t1.start();
    t2.start();
    t3.start();

    for (int i = 0; i < 100000; ++i) {
        mut.lock();
        if ((++qtbug16115_trylock_counter) != 1)
            ++qtbug16115_failure_count;
        if ((--qtbug16115_trylock_counter) != 0)
            ++qtbug16115_failure_count;
        mut.unlock();
    }
    t1.wait();
    t2.wait();
    t3.wait();
    QCOMPARE(qtbug16115_failure_count, 0);
}


class MoreStressTestThread : public QThread
{
    QTime t;
public:
    static QAtomicInt lockCount;
    static QAtomicInt sentinel[threadCount];
    static QMutex mutex[threadCount];
    static QAtomicInt errorCount;
    void start()
    {
        t.start();
        QThread::start();
    }
    void run()
    {
        quint64 i = 0;
        while (t.elapsed() < one_minute) {
            i++;
            uint nb = (i * 9 + lockCount * 13) % threadCount;
            QMutexLocker locker(&mutex[nb]);
            if (sentinel[nb]) errorCount.ref();
            if (sentinel[nb].fetchAndAddRelaxed(5)) errorCount.ref();
            if (!sentinel[nb].testAndSetRelaxed(5, 0)) errorCount.ref();
            if (sentinel[nb]) errorCount.ref();
            lockCount.ref();
            nb = (nb * 17 + i * 5 + lockCount * 3) % threadCount;
            if (mutex[nb].tryLock()) {
                if (sentinel[nb]) errorCount.ref();
                if (sentinel[nb].fetchAndAddRelaxed(16)) errorCount.ref();
                if (!sentinel[nb].testAndSetRelaxed(16, 0)) errorCount.ref();
                if (sentinel[nb]) errorCount.ref();
                lockCount.ref();
                mutex[nb].unlock();
            }
            nb = (nb * 15 + i * 47 + lockCount * 31) % threadCount;
            if (mutex[nb].tryLock(2)) {
                if (sentinel[nb]) errorCount.ref();
                if (sentinel[nb].fetchAndAddRelaxed(53)) errorCount.ref();
                if (!sentinel[nb].testAndSetRelaxed(53, 0)) errorCount.ref();
                if (sentinel[nb]) errorCount.ref();
                lockCount.ref();
                mutex[nb].unlock();
            }
        }
    }
};
QMutex MoreStressTestThread::mutex[threadCount];
QAtomicInt MoreStressTestThread::lockCount;
QAtomicInt MoreStressTestThread::sentinel[threadCount];
QAtomicInt MoreStressTestThread::errorCount = 0;

void tst_QMutex::moreStress()
{
    MoreStressTestThread threads[threadCount];
    for (int i = 0; i < threadCount; ++i)
        threads[i].start();
    QVERIFY(threads[0].wait(one_minute + 10000));
    for (int i = 1; i < threadCount; ++i)
        QVERIFY(threads[i].wait(10000));
    qDebug("locked %d times", int(MoreStressTestThread::lockCount));
    QCOMPARE(int(MoreStressTestThread::errorCount), 0);
}


QTEST_MAIN(tst_QMutex)
#include "tst_qmutex.moc"
