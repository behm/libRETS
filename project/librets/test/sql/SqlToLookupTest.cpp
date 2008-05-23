/*
 * Copyright (C) 2008 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */

#include <cppunit/extensions/HelperMacros.h>
#include "testUtil.h"
#include "TestSqlMetadata.h"
#include "librets/SqlToDmqlCompiler.h"
#include "librets/LookupQuery.h"
#include "librets/RetsSqlException.h"


using namespace librets;
using namespace std;

#define CLASS SqlToLookupTest

class CLASS : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CLASS);
    CPPUNIT_TEST(testSimpleQuery);
    CPPUNIT_TEST(testInvalidSql);
    CPPUNIT_TEST_SUITE_END();

  public:
    CLASS();

  protected:
    void testSimpleQuery();
    void testInvalidSql();
    
    LookupQueryPtr sqlToLookup(string sql);
    void assertInvalidSql(string sql,
                          const CPPUNIT_NS::SourceLine & sourceLine);
    SqlToDmqlCompilerPtr mCompiler;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CLASS);

CLASS::CLASS()
{
    TestSqlMetadataPtr metadata(new TestSqlMetadata());
    mCompiler.reset(new SqlToDmqlCompiler(metadata));
}

LookupQueryPtr CLASS::sqlToLookup(string sql)
{
    if (mCompiler->sqlToDmql(sql) != SqlToDmqlCompiler::LOOKUP_QUERY)
    {
        CPPUNIT_FAIL("Not a Lookup query");
    }
    
    return mCompiler->GetLookupQuery();
}

#define ASSERT_INVALID_SQL(_SQL_) assertInvalidSql(_SQL_, CPPUNIT_SOURCELINE())

void CLASS::assertInvalidSql(string sql,
                             const CPPUNIT_NS::SourceLine & sourceLine)
{
    try
    {
        istringstream inputStream(sql);
        mCompiler->sqlToDmql(inputStream);
        CPPUNIT_NS::Asserter::fail(
            CPPUNIT_NS::Message("forced failure",
                                "should have thrown exception"),
            sourceLine);
    }
    catch (RetsSqlException &)
    {
        // Expected
    }
}

void CLASS::testSimpleQuery()
{
    LookupQueryPtr query = sqlToLookup("select * from lookup:Property:AR");
    ASSERT_STRING_EQUAL("Property", query->GetResource());
    ASSERT_STRING_EQUAL("AR", query->GetLookup());
}

void CLASS::testInvalidSql()
{
    ASSERT_INVALID_SQL("select * from lookup:Property");
}
    
