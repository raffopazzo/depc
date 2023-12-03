#define BOOST_TEST_MODULE dep0_typecheck_tests_0004_plus_expr
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0004_plus_expr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(pass_000)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_000.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_001)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_001.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(constant(1), constant(2))));
}

BOOST_AUTO_TEST_CASE(pass_002)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_002.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(constant(1), numeric_constant("+2"))));
}

BOOST_AUTO_TEST_CASE(pass_003)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/pass_003.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "minus_one");
    BOOST_TEST(is_i32(f.value.ret_type.get()));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], plus(constant(1), constant(-2))));
}

BOOST_AUTO_TEST_CASE(typecheck_error_000)
{
    BOOST_TEST_REQUIRE(fail("0004_plus_expr/typecheck_error_000.depc"));
    BOOST_TEST(fail_result->error == "invalid sign for numeric constant");
    BOOST_TEST(fail_result->location.has_value());
    BOOST_TEST_REQUIRE(fail_result->context.has_value());
    std::ostringstream tgt;
    pretty_print(tgt, fail_result->context->second);
    BOOST_TEST(tgt.str() == "u32_t");
}

BOOST_AUTO_TEST_SUITE_END()
