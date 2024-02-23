#define BOOST_TEST_MODULE dep0_typecheck_tests_0002_user_defined_integrals
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0002_user_defined_integrals, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0002_user_defined_integrals/pass_000.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
}
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0002_user_defined_integrals/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0002_user_defined_integrals/pass_002.depc")); }

// BOOST_AUTO_TEST_CASE(parse_error_000)
// BOOST_AUTO_TEST_CASE(parse_error_001)
// BOOST_AUTO_TEST_CASE(parse_error_002)
// BOOST_AUTO_TEST_CASE(parse_error_003)
// BOOST_AUTO_TEST_CASE(parse_error_004)
// BOOST_AUTO_TEST_CASE(parse_error_005)
// BOOST_AUTO_TEST_CASE(parse_error_006)
// BOOST_AUTO_TEST_CASE(parse_error_007)
// BOOST_AUTO_TEST_CASE(parse_error_008)
// BOOST_AUTO_TEST_CASE(parse_error_009)
// BOOST_AUTO_TEST_CASE(parse_error_010)

BOOST_AUTO_TEST_CASE(typecheck_error_000) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_000.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_001) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_001.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_002) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_002.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_003) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_003.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_004) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_004.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_005) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_005.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_006) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_006.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(fail("0002_user_defined_integrals/typecheck_error_008.depc")); }

BOOST_AUTO_TEST_SUITE_END()
