#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_div(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::arith_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not arith_expr_t but ", pretty_name(expr.value));
    auto const* div = std::get_if<typename ast::expr_t<P>::arith_expr_t::div_t>(&x->value);
    if (not div)
        return failure("arithmetic expression is not div_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(div->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(div->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto div(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_div(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_minus(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::arith_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not arith_expr_t but ", pretty_name(expr.value));
    auto const* minus = std::get_if<typename ast::expr_t<P>::arith_expr_t::minus_t>(&x->value);
    if (not minus)
        return failure("arithmetic expression is not minus_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(minus->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(minus->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto minus(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_minus(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_mult(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::arith_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not arith_expr_t but ", pretty_name(expr.value));
    auto const* mult = std::get_if<typename ast::expr_t<P>::arith_expr_t::mult_t>(&x->value);
    if (not mult)
        return failure("arithmetic expression is not mult_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(mult->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(mult->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto mult(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_mult(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_plus(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::arith_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not arith_expr_t but ", pretty_name(expr.value));
    auto const* plus = std::get_if<typename ast::expr_t<P>::arith_expr_t::plus_t>(&x->value);
    if (not plus)
        return failure("arithmetic expression is not plus_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(plus->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(plus->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto plus(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_plus(x, f1, f2);
    };
}

} // namespace dep0::testing
