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

#ifndef QBYTEARRAY_H
#define QBYTEARRAY_H

#include <QtCore/qrefcount.h>
#include <QtCore/qnamespace.h>

#include <string.h>
#include <stdarg.h>

#ifdef truncate
#error qbytearray.h must be included before any header file that defines truncate
#endif

#if defined(Q_CC_GNU) && (__GNUC__ == 4 && __GNUC_MINOR__ == 0)
//There is a bug in GCC 4.0 that tries to instantiate template of annonymous enum
#  ifdef QT_USE_FAST_OPERATOR_PLUS
#    undef QT_USE_FAST_OPERATOR_PLUS
#  endif
#  ifdef QT_USE_QSTRINGBUILDER
#    undef QT_USE_QSTRINGBUILDER
#  endif

#endif


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

Q_CORE_EXPORT char *qstrdup(const char *);

inline uint qstrlen(const char *str)
{ return str ? uint(strlen(str)) : 0; }

inline uint qstrnlen(const char *str, uint maxlen)
{
    uint length = 0;
    if (str) {
        while (length < maxlen && *str++)
            length++;
    }
    return length;
}

Q_CORE_EXPORT char *qstrcpy(char *dst, const char *src);
Q_CORE_EXPORT char *qstrncpy(char *dst, const char *src, uint len);

Q_CORE_EXPORT int qstrcmp(const char *str1, const char *str2);
Q_CORE_EXPORT int qstrcmp(const QByteArray &str1, const QByteArray &str2);
Q_CORE_EXPORT int qstrcmp(const QByteArray &str1, const char *str2);
static inline int qstrcmp(const char *str1, const QByteArray &str2)
{ return -qstrcmp(str2, str1); }

inline int qstrncmp(const char *str1, const char *str2, uint len)
{
    return (str1 && str2) ? strncmp(str1, str2, len)
        : (str1 ? 1 : (str2 ? -1 : 0));
}
Q_CORE_EXPORT int qstricmp(const char *, const char *);
Q_CORE_EXPORT int qstrnicmp(const char *, const char *, uint len);

// implemented in qvsnprintf.cpp
Q_CORE_EXPORT int qvsnprintf(char *str, size_t n, const char *fmt, va_list ap);
Q_CORE_EXPORT int qsnprintf(char *str, size_t n, const char *fmt, ...);

// qChecksum: Internet checksum

Q_CORE_EXPORT quint16 qChecksum(const char *s, uint len);

class QByteRef;
class QString;
class QDataStream;
template <typename T> class QList;

struct QByteArrayData
{
    QtPrivate::RefCount ref;
    int size;
    uint alloc : 31;
    uint capacityReserved : 1;
    union {
        qptrdiff offset; // will always work as we add/subtract from a ushort ptr
        char d[sizeof(qptrdiff)];
    };
    inline char *data() { return d + sizeof(qptrdiff) + offset; }
    inline const char *data() const { return d + sizeof(qptrdiff) + offset; }
};

template<int N> struct QConstByteArrayData
{
    const QByteArrayData ba;
    const char data[N + 1];
};

template<int N> struct QConstByteArrayDataPtr
{
    const QConstByteArrayData<N> *ptr;
};


#if defined(Q_COMPILER_LAMBDA)
#  define QByteArrayLiteral(str) ([]() { \
        enum { Size = sizeof(str) - 1 }; \
        static const QConstByteArrayData<Size> qbytearray_literal = \
        { { Q_REFCOUNT_INITIALIZER(-1), Size, 0, 0, { 0 } }, str }; \
        QConstByteArrayDataPtr<Size> holder = { &qbytearray_literal }; \
    return holder; }())

#elif defined(Q_CC_GNU)
// We need to create a QByteArrayData in the .rodata section of memory
// and the only way to do that is to create a "static const" variable.
// To do that, we need the __extension__ {( )} trick which only GCC supports

#  define QByteArrayLiteral(str) \
    __extension__ ({ \
        enum { Size = sizeof(str) - 1 }; \
        static const QConstByteArrayData<Size> qbytearray_literal = \
        { { Q_REFCOUNT_INITIALIZER(-1), Size, 0, 0, { 0 } }, str }; \
        QConstByteArrayDataPtr<Size> holder = { &qbytearray_literal }; \
        holder; })
#endif

#ifndef QByteArrayLiteral
// no lambdas, not GCC, use const char * instead

# define QByteArrayLiteral(str) (str)
#endif


