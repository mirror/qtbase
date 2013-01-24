/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QACCESSIBLE_H
#define QACCESSIBLE_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtCore/qset.h>
#include <QtCore/qvector.h>
#include <QtCore/qvariant.h>
#include <QtGui/qcolor.h>
#include <QtGui/qevent.h>

#include <stdlib.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QAccessibleInterface;
class QAccessibleEvent;
class QWindow;

// We need to inherit QObject to expose the enums to QML.
class Q_GUI_EXPORT QAccessible
#ifndef Q_QDOC
        :public QObject
#endif
{
    Q_OBJECT
    Q_ENUMS(Role Event State)
public:

    enum Event {
        SoundPlayed          = 0x0001,
        Alert                = 0x0002,
        ForegroundChanged    = 0x0003,
        MenuStart            = 0x0004,
        MenuEnd              = 0x0005,
        PopupMenuStart       = 0x0006,
        PopupMenuEnd         = 0x0007,
        ContextHelpStart     = 0x000C,
        ContextHelpEnd       = 0x000D,
        DragDropStart        = 0x000E,
        DragDropEnd          = 0x000F,
        DialogStart          = 0x0010,
        DialogEnd            = 0x0011,
        ScrollingStart       = 0x0012,
        ScrollingEnd         = 0x0013,

        MenuCommand          = 0x0018,

        // Values from IAccessible2
        ActionChanged                    = 0x0101,
        ActiveDescendantChanged          = 0x0102,
        AttributeChanged                 = 0x0103,
        DocumentContentChanged           = 0x0104,
        DocumentLoadComplete             = 0x0105,
        DocumentLoadStopped              = 0x0106,
        DocumentReload                   = 0x0107,
        HyperlinkEndIndexChanged         = 0x0108,
        HyperlinkNumberOfAnchorsChanged  = 0x0109,
        HyperlinkSelectedLinkChanged     = 0x010A,
        HypertextLinkActivated           = 0x010B,
        HypertextLinkSelected            = 0x010C,
        HyperlinkStartIndexChanged       = 0x010D,
        HypertextChanged                 = 0x010E,
        HypertextNLinksChanged           = 0x010F,
        ObjectAttributeChanged           = 0x0110,
        PageChanged                      = 0x0111,
        SectionChanged                   = 0x0112,
        TableCaptionChanged              = 0x0113,
        TableColumnDescriptionChanged    = 0x0114,
        TableColumnHeaderChanged         = 0x0115,
        TableModelChanged                = 0x0116,
        TableRowDescriptionChanged       = 0x0117,
        TableRowHeaderChanged            = 0x0118,
        TableSummaryChanged              = 0x0119,
        TextAttributeChanged             = 0x011A,
        TextCaretMoved                   = 0x011B,
        // TextChanged = 0x011C, is deprecated in IA2, use TextUpdated
        TextColumnChanged                = 0x011D,
        TextInserted                     = 0x011E,
        TextRemoved                      = 0x011F,
        TextUpdated                      = 0x0120,
        TextSelectionChanged             = 0x0121,
        VisibleDataChanged               = 0x0122,

        ObjectCreated        = 0x8000,
        ObjectDestroyed      = 0x8001,
        ObjectShow           = 0x8002,
        ObjectHide           = 0x8003,
        ObjectReorder        = 0x8004,
        Focus                = 0x8005,
        Selection            = 0x8006,
        SelectionAdd         = 0x8007,
        SelectionRemove      = 0x8008,
        SelectionWithin      = 0x8009,
        StateChanged         = 0x800A,
        LocationChanged      = 0x800B,
        NameChanged          = 0x800C,
        DescriptionChanged   = 0x800D,
        ValueChanged         = 0x800E,
        ParentChanged        = 0x800F,
        HelpChanged          = 0x80A0,
        DefaultActionChanged = 0x80B0,
        AcceleratorChanged   = 0x80C0,

        InvalidEvent
    };

    // 64 bit enums seem hard on some platforms (windows...)
    // which makes using a bit field a sensible alternative
    struct State {
        // http://msdn.microsoft.com/en-us/library/ms697270.aspx
        quint64 disabled : 1; // used to be Unavailable
        quint64 selected : 1;
        quint64 focusable : 1;
        quint64 focused : 1;
        quint64 pressed : 1;
        quint64 checkable : 1;
        quint64 checked : 1;
        quint64 checkStateMixed : 1; // used to be Mixed
        quint64 readOnly : 1;
        quint64 hotTracked : 1;
        quint64 defaultButton : 1;
        quint64 expanded : 1;
        quint64 collapsed : 1;
        quint64 busy : 1;
        quint64 expandable : 1;
        quint64 marqueed : 1;
        quint64 animated : 1;
        quint64 invisible : 1;
        quint64 offscreen : 1;
        quint64 sizeable : 1;
        quint64 movable : 1;
        quint64 selfVoicing : 1;
        quint64 selectable : 1;
        quint64 linked : 1;
        quint64 traversed : 1;
        quint64 multiSelectable : 1;
        quint64 extSelectable : 1;
        quint64 passwordEdit : 1; // used to be Protected
        quint64 hasPopup : 1;
        quint64 modal : 1;

        // IA2 - we chose to not add some IA2 states for now
        // Below the ones that seem helpful
        quint64 active : 1;
        quint64 invalid : 1; // = defunct
        quint64 editable : 1;
        quint64 multiLine : 1;
        quint64 selectableText : 1;
        quint64 supportsAutoCompletion : 1;

        // quint64 horizontal : 1;
        // quint64 vertical : 1;
        // quint64 invalidEntry : 1;
        // quint64 managesDescendants : 1;
        // quint64 singleLine : 1; // we have multi line, this is redundant.
        // quint64 stale : 1;
        // quint64 transient : 1;
        // quint64 pinned : 1;

        // Apple - see http://mattgemmell.com/2010/12/19/accessibility-for-iphone-and-ipad-apps/
        // quint64 playsSound : 1;
        // quint64 summaryElement : 1;
        // quint64 updatesFrequently : 1;
        // quint64 adjustable : 1;
        // more and not included here: http://developer.apple.com/library/mac/#documentation/UserExperience/Reference/Accessibility_RoleAttribute_Ref/Attributes.html

        // MSAA
        // quint64 alertLow : 1;
        // quint64 alertMedium : 1;
        // quint64 alertHigh : 1;

        State() {
            memset(this, 0, sizeof(State));
        }
    };





    enum Role {
        NoRole         = 0x00000000,
        TitleBar       = 0x00000001,
        MenuBar        = 0x00000002,
        ScrollBar      = 0x00000003,
        Grip           = 0x00000004,
        Sound          = 0x00000005,
        Cursor         = 0x00000006,
        Caret          = 0x00000007,
        AlertMessage   = 0x00000008,
        Window         = 0x00000009,
        Client         = 0x0000000A,
        PopupMenu      = 0x0000000B,
        MenuItem       = 0x0000000C,
        ToolTip        = 0x0000000D,
        Application    = 0x0000000E,
        Document       = 0x0000000F,
        Pane           = 0x00000010,
        Chart          = 0x00000011,
        Dialog         = 0x00000012,
        Border         = 0x00000013,
        Grouping       = 0x00000014,
        Separator      = 0x00000015,
        ToolBar        = 0x00000016,
        StatusBar      = 0x00000017,
        Table          = 0x00000018,
        ColumnHeader   = 0x00000019,
        RowHeader      = 0x0000001A,
        Column         = 0x0000001B,
        Row            = 0x0000001C,
        Cell           = 0x0000001D,
        Link           = 0x0000001E,
        HelpBalloon    = 0x0000001F,
        Assistant      = 0x00000020,
        List           = 0x00000021,
        ListItem       = 0x00000022,
        Tree           = 0x00000023,
        TreeItem       = 0x00000024,
        PageTab        = 0x00000025,
        PropertyPage   = 0x00000026,
        Indicator      = 0x00000027,
        Graphic        = 0x00000028,
        StaticText     = 0x00000029,
        EditableText   = 0x0000002A,  // Editable, selectable, etc.
        Button         = 0x0000002B,
#ifndef Q_QDOC
        PushButton     = Button, // deprecated
#endif
        CheckBox       = 0x0000002C,
        RadioButton    = 0x0000002D,
        ComboBox       = 0x0000002E,
        // DropList       = 0x0000002F,
        ProgressBar    = 0x00000030,
        Dial           = 0x00000031,
        HotkeyField    = 0x00000032,
        Slider         = 0x00000033,
        SpinBox        = 0x00000034,
        Canvas         = 0x00000035,
        Animation      = 0x00000036,
        Equation       = 0x00000037,
        ButtonDropDown = 0x00000038,
        ButtonMenu     = 0x00000039,
        ButtonDropGrid = 0x0000003A,
        Whitespace     = 0x0000003B,
        PageTabList    = 0x0000003C,
        Clock          = 0x0000003D,
        Splitter       = 0x0000003E,
        // Reserved space in case MSAA roles needs to be added

        // Additional Qt roles where enum value does not map directly to MSAA:
        LayeredPane    = 0x00000080,
        Terminal       = 0x00000081,
        Desktop        = 0x00000082,
        UserRole       = 0x0000ffff
    };

    enum Text {
        Name         = 0,
        Description,
        Value,
        Help,
        Accelerator,
        DebugDescription,
        UserText     = 0x0000ffff
    };

    enum RelationFlag {
        Label         = 0x00000001,
        Labelled      = 0x00000002,
        Controller    = 0x00000004,
        Controlled    = 0x00000008,
        AllRelations  = 0xffffffff
    };
    Q_DECLARE_FLAGS(Relation, RelationFlag)

    enum InterfaceType
    {
        TextInterface,
        EditableTextInterface,
        ValueInterface,
        ActionInterface,
        ImageInterface,
        TableInterface,
        TableCellInterface
    };

    typedef QAccessibleInterface*(*InterfaceFactory)(const QString &key, QObject*);
    typedef void(*UpdateHandler)(QAccessibleEvent *event);
    typedef void(*RootObjectHandler)(QObject*);

    static void installFactory(InterfaceFactory);
    static void removeFactory(InterfaceFactory);
    static UpdateHandler installUpdateHandler(UpdateHandler);
    static RootObjectHandler installRootObjectHandler(RootObjectHandler);

    static QAccessibleInterface *queryAccessibleInterface(QObject *);
#if QT_DEPRECATED_SINCE(5, 0)
    QT_DEPRECATED static inline void updateAccessibility(QObject *object, int child, Event reason);
#endif
    static void updateAccessibility(QAccessibleEvent *event);

    static bool isActive();
    static void setRootObject(QObject *object);

    static void cleanup();

private:
    static UpdateHandler updateHandler;
    static RootObjectHandler rootObjectHandler;

    /*! @internal
      This class is purely a collection of enums and static functions,
      it is not supposed to be instantiated.
    */
    QAccessible() {}
};

