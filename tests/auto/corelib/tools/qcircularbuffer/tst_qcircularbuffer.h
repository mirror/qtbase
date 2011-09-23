#ifndef TST_QCIRCULARBUFFER_H
#define TST_QCIRCULARBUFFER_H

#include <QObject>
#include <QtTest/QtTest>

class MyComplexType
{
public:
    MyComplexType( int xx = 0 )
        : x( xx )
    {}

    int x;
};

class TestCircularBuffer : public QObject
{
    Q_OBJECT
public:
    TestCircularBuffer(QObject *parent = 0);

private slots:
    // Run before any tests
    void initTestCase();

    // Run after all tests have finished
    void cleanupTestCase();

    // Run before every test function
    void init();

    // Run after every test
    void cleanup();

    // The tests
    void construction();
    void append();
    void at();
    void capacity();
    void clear();
    void contains();
    void count();
    void data();
    void dataOneTwo();
    void endsWith();
    void erase();
    void fill();
    void first();
    void fromList();
    void fromVector();
    void indexOf();
    void insert();
    void insertIterator();
    void isEmpty();
    void isFull();
    void isLinearised();
    void last();
    void lastIndexOf();
    void linearise();
    void prepend();

    void removeStaticLinearised();
    void removeStaticNonLinearised();
    void removeMovableLinearised();
    void removeMovableNonLinearisedUpper();
    void removeMovableNonLinearisedLower();

    void replace();
    void resize();

    void setCapacityStatic();
    void setCapacityMovable();

    void size();
    void sizeAvailable();
    void startsWith();
    void toList();
    void toVector();
    void value();

    void operatorEquality();
    void operatorInequality();
    void operatorPlus();
    void operatorPlusEquals();
    void operatorStream();

    void const_iterator();
    void iterator();
};

#endif // TST_QCIRCULARBUFFER_H
