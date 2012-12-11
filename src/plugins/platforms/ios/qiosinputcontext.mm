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

#include "qiosinputcontext.h"
#include "qioswindow.h"
#include <QGuiApplication>

@interface QIOSKeyboardListener : NSObject {
@public
    QIOSInputContext *m_context;
    BOOL m_keyboardVisible;
}
@end

@implementation QIOSKeyboardListener

- (id)initWithQIOSInputContext:(QIOSInputContext *)context
{
    self = [super init];
    if (self) {
        m_context = context;
        m_keyboardVisible = NO;
        // After the keyboard became undockable (iOS5), UIKeyboardWillShow/UIKeyboardWillHide
        // no longer works for all cases. So listen to keyboard frame changes instead:
        [[NSNotificationCenter defaultCenter]
            addObserver:self
            selector:@selector(keyboardDidChangeFrame:)
            name:@"UIKeyboardDidChangeFrameNotification" object:nil];
    }
    return self;
}

- (void) dealloc
{
    [[NSNotificationCenter defaultCenter]
        removeObserver:self
        name:@"UIKeyboardDidChangeFrameNotification" object:nil];
    [super dealloc];
}

- (void) keyboardDidChangeFrame:(NSNotification *)notification
{
    CGRect frame;
    [[[notification userInfo] objectForKey:UIKeyboardFrameEndUserInfoKey] getValue:&frame];
    BOOL visible = CGRectIntersectsRect(frame, [UIScreen mainScreen].bounds);
    if (m_keyboardVisible != visible) {
        m_keyboardVisible = visible;
        m_context->emitInputPanelVisibleChanged();
    }
}

@end

QIOSInputContext::QIOSInputContext()
    : QPlatformInputContext(),
    m_keyboardListener([[QIOSKeyboardListener alloc] initWithQIOSInputContext:this])
{
}

QIOSInputContext::~QIOSInputContext()
{
    [m_keyboardListener release];
}

void QIOSInputContext::showInputPanel()
{
    if (isInputPanelVisible())
        return;

    // Documentation tells that one should call (and recall, if necessary) becomeFirstResponder/resignFirstResponder
    // to show/hide the keyboard. This is slightly inconvenient, since there exist no API to get the current first
    // responder. Rather than searching for it from the top, we assume that the view backing the focus window in Qt
    // is the best candidate as long as there exist no first responder from before (which the isInputPanelVisible
    // test on top should catch). Note that Qt will forward keyevents to whichever QObject that needs it, regardless of
    // which UIView the input actually came from. So in this respect, we're undermining iOS' responder chain.
    if (QWindow *window = QGuiApplication::focusWindow()) {
        QIOSWindow *qiosWindow = static_cast<QIOSWindow *>(window->handle());
        [qiosWindow->nativeView() becomeFirstResponder];
    }
}

void QIOSInputContext::hideInputPanel()
{
    if (!isInputPanelVisible())
        return;

    if (QWindow *window = QGuiApplication::focusWindow()) {
        QIOSWindow *qiosWindow = static_cast<QIOSWindow *>(window->handle());
        [qiosWindow->nativeView() resignFirstResponder];
    }
}

bool QIOSInputContext::isInputPanelVisible() const
{
    return m_keyboardListener->m_keyboardVisible;
}
