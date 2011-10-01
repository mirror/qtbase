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

#ifndef QOBJECT_H
#define QOBJECT_H

#ifndef QT_NO_QOBJECT

#include <QtCore/qobjectdefs.h>
#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#ifdef QT_INCLUDE_COMPAT
#include <QtCore/qcoreevent.h>
#endif
#include <QtCore/qscopedpointer.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEvent;
class QTimerEvent;
class QChildEvent;
struct QMetaObject;
class QVariant;
class QObjectPrivate;
class QObject;
class QThread;
class QWidget;
#ifndef QT_NO_REGEXP
class QRegExp;
#endif
#ifndef QT_NO_USERDATA
class QObjectUserData;
#endif

namespace QtPrivate {
    template <typename T> struct RemoveRef { typedef T Type; };
    template <typename T> struct RemoveRef<const T&> { typedef T Type; };
    template <typename T> struct RemoveRef<T&> { typedef T Type; };
    template <typename T> struct RemoveConstRef { typedef T Type; };
    template <typename T> struct RemoveConstRef<const T&> { typedef T Type; };

    template <typename Head, typename Tail> struct List { typedef Head Car; typedef Tail Cdr; };
    template <typename L, int N> struct List_Left { typedef List<typename L::Car, typename List_Left<typename L::Cdr, N - 1>::Value > Value; };
    template <typename L> struct List_Left<L,0> { typedef void Value; };
    template <typename L, int N> struct List_Select { typedef typename List_Select<typename L::Cdr, N - 1>::Value Value; };
    template <typename L> struct List_Select<L,0> { typedef typename L::Car Value; };

    //trick to set the return value of a slot that works even if the signal or the slot returns void
    template <typename T>
    struct ApplyReturnValue {
        void *data;
        ApplyReturnValue(void *data) : data(data) {}
    };
    template<typename T, typename U>
    void operator,(T value, const ApplyReturnValue<U> &container) {
        *reinterpret_cast<U*>(container.data) = value;
    }
    template<typename T>
    void operator,(T, const ApplyReturnValue<void> &) {}

