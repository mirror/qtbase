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

#ifndef QANDROIDPLATFORMMENUITEM_H
#define QANDROIDPLATFORMMENUITEM_H
#include <qpa/qplatformmenu.h>

class QAndroidPlatformMenu;

class QAndroidPlatformMenuItem: public QPlatformMenuItem
{
public:
    QAndroidPlatformMenuItem();
    virtual void setTag(quintptr tag);
    virtual quintptr tag() const;

    virtual void setText(const QString &text);
    const QString &text();

    virtual void setIcon(const QIcon &icon);
    const QIcon &icon();

    virtual void setMenu(QPlatformMenu *menu);
    QAndroidPlatformMenu *menu();

    virtual void setVisible(bool isVisible);
    bool isVisible();

    virtual void setIsSeparator(bool isSeparator);
    bool isSeparator();

    virtual void setFont(const QFont &font);

    virtual void setRole(MenuRole role);
    MenuRole role();

    virtual void setCheckable(bool checkable);
    bool isCheckable();

    virtual void setChecked(bool isChecked);
    bool isChecked();

    virtual void setShortcut(const QKeySequence &shortcut);

    virtual void setEnabled(bool enabled);
    bool isEnabled();

private:
    quintptr m_tag;
    QString m_text;
    QIcon m_icon;
    QAndroidPlatformMenu *m_menu;
    bool m_isVisible;
    bool m_isSeparator;
    MenuRole m_role;
    bool m_isCheckable;
    bool m_isChecked;
    bool m_isEnabled;
};

#endif // QANDROIDPLATFORMMENUITEM_H
