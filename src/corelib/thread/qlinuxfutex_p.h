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

#ifndef QLINUXFUTEX_P_H
#define QLINUXFUTEX_P_H

#include <QtCore/qglobal.h>

#if defined(Q_OS_LINUX) && !defined(Q_LINUX_DISABLE_FUTEX)
# include <sys/syscall.h>
# include <errno.h>
# ifdef SYS_futex
#  define Q_LINUX_FUTEX_AVAILABLE
# endif
#endif

#ifdef Q_LINUX_FUTEX_AVAILABLE

// from linux/futex.h
#define FUTEX_WAIT		0
#define FUTEX_WAKE		1
#define FUTEX_PRIVATE_FLAG	128
#define FUTEX_CLOCK_REALTIME	256
#define FUTEX_CMD_MASK		~(FUTEX_PRIVATE_FLAG | FUTEX_CLOCK_REALTIME)
#define FUTEX_WAIT_PRIVATE	(FUTEX_WAIT | FUTEX_PRIVATE_FLAG)
#define FUTEX_WAKE_PRIVATE	(FUTEX_WAKE | FUTEX_PRIVATE_FLAG)

extern QBasicAtomicInt qt_linux_private_futexes_status;

inline int qt_futex_private_flag()
{
    // this function must be thread-safe and lock-free
    register int value = qt_linux_private_futexes_status.load();
    if (value == 0) {
        value = syscall(SYS_futex, &qt_linux_private_futexes_status, FUTEX_WAKE_PRIVATE, 42, 0, 0, 0);
        if (value >= 0)
            value = 1;
        qt_linux_private_futexes_status.testAndSetRelaxed(0, value);
    }
    return value < 0 ? 0 : FUTEX_PRIVATE_FLAG;
}

inline int qt_futex_wait(volatile void *addr, int val, const struct timespec *timeout = 0)
{
    return syscall(SYS_futex, addr, FUTEX_WAIT | qt_futex_private_flag(), val, timeout, 0, 0);
}

inline int qt_futex_wake(volatile void *addr, int wakeCount)
{
    return syscall(SYS_futex, addr, FUTEX_WAKE | qt_futex_private_flag(), wakeCount, 0, 0, 0);
}

#endif

#endif // QLINUXFUTEX_P_H