Q_GUI_EXPORT bool operator==(const QAccessible::State &first, const QAccessible::State &second);

Q_DECLARE_OPERATORS_FOR_FLAGS(QAccessible::Relation)

class QAccessible2Interface;
class QAccessibleTextInterface;
class QAccessibleEditableTextInterface;
class QAccessibleValueInterface;
class QAccessibleActionInterface;
class QAccessibleImageInterface;
class QAccessibleTableInterface;
class QAccessibleTableCellInterface;

class Q_GUI_EXPORT QAccessibleInterface
{
public:
    virtual ~QAccessibleInterface() {}
    // check for valid pointers
    virtual bool isValid() const = 0;
    virtual QObject *object() const = 0;
    virtual QWindow *window() const;

    // relations
    virtual QVector<QPair<QAccessibleInterface*, QAccessible::Relation> > relations(QAccessible::Relation match = QAccessible::AllRelations) const;
    virtual QAccessibleInterface *focusChild() const;

    virtual QAccessibleInterface *childAt(int x, int y) const = 0;

    // navigation, hierarchy
    virtual QAccessibleInterface *parent() const = 0;
    virtual QAccessibleInterface *child(int index) const = 0;
    virtual int childCount() const = 0;
    virtual int indexOfChild(const QAccessibleInterface *) const = 0;