    //FunctionPointer traints
    template<typename Func> struct FunctionPointer { enum {ArgumentCount = 1000}; };
    template<class Obj, typename Ret> struct FunctionPointer<Ret (Obj::*) ()>
    {
        typedef Obj Object;
        typedef void Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) ();
        enum {ArgumentCount = 0};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) { (o->*f)(), ApplyReturnValue<R>(arg[0]); }
    };
    template<class Obj, typename Ret, typename Arg1> struct FunctionPointer<Ret (Obj::*) (Arg1)>
    {
        typedef Obj Object;
        typedef List<Arg1, void> Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1);
        enum {ArgumentCount = 1};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<typename Args::Car>::Type *>(arg[1]))), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, void> >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2);
        enum {ArgumentCount = 2};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2, typename Arg3> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2, Arg3)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, List<Arg3, void> > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2, Arg3);
        enum {ArgumentCount = 3};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2, Arg3, Arg4)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, List<Arg3, List<Arg4, void> > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2, Arg3, Arg4);
        enum {ArgumentCount = 4};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2, Arg3, Arg4, Arg5)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, List<Arg3, List<Arg4, List<Arg5, void> > > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2, Arg3, Arg4, Arg5);
        enum {ArgumentCount = 5};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 4>::Value>::Type *>(arg[5])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, List<Arg3, List<Arg4, List<Arg5, List<Arg6, void> > > > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);
        enum {ArgumentCount = 6};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 4>::Value>::Type *>(arg[5]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 5>::Value>::Type *>(arg[6])), ApplyReturnValue<R>(arg[0]);
        }
    };

    template<typename Ret> struct FunctionPointer<Ret (*) ()>
    {
        typedef void Arguments;
        typedef Ret (*Function) ();
        typedef Ret ReturnType;
        enum {ArgumentCount = 0};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) { f(), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1> struct FunctionPointer<Ret (*) (Arg1)>
    {
        typedef List<Arg1, void> Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1);
        enum {ArgumentCount = 1};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg)
        { f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1])), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1, typename Arg2> struct FunctionPointer<Ret (*) (Arg1, Arg2)>
    {
        typedef List<Arg1, List<Arg2, void> > Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2);
        enum {ArgumentCount = 2};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
              *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1, typename Arg2, typename Arg3> struct FunctionPointer<Ret (*) (Arg1, Arg2, Arg3)>
    {
        typedef List<Arg1, List<Arg2, List<Arg3, void> > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2, Arg3);
        enum {ArgumentCount = 3};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(       *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct FunctionPointer<Ret (*) (Arg1, Arg2, Arg3, Arg4)>
    {
        typedef List<Arg1, List<Arg2, List<Arg3, List<Arg4, void> > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2, Arg3, Arg4);
        enum {ArgumentCount = 4};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(       *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct FunctionPointer<Ret (*) (Arg1, Arg2, Arg3, Arg4, Arg5)>
    {
        typedef List<Arg1, List<Arg2, List<Arg3,
        List<Arg4, List<Arg5, void > > > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2, Arg3, Arg4, Arg5);
        enum {ArgumentCount = 5};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(       *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[5])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> struct FunctionPointer<Ret (*) (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>
    {
        typedef List<Arg1, List<Arg2, List<Arg3, List<Arg4, List<Arg5, List<Arg6, void> > > > > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);
        enum {ArgumentCount = 6};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(       *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[4]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[5]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 3>::Value>::Type *>(arg[6])), ApplyReturnValue<R>(arg[0]);
        }
    };

    template<typename F, int N> struct Functor;
    template<typename Function> struct Functor<Function, 0>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) { f(), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Function> struct Functor<Function, 1>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 2>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 3>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[4])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 4>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[3]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[4])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 5>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[3]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[4]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[5])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 6>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[3]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[4]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[5]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[6])), ApplyReturnValue<R>(arg[0]);
        }
    };

    // Logic that check if the arguments of the slot matches the argument of the signal.
    template<typename T, bool B> struct CheckCompatibleArgumentsHelper {};
    template<typename T> struct CheckCompatibleArgumentsHelper<T, true> : T {};
    template<typename A1, typename A2> struct AreCompatiblmeArgument {
        static int test(A2);
        static char test(...);
        static A2 dummy();
        enum { value = sizeof(test(dummy())) == sizeof(int) };
    };
    template<typename A1, typename A2> struct AreCompatiblmeArgument<A1, A2&> { enum { value = false }; };
    template<typename A> struct AreCompatiblmeArgument<A&, A&> { enum { value = true }; };

    template <typename List1, typename List2> struct CheckCompatibleArguments{};
    template <> struct CheckCompatibleArguments<void, void> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename List1> struct CheckCompatibleArguments<List1, void> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename Arg1, typename Arg2, typename Tail1, typename Tail2> struct CheckCompatibleArguments<List<Arg1, Tail1>, List<Arg2, Tail2> >
        : CheckCompatibleArgumentsHelper<CheckCompatibleArguments<Tail1, Tail2>, AreCompatiblmeArgument<
            typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value > {};


    //Generate the array of metatype id, for the queued connection
    template <typename ArgList> struct TypesAreDeclaredMetaType { enum { Value = false }; };
    template <> struct TypesAreDeclaredMetaType<void> { enum { Value = true }; };
    template <typename Arg, typename Tail> struct TypesAreDeclaredMetaType<List<Arg, Tail> > { enum { Value = QMetaTypeId2<Arg>::Defined && TypesAreDeclaredMetaType<Tail>::Value }; };

    template <typename ArgList, bool Declared = TypesAreDeclaredMetaType<ArgList>::Value > struct ConnectionTypes
    { static const int *types() { return 0; } };
    template <> struct ConnectionTypes<void, true>
    { static const int *types() { static const int t[1] = { 0 }; return t; } };
    template <typename Arg1> struct ConnectionTypes<List<Arg1, void>, true>
    { static const int *types() { static const int t[2] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2> struct ConnectionTypes<List<Arg1, List<Arg2, void> >, true>
    { static const int *types() { static const int t[3] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2, typename Arg3> struct ConnectionTypes<List<Arg1, List<Arg2,  List<Arg3, void> > >, true>
    { static const int *types() { static const int t[4] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(),
                                                            QtPrivate::QMetaTypeIdHelper<Arg3>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct ConnectionTypes<List<Arg1, List<Arg2,  List<Arg3, List<Arg4, void> > > >, true>
    { static const int *types() { static const int t[4] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(),
                QtPrivate::QMetaTypeIdHelper<Arg3>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg4>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct ConnectionTypes<List<Arg1, List<Arg2,  List<Arg3, List<Arg4, List<Arg5, void> > > > >, true>
    { static const int *types() { static const int t[4] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(),
                QtPrivate::QMetaTypeIdHelper<Arg3>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg4>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg5>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
    struct ConnectionTypes<List<Arg1, List<Arg2,  List<Arg3, List<Arg4, List<Arg5, List<Arg6, void> > > > > >, true>
    { static const int *types() { static const int t[4] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(),
            QtPrivate::QMetaTypeIdHelper<Arg3>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg4>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg5>::qt_metatype_id(),
            QtPrivate::QMetaTypeIdHelper<Arg6>::qt_metatype_id(), 0 }; return t; } };
}

typedef QList<QObject*> QObjectList;

Q_CORE_EXPORT void qt_qFindChildren_helper(const QObject *parent, const QString &name, const QRegExp *re,
                                           const QMetaObject &mo, QList<void *> *list, Qt::FindChildOptions options);
Q_CORE_EXPORT QObject *qt_qFindChild_helper(const QObject *parent, const QString &name, const QMetaObject &mo, Qt::FindChildOptions options);

class
#if defined(__INTEL_COMPILER) && defined(Q_OS_WIN)
Q_CORE_EXPORT
#endif
QObjectData {
public:
    virtual ~QObjectData() = 0;
    QObject *q_ptr;
    QObject *parent;
    QObjectList children;

    uint isWidget : 1;
    uint pendTimer : 1;
    uint blockSig : 1;
    uint wasDeleted : 1;
    uint ownObjectName : 1;
    uint sendChildEvents : 1;
    uint receiveChildEvents : 1;
    uint inEventHandler : 1; //only used if QT_JAMBI_BUILD
    uint inThreadChangeEvent : 1;
    uint hasGuards : 1; //true iff there is one or more QPointer attached to this object
    uint isWindow : 1; //for QWindow
    uint unused : 21;
    int postedEvents;
    QMetaObject *metaObject; // assert dynamic
};


class Q_CORE_EXPORT QObject
{
    Q_OBJECT
    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
    Q_DECLARE_PRIVATE(QObject)

public:
    Q_INVOKABLE explicit QObject(QObject *parent=0);
    virtual ~QObject();

    virtual bool event(QEvent *);
    virtual bool eventFilter(QObject *, QEvent *);

#ifdef qdoc
    static QString tr(const char *sourceText, const char *comment = 0, int n = -1);
    static QString trUtf8(const char *sourceText, const char *comment = 0, int n = -1);
    virtual const QMetaObject *metaObject() const;
    static const QMetaObject staticMetaObject;
#endif
#ifdef QT_NO_TRANSLATION
    static QString tr(const char *sourceText, const char *, int)
        { return QString::fromLatin1(sourceText); }
    static QString tr(const char *sourceText, const char * = 0)
        { return QString::fromLatin1(sourceText); }
#ifndef QT_NO_TEXTCODEC
    static QString trUtf8(const char *sourceText, const char *, int)
        { return QString::fromUtf8(sourceText); }
    static QString trUtf8(const char *sourceText, const char * = 0)
        { return QString::fromUtf8(sourceText); }
#endif
#endif //QT_NO_TRANSLATION

    QString objectName() const;
    void setObjectName(const QString &name);

    inline bool isWidgetType() const { return d_ptr->isWidget; }
    inline bool isWindowType() const { return d_ptr->isWindow; }

    inline bool signalsBlocked() const { return d_ptr->blockSig; }
    bool blockSignals(bool b);

    QThread *thread() const;
    void moveToThread(QThread *thread);

    int startTimer(int interval);
    void killTimer(int id);

    template<typename T>
    inline T findChild(const QString &aName = QString(), Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    { return static_cast<T>(qt_qFindChild_helper(this, aName, reinterpret_cast<T>(0)->staticMetaObject, options)); }

    template<typename T>
    inline QList<T> findChildren(const QString &aName = QString(), Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    {
        QList<T> list;
        union {
            QList<T> *typedList;
            QList<void *> *voidList;
        } u;
        u.typedList = &list;
        qt_qFindChildren_helper(this, aName, 0, reinterpret_cast<T>(0)->staticMetaObject, u.voidList, options);
        return list;
    }

#ifndef QT_NO_REGEXP
    template<typename T>
    inline QList<T> findChildren(const QRegExp &re, Qt::FindChildOptions options = Qt::FindChildrenRecursively) const
    {
        QList<T> list;
        union {
            QList<T> *typedList;
            QList<void *> *voidList;
        } u;
        u.typedList = &list;
        qt_qFindChildren_helper(this, QString(), &re, reinterpret_cast<T>(0)->staticMetaObject, u.voidList, options);
        return list;
    }
#endif

    inline const QObjectList &children() const { return d_ptr->children; }

    void setParent(QObject *);
    void installEventFilter(QObject *);
    void removeEventFilter(QObject *);

    typedef QMetaObject::Connection Connection;

    static Connection connect(const QObject *sender, const char *signal,
                        const QObject *receiver, const char *member, Qt::ConnectionType = Qt::AutoConnection);

    static Connection connect(const QObject *sender, const QMetaMethod &signal,
                        const QObject *receiver, const QMetaMethod &method,
                        Qt::ConnectionType type = Qt::AutoConnection);

    inline Connection connect(const QObject *sender, const char *signal,
                        const char *member, Qt::ConnectionType type = Qt::AutoConnection) const;

private:
    struct QSlotObjectBase {
        QAtomicInt ref;
        QSlotObjectBase() : ref(1) {}
        virtual ~QSlotObjectBase();
        virtual void call(QObject *receiver, void **a) = 0;
    };
    template<typename Func, typename Args, typename R> struct QSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        QSlotObject(Func f) : function(f) {};
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, static_cast<typename FuncType::Object *>(receiver), a);
        }
    };
    template<typename Func, typename Args, typename R> struct QStaticSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        QStaticSlotObject(Func f) : function(f) {}
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, receiver, a);
        }
    };
    template<typename Func, int N, typename Args, typename R> struct QFunctorSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::Functor<Func, N> FuncType;
        Func function;
        QFunctorSlotObject(const Func &f) : function(f) {}
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, receiver, a);
        }
    };

    static Connection connectImpl(const QObject *sender, void **signal, const QObject *receiver, QSlotObjectBase *slot,
                            Qt::ConnectionType type, const int *types, const QMetaObject *mo1);

public:
    //Connect a signal to a pointer to qobject member function,  dirrect connection to the pointer
    template <typename Func1, typename Func2>
    static inline Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                     const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                                     Qt::ConnectionType type = Qt::AutoConnection)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;
        reinterpret_cast<typename SignalType::Object *>(0)->qt_check_for_QOBJECT_macro(*reinterpret_cast<typename SignalType::Object *>(0));

        //compilation error if the arguments does not match.
        typedef typename QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::IncompatibleSignalSlotArguments EnsureCompatibleArguments;


        const int *types = 0;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection)
            types = QtPrivate::ConnectionTypes<typename SignalType::Arguments>::types();

        return connectImpl(sender, reinterpret_cast<void **>(&signal),
                           receiver, new QSlotObject<Func2,
                                                     typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                                     typename SignalType::ReturnType>(slot),
                            type, types, &SignalType::Object::staticMetaObject);
    }

    //connect to a function pointer  (not a member)
    template <typename Func1, typename Func2>
    static inline typename QtPrivate::QEnableIf<(int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= int(QtPrivate::FunctionPointer<Func2>::ArgumentCount)), Connection>::Type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;

        //compilation error if the arguments does not match.
        typedef typename QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::IncompatibleSignalSlotArguments EnsureCompatibleArguments;
        typedef typename QtPrivate::QEnableIf<(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount))>::Type EnsureArgumentsCount;

        return connectImpl(sender, reinterpret_cast<void **>(&signal), sender,
                           new QStaticSlotObject<Func2,
                                                 typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                                 typename SignalType::ReturnType>(slot),
                           Qt::DirectConnection, 0, &SignalType::Object::staticMetaObject);
    }

    //connect to a functor
    template <typename Func1, typename Func2>
    static inline typename QtPrivate::QEnableIf<QtPrivate::FunctionPointer<Func2>::ArgumentCount == 1000, Connection>::Type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;


        return connectImpl(sender, reinterpret_cast<void **>(&signal),
                           sender, new QFunctorSlotObject<Func2, SignalType::ArgumentCount, typename SignalType::Arguments, typename SignalType::ReturnType>(slot),
                           Qt::DirectConnection, 0, &SignalType::Object::staticMetaObject);
    }



    static bool disconnect(const QObject *sender, const char *signal,
                           const QObject *receiver, const char *member);
    static bool disconnect(const QObject *sender, const QMetaMethod &signal,
                           const QObject *receiver, const QMetaMethod &member);
    inline bool disconnect(const char *signal = 0,
                           const QObject *receiver = 0, const char *member = 0)
        { return disconnect(this, signal, receiver, member); }
    inline bool disconnect(const QObject *receiver, const char *member = 0)
        { return disconnect(this, 0, receiver, member); }
    static void disconnect(const Connection &);

    void dumpObjectTree();
    void dumpObjectInfo();

