#include <cppunit/extensions/HelperMacros.h>

#include <dbuscc/mocks.h>

namespace unittests {
namespace mocks {

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
{
}

void test::setUp()
{ }

void test::tearDown()
{ }

void test::basic()
{
	dbuscc::mock::connection conn;
	dbuscc::mock::call_message call;
	dbuscc::mock::return_message ret;
	dbuscc::mock::signal_message sig;
	dbuscc::mock::pending_call pending;
	dbuscc::mock::timeout timeout;
	dbuscc::mock::watch watch;
	dbuscc::mock::message_writer writer;
}

}}

