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

#include "testextrainclude.h"
#include <QtTest/QTest>
#include "testutil.h"

void TestExtraInclude::testClassExtraInclude()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'> \
        <value-type name='A'> \
            <extra-includes>\
                <include file-name='header.h' location='global' />\
            </extra-includes>\
        </value-type>\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    const AbstractMetaClass* classA = classes.findClass(QLatin1String("A"));
    QVERIFY(classA);

    QList<Include> includes = classA->typeEntry()->extraIncludes();
    QCOMPARE(includes.count(), 1);
    QCOMPARE(includes.first().name(), QLatin1String("header.h"));
}

void TestExtraInclude::testGlobalExtraIncludes()
{
    const char* cppCode ="struct A {};";
    const char* xmlCode = "\
    <typesystem package='Foo'>\
        <extra-includes>\
            <include file-name='header1.h' location='global' />\
            <include file-name='header2.h' location='global' />\
        </extra-includes>\
        <value-type name='A' />\
    </typesystem>";

    TestUtil t(cppCode, xmlCode, false);
    AbstractMetaClassList classes = t.builder()->classes();
    QVERIFY(classes.findClass(QLatin1String("A")));

    TypeDatabase* td = TypeDatabase::instance();
    TypeEntry* module = td->findType(QLatin1String("Foo"));
    QVERIFY(module);

    QList<Include> includes = module->extraIncludes();
    QCOMPARE(includes.count(), 2);
    QCOMPARE(includes.first().name(), QLatin1String("header1.h"));
    QCOMPARE(includes.last().name(), QLatin1String("header2.h"));
}

QTEST_APPLESS_MAIN(TestExtraInclude)
