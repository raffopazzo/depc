#define BOOST_TEST_MODULE dep0_typecheck_tests_0004_plus_expr
#include <boost/test/included/unit_test.hpp>

#include "typecheck_tests_fixture.hpp"

using namespace dep0::testing;

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests_0004_plus_expr, TypecheckTestsFixture)

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0173.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0174.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0175.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("+2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("0004_plus_expr/test_0176.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "minus_one");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::typecheck::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("-2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0177)
{
    BOOST_TEST_REQUIRE(fail("0004_plus_expr/test_0177.depc"));
    BOOST_TEST(fail_result->error == "invalid sign for numeric constant");
    BOOST_TEST(fail_result->location.has_value());
    BOOST_TEST_REQUIRE(fail_result->tgt.has_value());
    std::ostringstream tgt;
    pretty_print(tgt, fail_result->tgt.value());
    BOOST_TEST(tgt.str() == "u32_t");
}

BOOST_AUTO_TEST_SUITE_END()