class Q_CORE_EXPORT QByteArray
{
private:
    typedef QByteArrayData Data;

public:
    inline QByteArray();
    QByteArray(const char *);
    QByteArray(const char *, int size);
    QByteArray(int size, char c);
    QByteArray(int size, Qt::Initialization);
    inline QByteArray(const QByteArray &);
    inline ~QByteArray();

    QByteArray &operator=(const QByteArray &);
    QByteArray &operator=(const char *str);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QByteArray &operator=(QByteArray &&other)
    { qSwap(d, other.d); return *this; }
#endif

    inline void swap(QByteArray &other) { qSwap(d, other.d); }

    inline int size() const;
    bool isEmpty() const;
    void resize(int size);

    QByteArray &fill(char c, int size = -1);

    int capacity() const;
    void reserve(int size);
    void squeeze();

#ifndef QT_NO_CAST_FROM_BYTEARRAY
    operator const char *() const;
    operator const void *() const;
#endif
    char *data();
    const char *data() const;
    inline const char *constData() const;
    inline void detach();
    bool isDetached() const;
    inline bool isSharedWith(const QByteArray &other) const { return d == other.d; }
    void clear();

    char at(int i) const;
    char operator[](int i) const;
    char operator[](uint i) const;
    QByteRef operator[](int i);
    QByteRef operator[](uint i);

    int indexOf(char c, int from = 0) const;
    int indexOf(const char *c, int from = 0) const;
    int indexOf(const QByteArray &a, int from = 0) const;
    int lastIndexOf(char c, int from = -1) const;
    int lastIndexOf(const char *c, int from = -1) const;
    int lastIndexOf(const QByteArray &a, int from = -1) const;

    QBool contains(char c) const;
    QBool contains(const char *a) const;
    QBool contains(const QByteArray &a) const;
    int count(char c) const;
    int count(const char *a) const;
    int count(const QByteArray &a) const;

    QByteArray left(int len) const;
    QByteArray right(int len) const;
    QByteArray mid(int index, int len = -1) const;

    bool startsWith(const QByteArray &a) const;
    bool startsWith(char c) const;
    bool startsWith(const char *c) const;

    bool endsWith(const QByteArray &a) const;
    bool endsWith(char c) const;
    bool endsWith(const char *c) const;

    void truncate(int pos);
    void chop(int n);

    QByteArray toLower() const;
    QByteArray toUpper() const;

    QByteArray trimmed() const;
    QByteArray simplified() const;
    QByteArray leftJustified(int width, char fill = ' ', bool truncate = false) const;
    QByteArray rightJustified(int width, char fill = ' ', bool truncate = false) const;

    QByteArray &prepend(char c);
    QByteArray &prepend(const char *s);
    QByteArray &prepend(const char *s, int len);
    QByteArray &prepend(const QByteArray &a);
    QByteArray &append(char c);
    QByteArray &append(const char *s);
    QByteArray &append(const char *s, int len);
    QByteArray &append(const QByteArray &a);
    QByteArray &insert(int i, char c);
    QByteArray &insert(int i, const char *s);
    QByteArray &insert(int i, const char *s, int len);
    QByteArray &insert(int i, const QByteArray &a);
    QByteArray &remove(int index, int len);
    QByteArray &replace(int index, int len, const char *s);
    QByteArray &replace(int index, int len, const char *s, int alen);
    QByteArray &replace(int index, int len, const QByteArray &s);
    QByteArray &replace(char before, const char *after);
    QByteArray &replace(char before, const QByteArray &after);
    QByteArray &replace(const char *before, const char *after);
    QByteArray &replace(const char *before, int bsize, const char *after, int asize);
    QByteArray &replace(const QByteArray &before, const QByteArray &after);
    QByteArray &replace(const QByteArray &before, const char *after);
    QByteArray &replace(const char *before, const QByteArray &after);
    QByteArray &replace(char before, char after);
    QByteArray &operator+=(char c);
    QByteArray &operator+=(const char *s);
    QByteArray &operator+=(const QByteArray &a);

    QList<QByteArray> split(char sep) const;

    QByteArray repeated(int times) const;

#ifndef QT_NO_CAST_TO_ASCII
    QT_ASCII_CAST_WARN QByteArray &append(const QString &s);
    QT_ASCII_CAST_WARN QByteArray &insert(int i, const QString &s);
    QT_ASCII_CAST_WARN QByteArray &replace(const QString &before, const char *after);
    QT_ASCII_CAST_WARN QByteArray &replace(char c, const QString &after);
    QT_ASCII_CAST_WARN QByteArray &replace(const QString &before, const QByteArray &after);

