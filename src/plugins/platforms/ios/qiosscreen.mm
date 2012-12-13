/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qiosscreen.h"
#include "qioswindow.h"
#include <qpa/qwindowsysteminterface.h>

#include <sys/sysctl.h>

QT_BEGIN_NAMESPACE

/*!
    Returns the model identifier of the device.

    When running under the simulator, the identifier will not
    match the simulated device, but will be x86_64 or i386.
*/
static QString deviceModelIdentifier()
{
    static const char key[] = "hw.machine";

    size_t size;
    sysctlbyname(key, NULL, &size, NULL, 0);

    char value[size];
    sysctlbyname(key, &value, &size, NULL, 0);

    return QString::fromLatin1(value);
}

QIOSScreen::QIOSScreen(unsigned int screenIndex)
    : QPlatformScreen()
    , m_uiScreen([[UIScreen screens] objectAtIndex:qMin(screenIndex, [[UIScreen screens] count] - 1)])
    , m_orientationListener(0)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    QString deviceIdentifier = deviceModelIdentifier();

    if (deviceIdentifier == QStringLiteral("iPhone2,1") /* iPhone 3GS */
        || deviceIdentifier == QStringLiteral("iPod3,1") /* iPod touch 3G */) {
        m_depth = 18;
    } else {
        m_depth = 24;
    }

    int unscaledDpi = 163; // Regular iPhone DPI
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad
        && deviceIdentifier != QStringLiteral("iPad2,5") /* iPad Mini */) {
        unscaledDpi = 132;
    };

    // UIScreen does not report different bounds for different orientations. We
    // match this behavior by staying with a fixed QScreen geometry.
    CGRect bounds = [m_uiScreen bounds];
    m_geometry = QRect(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);

    const qreal millimetersPerInch = 25.4;
    m_physicalSize = QSizeF(m_geometry.size()) / unscaledDpi * millimetersPerInch;

    [pool release];
}

QIOSScreen::~QIOSScreen()
{
    [m_orientationListener release];
}

QRect QIOSScreen::geometry() const
{
    return m_geometry;
}

QRect QIOSScreen::availableGeometry() const
{
    CGRect frame = m_uiScreen.applicationFrame;
    return QRect(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
}

int QIOSScreen::depth() const
{
    return m_depth;
}

QImage::Format QIOSScreen::format() const
{
    return QImage::Format_ARGB32_Premultiplied;
}

QSizeF QIOSScreen::physicalSize() const
{
    return m_physicalSize;
}

Qt::ScreenOrientation QIOSScreen::nativeOrientation() const
{
    return Qt::PortraitOrientation;
}

Qt::ScreenOrientation QIOSScreen::orientation() const
{
    return m_orientationListener ? m_orientationListener->m_orientation : nativeOrientation();
}

void QIOSScreen::setOrientationUpdateMask(Qt::ScreenOrientations mask)
{
    if (m_orientationListener && mask == Qt::PrimaryOrientation) {
        [m_orientationListener release];
        m_orientationListener = 0;
    } else if (!m_orientationListener) {
        m_orientationListener = [[QIOSOrientationListener alloc] initWithQIOSScreen:this];
    }
}

UIScreen *QIOSScreen::uiScreen() const
{
    return m_uiScreen;
}

QT_END_NAMESPACE
