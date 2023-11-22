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
    auto const r1 = std::get_if<stmt_t::return_t>(&x.body.stmts[0].value);
    BOOST_TEST_REQUIRE(r1);
    BOOST_TEST_REQUIRE(r1->expr.has_value());
    auto const f2 = std::get_if<expr_t::abs_t>(&r1->expr->value);
    BOOST_TEST_REQUIRE(f2);
    BOOST_TEST(is_term_binder(f2->args[0], "x", is_type_u32));
    BOOST_TEST(is_term_binder(f2->args[1], "u", is_typename));
    BOOST_TEST(is_var(f2->ret_type.get(), "u"));

    BOOST_TEST_REQUIRE(f2->body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f2->body.stmts[0], testing::var("u")));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
