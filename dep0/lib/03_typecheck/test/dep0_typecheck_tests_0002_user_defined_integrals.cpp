#define BOOST_TEST_MODULE dep0_typecheck_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0002_user_defined_integrals, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/test_0152.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
}
BOOST_AUTO_TEST_CASE(test_0153) { BOOST_TEST(fail("0002_user_defined_integrals/test_0153.depc")); }
// BOOST_AUTO_TEST_CASE(test_0154) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0155) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0156) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0157) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0158) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0159) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0160) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0161) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0162) doesn't parse
BOOST_AUTO_TEST_CASE(test_0163) { BOOST_TEST(pass("0002_user_defined_integrals/test_0163.depc")); }
BOOST_AUTO_TEST_CASE(test_0164) { BOOST_TEST(pass("0002_user_defined_integrals/test_0164.depc")); }
// BOOST_AUTO_TEST_CASE(test_0165) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0166) doesn't parse

BOOST_AUTO_TEST_SUITE_END()
