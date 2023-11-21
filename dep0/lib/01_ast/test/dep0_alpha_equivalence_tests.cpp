#define BOOST_TEST_MODULE dep0_alpha_equivalence_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "ast_tests_fixture.hpp"

namespace dep0::ast {

template <Properties P>
boost::test_tools::predicate_result yay(expr_t<P> const& x, expr_t<P> const& y)
{
    auto const& result = is_alpha_equivalent(x, y);
    if (result)
        return true;
    else
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print(failed.message().stream(), result.error());
        return failed;
    }
}

template <Properties P>
boost::test_tools::predicate_result nay(expr_t<P> const& x, expr_t<P> const& y)
{
    if (not is_alpha_equivalent(x, y))
        return true;
    else
    {
        auto failed = boost::test_tools::predicate_result(false);
        pretty_print(failed.message().stream() << '`', x) << "` should not be alpha-equivalent to ";
        pretty_print(failed.message().stream() << '`', y) << '`';
        return failed;
    }
}

BOOST_FIXTURE_TEST_SUITE(dep0_alpha_equivalence_tests, AstTestsFixture)

BOOST_AUTO_TEST_CASE(var_test)
{
    BOOST_TEST(yay(var("x"), var("x")));
    BOOST_TEST(nay(var("x"), var("y")));
}

BOOST_AUTO_TEST_CASE(pi_test)
{
    BOOST_TEST(yay(
        pi({arg(i32(), "x")}, i32()),
        pi({arg(i32(), "y")}, i32())));
    BOOST_TEST(nay(
        pi({arg(i32(), "x")}, i32()),
        pi({arg(i32(), "y")}, u32())));
    BOOST_TEST(yay(
        pi({arg(i32(), "x")}, i32()),
        pi({arg(i32())}, i32())));
    BOOST_TEST(yay(
        pi({arg(i32())}, i32()),
        pi({arg(i32(), "y")}, i32())));
    BOOST_TEST(yay(
        pi({arg(i32())}, i32()),
        pi({arg(i32())}, i32())));
    BOOST_TEST(yay(
        pi({arg(i32())}, i32()),
        pi({arg(i32())}, i32())));

    BOOST_TEST(yay(
        pi({arg(typename_(), "x")}, var("x")),
        pi({arg(typename_(), "y")}, var("y"))));
    BOOST_TEST(nay(
        pi({arg(typename_(), "x")}, var("x")),
        pi({arg(typename_(), "y")}, var("x"))));

    BOOST_TEST(nay(
        pi({arg(typename_(), "x")}, var("x")),
        pi({arg(typename_())}, var("x"))));

    BOOST_TEST(yay(
        pi({arg(typename_(), "x"), arg(var("x"))}, var("x")),
        pi({arg(typename_(), "y"), arg(var("y"))}, var("y"))));
    BOOST_TEST(yay(
        pi({arg(typename_(), "x"), arg(i32(), "x")}, i32()),
        pi({arg(typename_()), arg(i32())}, i32())));
}

BOOST_AUTO_TEST_CASE(abs_test)
{
    BOOST_TEST(yay(
        abs({arg(i32(), "x")}, i32(), body(return_(var("x")))),
        abs({arg(i32(), "x")}, i32(), body(return_(var("x"))))));
    BOOST_TEST(yay(
        abs({arg(i32(), "x")}, i32(), body(return_(var("x")))),
        abs({arg(i32(), "y")}, i32(), body(return_(var("y"))))));
    BOOST_TEST(nay(
        abs({arg(i32(), "x")}, i32(), body(return_(var("x")))),
        abs({arg(i32(), "x")}, i32(), body(return_(var("y"))))));
    BOOST_TEST(yay(
        abs({arg(i32(), "x")}, i32(), body(return_(var("y")))),
        abs({arg(i32())}, i32(), body(return_(var("y"))))));
    BOOST_TEST(yay(
        abs({arg(i32())}, i32(), body(return_(var("y")))),
        abs({arg(i32())}, i32(), body(return_(var("y"))))));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