    // properties and state
    virtual QString text(QAccessible::Text t) const = 0;
    virtual void setText(QAccessible::Text t, const QString &text) = 0;
    virtual QRect rect() const = 0;
    virtual QAccessible::Role role() const = 0;
    virtual QAccessible::State state() const = 0;

    virtual QColor foregroundColor() const;
    virtual QColor backgroundColor() const;

    inline QAccessibleTextInterface *textInterface()
    { return reinterpret_cast<QAccessibleTextInterface *>(interface_cast(QAccessible::TextInterface)); }

    inline QAccessibleEditableTextInterface *editableTextInterface()
    { return reinterpret_cast<QAccessibleEditableTextInterface *>(interface_cast(QAccessible::EditableTextInterface)); }

    inline QAccessibleValueInterface *valueInterface()
    { return reinterpret_cast<QAccessibleValueInterface *>(interface_cast(QAccessible::ValueInterface)); }

    inline QAccessibleActionInterface *actionInterface()
    { return reinterpret_cast<QAccessibleActionInterface *>(interface_cast(QAccessible::ActionInterface)); }

    inline QAccessibleImageInterface *imageInterface()
    { return reinterpret_cast<QAccessibleImageInterface *>(interface_cast(QAccessible::ImageInterface)); }

    inline QAccessibleTableInterface *tableInterface()
    { return reinterpret_cast<QAccessibleTableInterface *>(interface_cast(QAccessible::TableInterface)); }

