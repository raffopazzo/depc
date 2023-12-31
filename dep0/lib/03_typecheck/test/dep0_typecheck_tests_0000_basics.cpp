#define BOOST_TEST_MODULE dep0_typecheck_tests_0000_basics
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0000_basics, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000) { BOOST_TEST(pass("0000_basics/pass_000.depc")); }
BOOST_AUTO_TEST_CASE(pass_001) { BOOST_TEST(pass("0000_basics/pass_001.depc")); }
BOOST_AUTO_TEST_CASE(pass_002) { BOOST_TEST(pass("0000_basics/pass_002.depc")); }
BOOST_AUTO_TEST_CASE(pass_003) { BOOST_TEST(pass("0000_basics/pass_003.depc")); }
BOOST_AUTO_TEST_CASE(pass_004) { BOOST_TEST(pass("0000_basics/pass_004.depc")); }
BOOST_AUTO_TEST_CASE(pass_005) { BOOST_TEST(pass("0000_basics/pass_005.depc")); }
BOOST_AUTO_TEST_CASE(pass_006) { BOOST_TEST(pass("0000_basics/pass_006.depc")); }
BOOST_AUTO_TEST_CASE(pass_007) { BOOST_TEST(pass("0000_basics/pass_007.depc")); }
BOOST_AUTO_TEST_CASE(pass_008) { BOOST_TEST(pass("0000_basics/pass_008.depc")); }
BOOST_AUTO_TEST_CASE(pass_009) { BOOST_TEST(pass("0000_basics/pass_009.depc")); }
BOOST_AUTO_TEST_CASE(pass_010) { BOOST_TEST(pass("0000_basics/pass_010.depc")); }
BOOST_AUTO_TEST_CASE(pass_011) { BOOST_TEST(pass("0000_basics/pass_011.depc")); }
BOOST_AUTO_TEST_CASE(pass_012) { BOOST_TEST(pass("0000_basics/pass_012.depc")); }
BOOST_AUTO_TEST_CASE(pass_013) { BOOST_TEST(pass("0000_basics/pass_013.depc")); }

BOOST_AUTO_TEST_CASE(pass_014)
{
    BOOST_TEST_REQUIRE(pass("0000_basics/pass_014.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "unit");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST(f.value.body.stmts.size() == 0ul);
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST(is_expr_of(f.properties.sort.get(), pi_of(std::tuple{}, is_unit)));
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_unit(f.value.ret_type.get()));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], app_of(var("unit"))));
    }
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_000.depc"));
    BOOST_TEST(fail_result->error == "expecting expression of type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_001)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_001.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `unit_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_002)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_002.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between expression of type `bool` and expected type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_003)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_003.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between numeric constant and `bool`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_004)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_004.depc"));
    BOOST_TEST(fail_result->error == "in function `main` missing return statement");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_005)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_005.depc"));
    BOOST_TEST(fail_result->error == "type mismatch between expression of type `bool` and expected type `i32_t`");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_006)
{
    BOOST_TEST_REQUIRE(fail("0000_basics/typecheck_error_006.depc"));
    BOOST_TEST(fail_result->error == "in function `main` missing return statement");
    BOOST_TEST(fail_result->location.has_value());
}
BOOST_AUTO_TEST_CASE(typecheck_error_007) { BOOST_TEST(fail("0000_basics/typecheck_error_007.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_008) { BOOST_TEST(fail("0000_basics/typecheck_error_008.depc")); }
BOOST_AUTO_TEST_CASE(typecheck_error_009) { BOOST_TEST(fail("0000_basics/typecheck_error_009.depc")); }

BOOST_AUTO_TEST_SUITE_END()
