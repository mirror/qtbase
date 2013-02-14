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

#ifndef QATOMIC_ANDROID_H
#define QATOMIC_ANDROID_H

#if 0
// silence syncqt warnings
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
#endif

#if defined(QT_USE_FAST_ATOMICS) || defined(QT_BUILDING_QT)
# if defined(Q_PROCESSOR_ARM_V7)
#  include "QtCore/qatomic_armv7.h"
# elif defined(Q_PROCESSOR_ARM_V6)
#  include "QtCore/qatomic_armv6.h"
# elif defined(Q_PROCESSOR_ARM_V5)
#  include "QtCore/qatomic_armv5.h"
# elif defined (Q_PROCESSOR_X86)
#  include "QtCore/qatomic_x86.h"
# elif defined (Q_PROCESSOR_MIPS)
#  include "QtCore/qatomic_mips.h"
# else
# error "Unhandled Android platform"
# endif
#else
# if /*defined(Q_COMPILER_ATOMICS) && defined(Q_COMPILER_CONSTEXPR)
#  include <QtCore/qatomic_cxx11.h>
# elif */defined(Q_CC_GNU)
#  include <QtCore/qatomic_gcc.h>
# else
#  error "Unknown atomics"
# endif
#endif

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
QT_END_HEADER
#endif

#endif // QATOMIC_ANDROID_H
