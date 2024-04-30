#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_boolean_constant(ast::expr_t<P> const& expr, bool const x)
{
    auto const c = std::get_if<typename ast::expr_t<P>::boolean_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not boolean_constant_t but ", pretty_name(expr.value));
    if (c->value != x)
        return failure("boolean constant is not ", x, " but ", c->value);
    return true;
}

template <ast::Properties P>
boost::test_tools::predicate_result is_numeric_constant(ast::expr_t<P> const& expr, int const x)
{
    auto const c = std::get_if<typename ast::expr_t<P>::numeric_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not numeric_constant_t but ", pretty_name(expr.value));
    if (c->value != x)
        return failure("numeric constant ", c->value, " != ", x);
    return true;
}

template <ast::Properties P>
boost::test_tools::predicate_result
is_numeric_constant(ast::expr_t<P> const& expr, boost::multiprecision::cpp_int const& x)
{
    auto const c = std::get_if<typename ast::expr_t<P>::numeric_constant_t>(&expr.value);
    if (not c)
        return failure("expression is not numeric_constant_t but ", pretty_name(expr.value));
    if (c->value != x)
        return failure("numeric constant ", c->value, " != ", x);
    return true;
}

template <ast::Properties P>
boost::test_tools::predicate_result
is_string_literal(ast::expr_t<P> const& expr, std::string_view const x)
{
    auto const s = std::get_if<typename ast::expr_t<P>::string_literal_t>(&expr.value);
    if (not s)
        return failure("expression is not string_literal_t but ", pretty_name(expr.value));
    if (s->value != x)
        return failure("string literal ", s->value, " != ", x);
    return true;
}

inline auto constant(bool value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_boolean_constant(expr, value);
    };
}

inline auto constant(int const value)
{
    return [value] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_numeric_constant(expr, value);
    };
}

inline auto constant(char const* const value) // not std::string_view otherwise bool wins via standard conversion rules
{
    return [value=boost::multiprecision::cpp_int(value)] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_numeric_constant(expr, value);
    };
}

inline auto string_literal(std::string_view const x)
{
    return [x] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_string_literal(expr, x);
    };
}

} // namespace dep0::testing
