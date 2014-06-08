#include <cppunit/extensions/HelperMacros.h>

#include <dbuscc/glue/pending-call.h>

namespace unittests {
namespace pending_call {

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
}

}}

