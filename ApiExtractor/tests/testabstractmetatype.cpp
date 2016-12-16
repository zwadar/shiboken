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

#include "testabstractmetatype.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestAbstractMetaType::testConstCharPtrType()
{
    const char* cppCode ="const char* justAtest();";
    const char* xmlCode = "<typesystem package=\"Foo\">\
        <primitive-type name='char'/>\
        <function signature='justAtest()' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);
    QCOMPARE(t.builder()->globalFunctions().size(), 1);
    AbstractMetaFunction* func = t.builder()->globalFunctions().first();
    AbstractMetaType* rtype = func->type();
    // Test properties of const char*
    QVERIFY(rtype);
    QCOMPARE(rtype->package(), QLatin1String("Foo"));
    QCOMPARE(rtype->name(), QLatin1String("char"));
    QVERIFY(rtype->isConstant());
    QVERIFY(!rtype->isArray());
    QVERIFY(!rtype->isContainer());
    QVERIFY(!rtype->isObject());
    QVERIFY(!rtype->isPrimitive()); // const char* differs from char, so it's not considered a primitive type by apiextractor
    QVERIFY(rtype->isNativePointer());
    QVERIFY(!rtype->isQObject());
    QVERIFY(!rtype->isReference());
    QVERIFY(!rtype->isValue());
    QVERIFY(!rtype->isValuePointer());
}

void TestAbstractMetaType::testApiVersionSupported()
{
    const char* cppCode ="class foo {}; class foo2 {};\
                          void justAtest(); void justAtest3();";
    const char* xmlCode = "<typesystem package='Foo'>\
        <value-type name='foo' since='0.1'/>\
        <value-type name='foo2' since='1.0'/>\
        <value-type name='foo3' since='1.1'/>\
        <function signature='justAtest()' since='0.1'/>\
        <function signature='justAtest2()' since='1.1'/>\
        <function signature='justAtest3()'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, false, "1.0");

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 2);


    AbstractMetaFunctionList functions = t.builder()->globalFunctions();
    QCOMPARE(functions.size(), 2);
}


void TestAbstractMetaType::testApiVersionNotSupported()
{
    const char* cppCode ="class object {};";
    const char* xmlCode = "<typesystem package='Foo'>\
        <value-type name='object' since='0.1'/>\
    </typesystem>";
    TestUtil t(cppCode, xmlCode, true, "0.1");

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 1);
}

void TestAbstractMetaType::testCharType()
{
    const char* cppCode ="char justAtest(); class A {};";
    const char* xmlCode = "<typesystem package=\"Foo\">\
    <primitive-type name='char'/>\
    <value-type name='A' />\
    <function signature='justAtest()' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 1);
    QCOMPARE(classes.first()->package(), QLatin1String("Foo"));

    AbstractMetaFunctionList functions = t.builder()->globalFunctions();
    QCOMPARE(functions.size(), 1);
    AbstractMetaFunction* func = functions.first();
    AbstractMetaType* rtype = func->type();
    // Test properties of const char*
    QVERIFY(rtype);
    QCOMPARE(rtype->package(), QLatin1String("Foo"));
    QCOMPARE(rtype->name(), QLatin1String("char"));
    QVERIFY(!rtype->isConstant());
    QVERIFY(!rtype->isArray());
    QVERIFY(!rtype->isContainer());
    QVERIFY(!rtype->isObject());
    QVERIFY(rtype->isPrimitive());
    QVERIFY(!rtype->isNativePointer());
    QVERIFY(!rtype->isQObject());
    QVERIFY(!rtype->isReference());
    QVERIFY(!rtype->isValue());
    QVERIFY(!rtype->isValuePointer());
}

void TestAbstractMetaType::testTypedef()
{
    const char* cppCode ="\
    struct A {\
        void someMethod();\
    };\
    typedef A B;\
    typedef B C;";
    const char* xmlCode = "<typesystem package=\"Foo\">\
    <value-type name='C' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 1);
    AbstractMetaClass* c = classes.findClass(QLatin1String("C"));
    QVERIFY(c);
    QVERIFY(c->isTypeAlias());
}

void TestAbstractMetaType::testTypedefWithTemplates()
{
    const char* cppCode ="\
    template<typename T>\
    class A {};\
    \
    class B {};\
    typedef A<B> C;\
    \
    void func(C c);\
    ";
    const char* xmlCode = "<typesystem package=\"Foo\">\
    <container-type name='A' type='list'/>\
    <value-type name='B' />\
    <function signature='func(A&lt;B&gt;)' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 1);
    AbstractMetaFunctionList functions = t.builder()->globalFunctions();
    QCOMPARE(functions.count(), 1);
    AbstractMetaFunction* function = functions.first();
    AbstractMetaArgumentList args = function->arguments();
    QCOMPARE(args.count(), 1);
    AbstractMetaArgument* arg = args.first();
    AbstractMetaType* metaType = arg->type();
    QCOMPARE(metaType->cppSignature(), QLatin1String("A<B >"));
}


void TestAbstractMetaType::testObjectTypeUsedAsValue()
{
    const char* cppCode ="\
    class A {\
        void method(A);\
    };\
    ";
    const char* xmlCode = "<typesystem package='Foo'>\
    <object-type name='A' />\
    </typesystem>";
    TestUtil t(cppCode, xmlCode);

    AbstractMetaClassList classes = t.builder()->classes();
    QCOMPARE(classes.size(), 1);
    AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    QVERIFY(classA);
    AbstractMetaFunctionList overloads = classA->queryFunctionsByName(QLatin1String("method"));
    QCOMPARE(overloads.count(), 1);
    AbstractMetaFunction* method = overloads.first();
    QVERIFY(method);
    AbstractMetaArgumentList args = method->arguments();
    QCOMPARE(args.count(), 1);
    AbstractMetaArgument* arg = args.first();
    AbstractMetaType* metaType = arg->type();
    QCOMPARE(metaType->cppSignature(), QLatin1String("A"));
    QVERIFY(metaType->isValue());
    QVERIFY(metaType->typeEntry()->isObject());
}

QTEST_APPLESS_MAIN(TestAbstractMetaType)