    inline QAccessibleTableCellInterface *tableCellInterface()
    { return reinterpret_cast<QAccessibleTableCellInterface *>(interface_cast(QAccessible::TableCellInterface)); }

    virtual void virtual_hook(int id, void *data);

    virtual void *interface_cast(QAccessible::InterfaceType)
    { return 0; }
private:
};

class Q_GUI_EXPORT QAccessibleEvent
{
    Q_DISABLE_COPY(QAccessibleEvent)
public:
    inline QAccessibleEvent(QObject *obj, QAccessible::Event typ)
        : m_type(typ), m_object(obj), m_child(-1)
    {
        Q_ASSERT(obj);
        // All events below have a subclass of QAccessibleEvent.
        // Use the subclass, since it's expected that it's possible to cast to that.
        Q_ASSERT(m_type != QAccessible::ValueChanged);
        Q_ASSERT(m_type != QAccessible::StateChanged);
        Q_ASSERT(m_type != QAccessible::TextCaretMoved);
        Q_ASSERT(m_type != QAccessible::TextSelectionChanged);
        Q_ASSERT(m_type != QAccessible::TextInserted);
        Q_ASSERT(m_type != QAccessible::TextRemoved);
        Q_ASSERT(m_type != QAccessible::TextUpdated);
        Q_ASSERT(m_type != QAccessible::TableModelChanged);
    }

    virtual ~QAccessibleEvent()
    {}

    QAccessible::Event type() const { return m_type; }
    QObject *object() const { return m_object; }

    void setChild(int chld) { m_child = chld; }
    int child() const { return m_child; }

    virtual QAccessibleInterface *accessibleInterface() const;

protected:
    QAccessible::Event m_type;
    QObject *m_object;
    int m_child;
};

class Q_GUI_EXPORT QAccessibleStateChangeEvent :public QAccessibleEvent
{
public:
    inline QAccessibleStateChangeEvent(QObject *obj, QAccessible::State state)
        : QAccessibleEvent(obj, QAccessible::InvalidEvent), m_changedStates(state)
    {
        m_type = QAccessible::StateChanged;
    }

    QAccessible::State changedStates() const {
        return m_changedStates;
    }

protected:
    QAccessible::State m_changedStates;
};

// Update the cursor and optionally the selection.
class Q_GUI_EXPORT QAccessibleTextCursorEvent : public QAccessibleEvent
{
public:
    inline QAccessibleTextCursorEvent(QObject *obj, int cursorPos)
        : QAccessibleEvent(obj, QAccessible::InvalidEvent)
      , m_cursorPosition(cursorPos)
    {
        m_type = QAccessible::TextCaretMoved;
    }

    void setCursorPosition(int position) { m_cursorPosition = position; }
    int cursorPosition() const { return m_cursorPosition; }

protected:
    int m_cursorPosition;
};

// Updates the cursor position simultaneously. By default the cursor is set to the end of the selection.
class Q_GUI_EXPORT QAccessibleTextSelectionEvent : public QAccessibleTextCursorEvent
{
public:
    inline QAccessibleTextSelectionEvent(QObject *obj, int start, int end)
        : QAccessibleTextCursorEvent(obj, (start == -1) ? 0 : end)
        , m_selectionStart(start), m_selectionEnd(end)
    {
        m_type = QAccessible::TextSelectionChanged;
    }

    void setSelection(int start, int end) {
        m_selectionStart = start;
        m_selectionEnd = end;
    }

    int selectionStart() const { return m_selectionStart; }
    int selectionEnd() const { return m_selectionEnd; }

protected:
        int m_selectionStart;
        int m_selectionEnd;
};

