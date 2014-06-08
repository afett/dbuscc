#include <cppunit/extensions/HelperMacros.h>

#include <dbuscc/glue/error.h>

namespace unittests {
namespace error {

class test : public CppUnit::TestCase {
public:
	test();
	void setUp();
	void tearDown();

private:
	void basic();

	CPPUNIT_TEST_SUITE(test);
	CPPUNIT_TEST(basic);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(test);

test::test()
{ }

void test::setUp()
{ }

void test::tearDown()
{ }

void test::basic()
{
	dbuscc::error error;
	CPPUNIT_ASSERT(error.glue().raw());
}

}}

