#define BOOST_TEST_MODULE dep0_contains_var_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/contains_var.hpp"

#include "ast_tests_fixture.hpp"

namespace dep0::ast {

BOOST_FIXTURE_TEST_SUITE(dep0_contains_var_tests, AstTestsFixture)

BOOST_AUTO_TEST_CASE(var_test)
{
    auto const x = var("x");
    BOOST_TEST(contains_var(x, var_t("x"), occurrence_style::anywhere));
    BOOST_TEST(contains_var(x, var_t("x"), occurrence_style::free));
    BOOST_TEST(not contains_var(x, var_t("y"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(x, var_t("y"), occurrence_style::free));
}

BOOST_AUTO_TEST_CASE(abs_test)
{
    auto const f1 = abs({arg(i32(), "x")}, i32(), body(return_(var("x"))));
    BOOST_TEST(contains_var(f1, var_t("x"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(f1, var_t("x"), occurrence_style::free));
    BOOST_TEST(not contains_var(f1, var_t("y"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(f1, var_t("y"), occurrence_style::free));

    auto const f2 = abs({arg(i32(), "x")}, i32(), body(return_(var("y"))));
    BOOST_TEST(contains_var(f2, var_t("x"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(f2, var_t("x"), occurrence_style::free));
}

BOOST_AUTO_TEST_CASE(pi_test)
{
    auto const t1 = pi({arg(i32(), "x")}, i32());
    BOOST_TEST(contains_var(t1, var_t("x"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(t1, var_t("x"), occurrence_style::free));

    auto const t2 = pi({arg(typename_(), "x")}, var("x"));
    BOOST_TEST(contains_var(t2, var_t("x"), occurrence_style::anywhere));
    BOOST_TEST(not contains_var(t2, var_t("x"), occurrence_style::free));

    auto const t3 = pi({arg(typename_(), "x")}, var("y"));
    BOOST_TEST(contains_var(t3, var_t("y"), occurrence_style::anywhere));
    BOOST_TEST(contains_var(t3, var_t("y"), occurrence_style::free));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
