#define BOOST_TEST_MODULE dep0_typecheck_tests_0000_basics
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0000_basics, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("0000_basics/test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("0000_basics/test_0001.depc")); }
BOOST_AUTO_TEST_CASE(test_0002)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0002.depc"));
    BOOST_TEST(fail_result->error == "expecting expression of type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("0000_basics/test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0004.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `unit_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("0000_basics/test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("0000_basics/test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("0000_basics/test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0008.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between expression of type `bool` and expected type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0009)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0009.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `bool`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0010)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0010.depc"));
    BOOST_TEST(fail_result->error == "in function `main` missing return statement");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("0000_basics/test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0012.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between expression of type `bool` and expected type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0013)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/test_0013.depc"));
    BOOST_TEST(fail_result->error == "in function `main` missing return statement");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("0000_basics/test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("0000_basics/test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(fail("0000_basics/test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(fail("0000_basics/test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("0000_basics/test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("0000_basics/test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("0000_basics/test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("0000_basics/test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("0000_basics/test_0022.depc")); }
BOOST_AUTO_TEST_CASE(test_0023) { BOOST_TEST(fail("0000_basics/test_0023.depc")); }

BOOST_AUTO_TEST_SUITE_END()
