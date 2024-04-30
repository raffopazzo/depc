#define BOOST_TEST_MODULE dep0_typecheck_tests_0013_cstr
#include <boost/test/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0013_cstr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0013_cstr/pass_000.depc"));
    BOOST_TEST(pass_result->entries.size() == 8ul);
}

// BOOST_AUTO_TEST_CASE(parse_error_000)

BOOST_AUTO_TEST_SUITE_END()