#ifndef QT_NO_PROPERTIES
    bool setProperty(const char *name, const QVariant &value);
    QVariant property(const char *name) const;
    QList<QByteArray> dynamicPropertyNames() const;
#endif // QT_NO_PROPERTIES

#ifndef QT_NO_USERDATA
    static uint registerUserData();
    void setUserData(uint id, QObjectUserData* data);
    QObjectUserData* userData(uint id) const;
#endif // QT_NO_USERDATA

Q_SIGNALS:
    void destroyed(QObject * = 0);

public:
    inline QObject *parent() const { return d_ptr->parent; }

    inline bool inherits(const char *classname) const
        { return const_cast<QObject *>(this)->qt_metacast(classname) != 0; }

public Q_SLOTS:
    void deleteLater();

protected:
    QObject *sender() const;
    int senderSignalIndex() const;
    int receivers(const char* signal) const;

    virtual void timerEvent(QTimerEvent *);
    virtual void childEvent(QChildEvent *);
    virtual void customEvent(QEvent *);

    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

protected:
    QObject(QObjectPrivate &dd, QObject *parent = 0);

protected:
    QScopedPointer<QObjectData> d_ptr;

    static const QMetaObject staticQtMetaObject;

    friend struct QMetaObject;
    friend class QMetaCallEvent;
    friend class QApplication;
    friend class QApplicationPrivate;
    friend class QCoreApplication;
    friend class QCoreApplicationPrivate;
    friend class QWidget;
    friend class QThreadData;

