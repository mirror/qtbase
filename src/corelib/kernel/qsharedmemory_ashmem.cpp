/****************************************************************************
**
** Copyright (C) 2012 BogDan Vatra <bogdan@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplatformdefs.h"

#include "qsharedmemory.h"
#include "qsharedmemory_p.h"
#include "qsystemsemaphore.h"
#include <qdir.h>
#include <qdebug.h>

#include <errno.h>

#ifndef QT_NO_SHAREDMEMORY
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/ashmem.h>

#include "private/qcore_unix_p.h"

#define ASHMEM_DEVICE "/" ASHMEM_NAME_DEF

QT_BEGIN_NAMESPACE

/*
 * ashmem_create_region - creates a new ashmem region and returns the file
 * descriptor, or <0 on error
 *
 * `name' is an optional label to give the region (visible in /proc/pid/maps)
 * `size' is the size of the region, in page-aligned bytes
 */
static int ashmem_create_region(const char *name, size_t size)
{
    int fd, ret;

    fd = qt_safe_open(ASHMEM_DEVICE, O_RDWR);
    if (fd < 0)
        return fd;

    if (name) {
        char buf[ASHMEM_NAME_LEN];

        strlcpy(buf, name, sizeof(buf));
        ret = ioctl(fd, ASHMEM_SET_NAME, buf);
        if (ret < 0)
            goto error;
    }

    ret = ioctl(fd, ASHMEM_SET_SIZE, size);
    if (ret < 0)
        goto error;

    return fd;

error:
    qt_safe_close(fd);
    return ret;
}
//PROT_READ | PROT_WRITE
static int ashmem_set_prot_region(int fd, int prot)
{
    return ioctl(fd, ASHMEM_SET_PROT_MASK, prot);
}

static int ashmem_pin_region(int fd, size_t offset, size_t len)
{
    struct ashmem_pin pin = { offset, len };
    return ioctl(fd, ASHMEM_PIN, &pin);
}

static int ashmem_unpin_region(int fd, size_t offset, size_t len)
{
    struct ashmem_pin pin = { offset, len };
    return ioctl(fd, ASHMEM_UNPIN, &pin);
}

static int ashmem_get_size_region(int fd)
{
  return ioctl(fd, ASHMEM_GET_SIZE, NULL);
}

QSharedMemoryPrivate::QSharedMemoryPrivate()
    : QObjectPrivate(), memory(0), size(0), error(QSharedMemory::NoError),
#ifndef QT_NO_SYSTEMSEMAPHORE
      systemSemaphore(QString()), lockedByMe(false),
#endif
      ashmem_fd(-1)
{
}

bool QSharedMemoryPrivate::cleanHandle()
{
    if (ashmem_fd < 0)
        return false;
    munmap(memory, size);
    return 0 == qt_safe_close(ashmem_fd);
}

bool QSharedMemoryPrivate::create(int sz)
{
    ashmem_fd = ashmem_create_region(makePlatformSafeKey(key).toLatin1(), sz);
    if (ashmem_fd < 0)
    {
        QString function = QLatin1String("QSharedMemory::create");
        setErrorString(function);
        return false;
    }
    size = sz;
    return true;
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
    if (ashmem_fd < 0)
        return false;
    int prot = (mode == QSharedMemory::ReadOnly) ? PROT_READ : (PROT_READ | PROT_WRITE);

    if (old_prot != prot)
    {
        if(memory && memory != MAP_FAILED)
            munmap(memory, size);

        if (ashmem_set_prot_region(ashmem_fd,prot) < 0)
            return false;

        if ((memory = mmap(0, size, prot, MAP_SHARED, ashmem_fd, 0)) == MAP_FAILED)
            return false;

        old_prot = prot;
    }
    else
        if (ashmem_pin_region(ashmem_fd, 0, 0) < 0)
            return false;

    return true;
}

bool QSharedMemoryPrivate::detach()
{
    if (ashmem_unpin_region(ashmem_fd, 0, 0) < 0)
        return false;
    return true;
}


QT_END_NAMESPACE

#endif // QT_NO_SHAREDMEMORY
