#define BOOST_TEST_MODULE dep0_typecheck_tests_0020_builtins
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0020_builtins, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000) { BOOST_TEST_REQUIRE(pass("0020_builtins/pass_000.depc")); }
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST_REQUIRE(pass("0020_builtins/pass_001.depc")); }

BOOST_AUTO_TEST_SUITE_END()
