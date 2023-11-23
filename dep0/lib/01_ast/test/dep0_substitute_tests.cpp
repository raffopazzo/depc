#define BOOST_TEST_MODULE dep0_substitute_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/substitute.hpp"

#include "dep0/testing/ast_predicates.hpp"

#include "ast_tests_fixture.hpp"

using namespace dep0::testing;

namespace dep0::ast {

BOOST_FIXTURE_TEST_SUITE(dep0_substitute_tests, AstTestsFixture)

BOOST_AUTO_TEST_CASE(substitute_inside_argument_type_until_first_rebinding)
{
    auto main =
        abs({}, i32(),
            body(return_(abs({arg(var("u"), "x"), arg(typename_(), "u")}, var("u"), body(return_(var("u")))))));
    auto& x = std::get<expr_t::abs_t>(main.value);
    substitute(var_t("u"), u32(), x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);

    BOOST_TEST_REQUIRE(x.body.stmts.size() == 1ul);
    auto const r = std::get_if<stmt_t::return_t>(&x.body.stmts[0].value);
    BOOST_TEST_REQUIRE(r);
    BOOST_TEST_REQUIRE(r->expr.has_value());
    auto const f = std::get_if<expr_t::abs_t>(&r->expr->value);
    BOOST_TEST_REQUIRE(f);
    BOOST_TEST(is_term_binder(f->args[0], "x", is_type_u32));
    BOOST_TEST(is_term_binder(f->args[1], "u", is_typename));
    BOOST_TEST(is_var(f->ret_type.get(), "u"));

    BOOST_TEST_REQUIRE(f->body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f->body.stmts[0], testing::var("u")));
}

BOOST_AUTO_TEST_CASE(substitute_inside_body)
{
    auto main = abs({arg(var("t"), "x"), arg(var("u"), "y")}, i32(), body(return_(app(var("z"), {var("y")}))));
    auto& f = std::get<expr_t::abs_t>(main.value);
    substitute(var_t("z"), var("y"), f.args.begin(), f.args.end(), f.ret_type.get(), &f.body);

    BOOST_TEST_REQUIRE(f.args.size() == 2ul);
    BOOST_TEST(is_term_binder(f.args[0], "x", testing::var("t")));
    BOOST_TEST_REQUIRE(f.args[1].var.has_value());
    BOOST_TEST(f.args[1].var->idx > 0ul);
    BOOST_TEST(is_type_i32(f.ret_type.get()));

    BOOST_TEST_REQUIRE(f.body.stmts.size() == 1ul);
    BOOST_TEST(
        is_return_of(
            f.body.stmts[0],
            app_of<properties_t>(
                testing::var("y"),
                [&] (expr_t const& y)
                {
                    auto const v = std::get_if<expr_t::var_t>(&y.value);
                    BOOST_TEST_REQUIRE(v);
                    BOOST_TEST(v->name == f.args[1].var->name);
                    BOOST_TEST(v->idx == f.args[1].var->idx);
                    return boost::test_tools::predicate_result(true);
                })));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
