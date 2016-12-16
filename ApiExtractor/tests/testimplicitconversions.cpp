/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of PySide2.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "testimplicitconversions.h"
#include "testutil.h"
#include <QtTest/QTest>

void TestImplicitConversions::testWithPrivateCtors()
{
    const char* cppCode ="\
    class B;\
    class C;\
    class A {\
        A(const B&);\
    public:\
        A(const C&);\
    };\
    class B {};\
    class C {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <value-type name='A'/> \
        <value-type name='B'/> \
        <value-type name='C'/> \
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);

    AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    AbstractMetaClass* classC = classes.findClass(QLatin1String("C"));
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    QCOMPARE(implicitConvs.first()->arguments().first()->type()->typeEntry(), classC->typeEntry());
}

void TestImplicitConversions::testWithModifiedVisibility()
{
    const char* cppCode ="\
    class B;\
    class A {\
    public:\
        A(const B&);\
    };\
    class B {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <value-type name='A'>\
            <modify-function signature='A(const B&amp;)'>\
                <access modifier='private' />\
            </modify-function>\
        </value-type>\
        <value-type name='B'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    AbstractMetaClass* classB = classes.findClass(QLatin1String("B"));
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    QCOMPARE(implicitConvs.first()->arguments().first()->type()->typeEntry(), classB->typeEntry());
}


void TestImplicitConversions::testWithAddedCtor()
{
    const char* cppCode ="\
    class B;\
    class A {\
    public:\
        A(const B&);\
    };\
    class B {};\
    class C {};\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <custom-type name='TARGETLANGTYPE' />\
        <value-type name='A'>\
            <add-function signature='A(const C&amp;)' />\
        </value-type>\
        <value-type name='B'>\
            <add-function signature='B(TARGETLANGTYPE*)' />\
        </value-type>\
        <value-type name='C'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 3);

    AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 2);

    // Added constructors with custom types should never result in implicit converters.
    AbstractMetaClass* classB = classes.findClass(QLatin1String("B"));
    implicitConvs = classB->implicitConversions();
    QCOMPARE(implicitConvs.count(), 0);
}

void TestImplicitConversions::testWithExternalConversionOperator()
{
    const char* cppCode ="\
    class A {};\
    struct B {\
        operator A() const;\
    };\
    ";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <value-type name='A'/>\
        <value-type name='B'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.count(), 2);
    AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    AbstractMetaClass* classB = classes.findClass(QLatin1String("B"));
    AbstractMetaFunctionList implicitConvs = classA->implicitConversions();
    QCOMPARE(implicitConvs.count(), 1);
    AbstractMetaFunctionList externalConvOps = classA->externalConversionOperators();
    QCOMPARE(externalConvOps.count(), 1);

    const AbstractMetaFunction* convOp = 0;
    foreach(const AbstractMetaFunction* func, classB->functions()) {
        if (func->isConversionOperator())
            convOp = func;
    }
    QVERIFY(convOp);
    QCOMPARE(implicitConvs.first(), convOp);
}

QTEST_APPLESS_MAIN(TestImplicitConversions)
