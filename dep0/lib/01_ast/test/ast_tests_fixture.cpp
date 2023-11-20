#define BOOST_TEST_MODULE dep0_alpha_equivalence_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "dep0/testing/literal_string.hpp"

using namespace dep0::testing;

namespace dep0::ast {

struct dummy_properties_t
{
    using module_properties_type = dummy_properties_t;
    using type_def_properties_type = dummy_properties_t;
    using func_def_properties_type = dummy_properties_t;
    using func_arg_properties_type = dummy_properties_t;
    using body_properties_type = dummy_properties_t;
    using stmt_properties_type = dummy_properties_t;
    using expr_properties_type = dummy_properties_t;
};
static_assert(Properties<dummy_properties_t>);

struct Fixture
{
    using module_t = dep0::ast::module_t<dummy_properties_t>;
    using type_def_t = dep0::ast::type_def_t<dummy_properties_t>;
    using func_def_t = dep0::ast::func_def_t<dummy_properties_t>;
    using func_arg_t = dep0::ast::func_arg_t<dummy_properties_t>;
    using body_t = dep0::ast::body_t<dummy_properties_t>;
    using stmt_t = dep0::ast::stmt_t<dummy_properties_t>;
    using expr_t = dep0::ast::expr_t<dummy_properties_t>;

    func_def_t func_def(char const* const name, std::vector<expr_t::abs_t::arg_t> args, expr_t ret, body_t body)
    {
        return func_def_t{
            dummy_properties_t{},
            literal_string(name),
            expr_t::abs_t{std::move(args), std::move(ret), std::move(body)}
        };
    }

    expr_t typename_() { return expr_t{dummy_properties_t{}, expr_t::typename_t{}}; }
    expr_t i32() { return expr_t{dummy_properties_t{}, expr_t::i32_t{}}; }
    expr_t u32() { return expr_t{dummy_properties_t{}, expr_t::u32_t{}}; }
    body_t body(stmt_t s) { return body_t{dummy_properties_t{}, std::vector{std::move(s)}}; }
    stmt_t return_(expr_t e) { return stmt_t{dummy_properties_t{}, stmt_t::return_t{std::move(e)}}; }
    expr_t app(expr_t f, std::vector<expr_t> args)
    {
        return expr_t{dummy_properties_t{}, expr_t::app_t{std::move(f), std::move(args)}};
    }
    expr_t numeric_constant(char const* const number)
    {
        return expr_t{
            dummy_properties_t{},
            expr_t::numeric_constant_t{std::nullopt, literal_string(number)}};
    }
    expr_t var(char const* const name)
    {
        return expr_t{dummy_properties_t{}, expr_t::var_t{literal_string(name)}};
    }
    func_arg_t arg(expr_t type, char const* const var_name = nullptr)
    {
        return func_arg_t{
            dummy_properties_t{},
            std::move(type),
            var_name ? std::optional{expr_t::var_t{literal_string(var_name)}} : std::nullopt
        };
    }
    expr_t pi(std::vector<func_arg_t> args, expr_t ret_type)
    {
        return expr_t{dummy_properties_t{}, expr_t::pi_t{std::move(args), std::move(ret_type)}};
    }

    static boost::test_tools::predicate_result yay(expr_t const& x, expr_t const& y)
    {
        auto result = is_alpha_equivalent(x, y);
        if (result)
            return true;
        else
        {
            auto failed = boost::test_tools::predicate_result(false);
            pretty_print(failed.message().stream(), result.error());
            return failed;
        }
    }

    static boost::test_tools::predicate_result nay(expr_t const& x, expr_t const& y)
    {
        auto result = is_alpha_equivalent(x, y);
        if (not result)
            return true;
        else
        {
            auto failed = boost::test_tools::predicate_result(false);
            pretty_print(failed.message().stream() << '`', x) << '`';
            pretty_print(failed.message().stream() << " should not be alpha-equivalent to `", y) << '`';
            return failed;
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_alpha_equivalence_tests, Fixture)

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

BOOST_AUTO_TEST_SUITE_END()

} // namespace dep0::ast