    QT_ASCII_CAST_WARN QByteArray &operator+=(const QString &s);
    QT_ASCII_CAST_WARN int indexOf(const QString &s, int from = 0) const;
    QT_ASCII_CAST_WARN int lastIndexOf(const QString &s, int from = -1) const;
#endif
#ifndef QT_NO_CAST_FROM_ASCII
    inline QT_ASCII_CAST_WARN bool operator==(const QString &s2) const;
    inline QT_ASCII_CAST_WARN bool operator!=(const QString &s2) const;
    inline QT_ASCII_CAST_WARN bool operator<(const QString &s2) const;
    inline QT_ASCII_CAST_WARN bool operator>(const QString &s2) const;
    inline QT_ASCII_CAST_WARN bool operator<=(const QString &s2) const;
    inline QT_ASCII_CAST_WARN bool operator>=(const QString &s2) const;
#endif

    short toShort(bool *ok = 0, int base = 10) const;
    ushort toUShort(bool *ok = 0, int base = 10) const;
    int toInt(bool *ok = 0, int base = 10) const;
    uint toUInt(bool *ok = 0, int base = 10) const;
    long toLong(bool *ok = 0, int base = 10) const;
    ulong toULong(bool *ok = 0, int base = 10) const;
    qlonglong toLongLong(bool *ok = 0, int base = 10) const;
    qulonglong toULongLong(bool *ok = 0, int base = 10) const;
    float toFloat(bool *ok = 0) const;
    double toDouble(bool *ok = 0) const;
    QByteArray toBase64() const;
    QByteArray toHex() const;
    QByteArray toPercentEncoding(const QByteArray &exclude = QByteArray(),
                                 const QByteArray &include = QByteArray(),
                                 char percent = '%') const;

    QByteArray &setNum(short, int base = 10);
    QByteArray &setNum(ushort, int base = 10);
    QByteArray &setNum(int, int base = 10);
    QByteArray &setNum(uint, int base = 10);
    QByteArray &setNum(qlonglong, int base = 10);
    QByteArray &setNum(qulonglong, int base = 10);
    QByteArray &setNum(float, char f = 'g', int prec = 6);
    QByteArray &setNum(double, char f = 'g', int prec = 6);
    QByteArray &setRawData(const char *a, uint n); // ### Qt 5: use an int

    static QByteArray number(int, int base = 10);
    static QByteArray number(uint, int base = 10);
    static QByteArray number(qlonglong, int base = 10);
    static QByteArray number(qulonglong, int base = 10);
    static QByteArray number(double, char f = 'g', int prec = 6);
    static QByteArray fromRawData(const char *, int size);
    static QByteArray fromBase64(const QByteArray &base64);
    static QByteArray fromHex(const QByteArray &hexEncoded);
    static QByteArray fromPercentEncoding(const QByteArray &pctEncoded, char percent = '%');


    typedef char *iterator;
    typedef const char *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    iterator begin();
    const_iterator begin() const;
    const_iterator constBegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator constEnd() const;

    // stl compatibility
    typedef const char & const_reference;
    typedef char & reference;
    typedef char value_type;
    void push_back(char c);
    void push_back(const char *c);
    void push_back(const QByteArray &a);
    void push_front(char c);
    void push_front(const char *c);
    void push_front(const QByteArray &a);

    inline int count() const { return d->size; }
    int length() const { return d->size; }
    bool isNull() const;

    template <int n>
    inline QByteArray(const QConstByteArrayData<n> &dd)
        : d(const_cast<QByteArrayData *>(&dd.str)) {}
    template <int N>
    Q_DECL_CONSTEXPR inline QByteArray(QConstByteArrayDataPtr<N> dd)
        : d(const_cast<QByteArrayData *>(&dd.ptr->ba)) {}

private:
    operator QNoImplicitBoolCast() const;
    static QConstByteArrayData<1> shared_null;
    static QConstByteArrayData<1> shared_empty;
    Data *d;
    QByteArray(Data *dd, int /*dummy*/, int /*dummy*/) : d(dd) {}
    void realloc(int alloc);
    void expand(int i);
    QByteArray nulTerminated() const;

