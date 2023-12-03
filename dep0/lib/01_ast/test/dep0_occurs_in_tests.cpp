#define BOOST_TEST_MODULE dep0_occurs_in_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/occurs_in.hpp"

#include "ast_tests_fixture.hpp"

namespace dep0::ast {

BOOST_FIXTURE_TEST_SUITE(dep0_occurs_in_tests, AstTestsFixture)

BOOST_AUTO_TEST_CASE(var_test)
{
    auto const x = var("x");
    BOOST_TEST(occurs_in(var_t("x"), x, occurrence_style::anywhere));
    BOOST_TEST(occurs_in(var_t("x"), x, occurrence_style::free));
    BOOST_TEST(not occurs_in(var_t("y"), x, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("y"), x, occurrence_style::free));
}

BOOST_AUTO_TEST_CASE(abs_test)
{
    auto const f1 = abs({arg(i32(), "x")}, i32(), body(return_(var("x"))));
    BOOST_TEST(occurs_in(var_t("x"), f1, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("x"), f1, occurrence_style::free));
    BOOST_TEST(not occurs_in(var_t("y"), f1, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("y"), f1, occurrence_style::free));

    auto const f2 = abs({arg(i32(), "x")}, i32(), body(return_(var("y"))));
    BOOST_TEST(occurs_in(var_t("x"), f2, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("x"), f2, occurrence_style::free));
    BOOST_TEST(occurs_in(var_t("y"), f2, occurrence_style::anywhere));
    BOOST_TEST(occurs_in(var_t("y"), f2, occurrence_style::free));
}

BOOST_AUTO_TEST_CASE(pi_test)
{
    auto const t1 = pi({arg(i32(), "x")}, i32());
    BOOST_TEST(occurs_in(var_t("x"), t1, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("x"), t1, occurrence_style::free));

    auto const t2 = pi({arg(typename_(), "x")}, var("x"));
    BOOST_TEST(occurs_in(var_t("x"), t2, occurrence_style::anywhere));
    BOOST_TEST(not occurs_in(var_t("x"), t2, occurrence_style::free));

    auto const t3 = pi({arg(typename_(), "x")}, var("y"));
    BOOST_TEST(occurs_in(var_t("y"), t3, occurrence_style::anywhere));
    BOOST_TEST(occurs_in(var_t("y"), t3, occurrence_style::free));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
