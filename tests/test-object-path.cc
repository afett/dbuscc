#include <cppunit/extensions/HelperMacros.h>

#include <dbuscc/object-path.h>

namespace unittests {
namespace object_path {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void operators();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(operators);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::operators()
{
	dbuscc::object_path empty;
	dbuscc::object_path foo("/foo");
	dbuscc::object_path bar("/bar");
	dbuscc::object_path bar2(bar);

	CPPUNIT_ASSERT(foo != bar);
	CPPUNIT_ASSERT(foo != bar2);
	CPPUNIT_ASSERT(foo != empty);
	CPPUNIT_ASSERT(foo == foo);

	CPPUNIT_ASSERT(bar == bar2);

	CPPUNIT_ASSERT(empty < foo);
	CPPUNIT_ASSERT(bar < foo);

	empty = foo;
	CPPUNIT_ASSERT(foo == empty);
}

}}