private:
    Q_DISABLE_COPY(QObject)
    Q_PRIVATE_SLOT(d_func(), void _q_reregisterTimers(void *))
};

inline QObject::Connection QObject::connect(const QObject *asender, const char *asignal,
                             const char *amember, Qt::ConnectionType atype) const
{ return connect(asender, asignal, this, amember, atype); }

#ifndef QT_NO_USERDATA
class Q_CORE_EXPORT QObjectUserData {
public:
    virtual ~QObjectUserData();
};
#endif

#ifdef qdoc
T qFindChild(const QObject *o, const QString &name = QString());
QList<T> qFindChildren(const QObject *oobj, const QString &name = QString());
QList<T> qFindChildren(const QObject *o, const QRegExp &re);
#endif
#ifdef QT_DEPRECATED
template<typename T>
inline QT_DEPRECATED T qFindChild(const QObject *o, const QString &name = QString())
{ return o->findChild<T>(name); }

template<typename T>
inline QT_DEPRECATED QList<T> qFindChildren(const QObject *o, const QString &name = QString())
{
    return o->findChildren<T>(name);
}

#ifndef QT_NO_REGEXP
template<typename T>
inline QT_DEPRECATED QList<T> qFindChildren(const QObject *o, const QRegExp &re)
{
    return o->findChildren<T>(re);
}
#endif