    friend class QByteRef;
    friend class QString;
    friend Q_CORE_EXPORT QByteArray qUncompress(const uchar *data, int nbytes);
public:
    typedef Data * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

inline QByteArray::QByteArray(): d(const_cast<Data *>(&shared_null.ba)) { d->ref.ref(); }
inline QByteArray::~QByteArray() { if (!d->ref.deref()) qFree(d); }
inline int QByteArray::size() const
{ return d->size; }

inline char QByteArray::at(int i) const
{ Q_ASSERT(i >= 0 && i < size()); return d->data()[i]; }
inline char QByteArray::operator[](int i) const
{ Q_ASSERT(i >= 0 && i < size()); return d->data()[i]; }
inline char QByteArray::operator[](uint i) const
{ Q_ASSERT(i < uint(size())); return d->data()[i]; }

inline bool QByteArray::isEmpty() const
{ return d->size == 0; }
#ifndef QT_NO_CAST_FROM_BYTEARRAY
inline QByteArray::operator const char *() const
{ return d->data(); }
inline QByteArray::operator const void *() const
{ return d->data(); }
#endif
inline char *QByteArray::data()
{ detach(); return d->data(); }
inline const char *QByteArray::data() const
{ return d->data(); }
inline const char *QByteArray::constData() const
{ return d->data(); }
inline void QByteArray::detach()
{ if (d->ref != 1 || d->offset) realloc(d->size); }
inline bool QByteArray::isDetached() const
{ return d->ref == 1; }
inline QByteArray::QByteArray(const QByteArray &a) : d(a.d)
{ d->ref.ref(); }

inline int QByteArray::capacity() const
{ return d->alloc; }

inline void QByteArray::reserve(int asize)
{ if (d->ref != 1 || asize > d->alloc) realloc(asize); d->capacityReserved = true; }

inline void QByteArray::squeeze()
{ if (d->size < d->alloc) realloc(d->size); d->capacityReserved = false; }

class Q_CORE_EXPORT QByteRef {
    QByteArray &a;
    int i;
    inline QByteRef(QByteArray &array, int idx)
        : a(array),i(idx) {}
    friend class QByteArray;
public:
    inline operator char() const
        { return i < a.d->size ? a.d->data()[i] : char(0); }
    inline QByteRef &operator=(char c)
        { if (i >= a.d->size) a.expand(i); else a.detach();
          a.d->data()[i] = c;  return *this; }
    inline QByteRef &operator=(const QByteRef &c)
        { if (i >= a.d->size) a.expand(i); else a.detach();
          a.d->data()[i] = c.a.d->data()[c.i];  return *this; }
    inline bool operator==(char c) const
    { return a.d->data()[i] == c; }
    inline bool operator!=(char c) const
    { return a.d->data()[i] != c; }
    inline bool operator>(char c) const
    { return a.d->data()[i] > c; }
    inline bool operator>=(char c) const
    { return a.d->data()[i] >= c; }
    inline bool operator<(char c) const
    { return a.d->data()[i] < c; }
    inline bool operator<=(char c) const
    { return a.d->data()[i] <= c; }
};

inline QByteRef QByteArray::operator[](int i)
{ Q_ASSERT(i >= 0); return QByteRef(*this, i); }
inline QByteRef QByteArray::operator[](uint i)
{ return QByteRef(*this, i); }
inline QByteArray::iterator QByteArray::begin()
{ detach(); return d->data(); }
inline QByteArray::const_iterator QByteArray::begin() const
{ return d->data(); }
inline QByteArray::const_iterator QByteArray::constBegin() const
{ return d->data(); }
inline QByteArray::iterator QByteArray::end()
{ detach(); return d->data() + d->size; }
inline QByteArray::const_iterator QByteArray::end() const
{ return d->data() + d->size; }
inline QByteArray::const_iterator QByteArray::constEnd() const
{ return d->data() + d->size; }
inline QByteArray &QByteArray::operator+=(char c)
{ return append(c); }
inline QByteArray &QByteArray::operator+=(const char *s)
{ return append(s); }
inline QByteArray &QByteArray::operator+=(const QByteArray &a)
{ return append(a); }
inline void QByteArray::push_back(char c)
{ append(c); }
inline void QByteArray::push_back(const char *c)
{ append(c); }
inline void QByteArray::push_back(const QByteArray &a)
{ append(a); }
inline void QByteArray::push_front(char c)
{ prepend(c); }
inline void QByteArray::push_front(const char *c)
{ prepend(c); }
inline void QByteArray::push_front(const QByteArray &a)
{ prepend(a); }
inline QBool QByteArray::contains(const QByteArray &a) const
{ return QBool(indexOf(a) != -1); }
inline QBool QByteArray::contains(char c) const
{ return QBool(indexOf(c) != -1); }
inline bool operator==(const QByteArray &a1, const QByteArray &a2)
{ return (a1.size() == a2.size()) && (memcmp(a1.constData(), a2.constData(), a1.size())==0); }
inline bool operator==(const QByteArray &a1, const char *a2)
{ return a2 ? qstrcmp(a1,a2) == 0 : a1.isEmpty(); }
inline bool operator==(const char *a1, const QByteArray &a2)
{ return a1 ? qstrcmp(a1,a2) == 0 : a2.isEmpty(); }
inline bool operator!=(const QByteArray &a1, const QByteArray &a2)
{ return !(a1==a2); }
inline bool operator!=(const QByteArray &a1, const char *a2)
{ return a2 ? qstrcmp(a1,a2) != 0 : !a1.isEmpty(); }
inline bool operator!=(const char *a1, const QByteArray &a2)
{ return a1 ? qstrcmp(a1,a2) != 0 : !a2.isEmpty(); }
inline bool operator<(const QByteArray &a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) < 0; }
 inline bool operator<(const QByteArray &a1, const char *a2)
{ return qstrcmp(a1, a2) < 0; }
inline bool operator<(const char *a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) < 0; }
inline bool operator<=(const QByteArray &a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) <= 0; }
inline bool operator<=(const QByteArray &a1, const char *a2)
{ return qstrcmp(a1, a2) <= 0; }
inline bool operator<=(const char *a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) <= 0; }
inline bool operator>(const QByteArray &a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) > 0; }
inline bool operator>(const QByteArray &a1, const char *a2)
{ return qstrcmp(a1, a2) > 0; }
inline bool operator>(const char *a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) > 0; }
inline bool operator>=(const QByteArray &a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) >= 0; }
inline bool operator>=(const QByteArray &a1, const char *a2)
{ return qstrcmp(a1, a2) >= 0; }
inline bool operator>=(const char *a1, const QByteArray &a2)
{ return qstrcmp(a1, a2) >= 0; }
#if !defined(QT_USE_QSTRINGBUILDER)
inline const QByteArray operator+(const QByteArray &a1, const QByteArray &a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const QByteArray &a1, const char *a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const QByteArray &a1, char a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(const char *a1, const QByteArray &a2)
{ return QByteArray(a1) += a2; }
inline const QByteArray operator+(char a1, const QByteArray &a2)
{ return QByteArray(&a1, 1) += a2; }
#endif // QT_USE_QSTRINGBUILDER
inline QBool QByteArray::contains(const char *c) const
{ return QBool(indexOf(c) != -1); }
inline QByteArray &QByteArray::replace(char before, const char *c)
{ return replace(&before, 1, c, qstrlen(c)); }
inline QByteArray &QByteArray::replace(const QByteArray &before, const char *c)
{ return replace(before.constData(), before.size(), c, qstrlen(c)); }
inline QByteArray &QByteArray::replace(const char *before, const char *after)
{ return replace(before, qstrlen(before), after, qstrlen(after)); }

inline QByteArray &QByteArray::setNum(short n, int base)
{ return setNum(qlonglong(n), base); }
inline QByteArray &QByteArray::setNum(ushort n, int base)
{ return setNum(qulonglong(n), base); }
inline QByteArray &QByteArray::setNum(int n, int base)
{ return setNum(qlonglong(n), base); }
inline QByteArray &QByteArray::setNum(uint n, int base)
{ return setNum(qulonglong(n), base); }
inline QByteArray &QByteArray::setNum(float n, char f, int prec)
{ return setNum(double(n),f,prec); }


#if !defined(QT_NO_DATASTREAM) || (defined(QT_BOOTSTRAPPED) && !defined(QT_BUILD_QMAKE))
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QByteArray &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QByteArray &);
#endif

#ifndef QT_NO_COMPRESS
Q_CORE_EXPORT QByteArray qCompress(const uchar* data, int nbytes, int compressionLevel = -1);
Q_CORE_EXPORT QByteArray qUncompress(const uchar* data, int nbytes);
inline QByteArray qCompress(const QByteArray& data, int compressionLevel = -1)
{ return qCompress(reinterpret_cast<const uchar *>(data.constData()), data.size(), compressionLevel); }
inline QByteArray qUncompress(const QByteArray& data)
{ return qUncompress(reinterpret_cast<const uchar*>(data.constData()), data.size()); }
#endif

Q_DECLARE_TYPEINFO(QByteArray, Q_MOVABLE_TYPE);
Q_DECLARE_SHARED(QByteArray)

QT_END_NAMESPACE

QT_END_HEADER

#ifdef QT_USE_QSTRINGBUILDER
#include <QtCore/qstring.h>
#endif

#endif // QBYTEARRAY_H