class Q_GUI_EXPORT QAccessibleTextInsertEvent : public QAccessibleTextCursorEvent
{
public:
    inline QAccessibleTextInsertEvent(QObject *obj, int position, const QString &text)
        : QAccessibleTextCursorEvent(obj, position + text.length())
        , m_position(position), m_text(text)
    {
        m_type = QAccessible::TextInserted;
    }

    QString textInserted() const {
        return m_text;
    }
    int changePosition() const {
        return m_position;
    }

protected:
    int m_position;
    QString m_text;
};

class Q_GUI_EXPORT QAccessibleTextRemoveEvent : public QAccessibleTextCursorEvent
{
public:
    inline QAccessibleTextRemoveEvent(QObject *obj, int position, const QString &text)
        : QAccessibleTextCursorEvent(obj, position)
        , m_position(position), m_text(text)
    {
        m_type = QAccessible::TextRemoved;
    }

    QString textRemoved() const {
        return m_text;
    }
    int changePosition() const {
        return m_position;
    }

protected:
    int m_position;
    QString m_text;
};

class Q_GUI_EXPORT QAccessibleTextUpdateEvent : public QAccessibleTextCursorEvent
{
public:
    inline QAccessibleTextUpdateEvent(QObject *obj, int position, const QString &oldText, const QString &text)
        : QAccessibleTextCursorEvent(obj, position + text.length())
        , m_position(position), m_oldText(oldText), m_text(text)
    {
        m_type = QAccessible::TextUpdated;
    }
    QString textRemoved() const {
        return m_oldText;
    }
    QString textInserted() const {
        return m_text;
    }
    int changePosition() const {
        return m_position;
    }

protected:
    int m_position;
    QString m_oldText;
    QString m_text;
};

class Q_GUI_EXPORT QAccessibleValueChangeEvent : public QAccessibleEvent
{
public:
    inline QAccessibleValueChangeEvent(QObject *obj, const QVariant &val)
        : QAccessibleEvent(obj, QAccessible::InvalidEvent)
      , m_value(val)
    {
        m_type = QAccessible::ValueChanged;
    }

    void setValue(const QVariant & val) { m_value= val; }
    QVariant value() const { return m_value; }

protected:
    QVariant m_value;
};

class Q_GUI_EXPORT QAccessibleTableModelChangeEvent : public QAccessibleEvent
{
public:
    enum ModelChangeType {
        ModelReset,
        DataChanged,
        RowsInserted,
        ColumnsInserted,
        RowsRemoved,
        ColumnsRemoved
    };

    inline QAccessibleTableModelChangeEvent(QObject *obj, ModelChangeType changeType)
        : QAccessibleEvent(obj, QAccessible::InvalidEvent)
        , m_modelChangeType(changeType)
        , m_firstRow(-1), m_firstColumn(-1), m_lastRow(-1), m_lastColumn(-1)
    {
        m_type = QAccessible::TableModelChanged;
    }
    void setModelChangeType(ModelChangeType changeType) { m_modelChangeType = changeType; }
    ModelChangeType modelChangeType() const { return m_modelChangeType; }

    void setFirstRow(int row) { m_firstRow = row; }
    void setFirstColumn(int col) { m_firstColumn = col; }
    void setLastRow(int row) { m_lastRow = row; }
    void setLastColumn(int col) { m_lastColumn = col; }
    int firstRow() const { return m_firstRow; }
    int firstColumn() const { return m_firstColumn; }
    int lastRow() const { return m_lastRow; }
    int lastColumn() const { return m_lastColumn; }

protected:
    ModelChangeType m_modelChangeType;
    int m_firstRow;
    int m_firstColumn;
    int m_lastRow;
    int m_lastColumn;
};

#define QAccessibleInterface_iid "org.qt-project.Qt.QAccessibleInterface"
Q_DECLARE_INTERFACE(QAccessibleInterface, QAccessibleInterface_iid)

Q_GUI_EXPORT const char *qAccessibleRoleString(QAccessible::Role role);
Q_GUI_EXPORT const char *qAccessibleEventString(QAccessible::Event event);

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug d, const QAccessibleInterface *iface);
Q_GUI_EXPORT QDebug operator<<(QDebug d, const QAccessibleEvent &ev);
#endif

#if QT_DEPRECATED_SINCE(5, 0)
inline void QAccessible::updateAccessibility(QObject *object, int child, Event reason)
{
    Q_ASSERT(object);

    QAccessibleEvent ev(object, reason);
    ev.setChild(child);
    updateAccessibility(&ev);
}
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QACCESSIBLE_H