#endif //QT_DEPRECATED

template <class T>
inline T qobject_cast(QObject *object)
{
#if !defined(QT_NO_QOBJECT_CHECK)
    reinterpret_cast<T>(object)->qt_check_for_QOBJECT_macro(*reinterpret_cast<T>(object));
#endif
    return static_cast<T>(reinterpret_cast<T>(object)->staticMetaObject.cast(object));
}

template <class T>
inline T qobject_cast(const QObject *object)
{
#if !defined(QT_NO_QOBJECT_CHECK)
    reinterpret_cast<T>(object)->qt_check_for_QOBJECT_macro(*reinterpret_cast<T>(const_cast<QObject *>(object)));
#endif
    return static_cast<T>(reinterpret_cast<T>(object)->staticMetaObject.cast(object));
}


template <class T> inline const char * qobject_interface_iid()
{ return 0; }

#ifndef Q_MOC_RUN
#  define Q_DECLARE_INTERFACE(IFace, IId) \
    template <> inline const char *qobject_interface_iid<IFace *>() \
    { return IId; } \
    template <> inline IFace *qobject_cast<IFace *>(QObject *object) \
    { return reinterpret_cast<IFace *>((object ? object->qt_metacast(IId) : 0)); } \
    template <> inline IFace *qobject_cast<IFace *>(const QObject *object) \
    { return reinterpret_cast<IFace *>((object ? const_cast<QObject *>(object)->qt_metacast(IId) : 0)); }
#endif // Q_MOC_RUN

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QObject *);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

#endif // QOBJECT_H
