/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result is_not_of(ast::expr_t<P> const& expr, F&& f)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::boolean_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not boolean_expr_t but ", pretty_name(expr.value));
    auto const not_ = std::get_if<typename ast::expr_t<P>::boolean_expr_t::not_t>(&x->value);
    if (not not_)
        return failure("boolean expression is not not_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F>(f)(not_->expr.get()); not result)
        return failure("negation expression predicate failed: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
constexpr auto not_of(F&& f)
{
    return [f=std::forward<F>(f)] (ast::expr_t<P> const& x)
    {
        return is_not_of(x, f);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_and_of(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::boolean_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not boolean_expr_t but ", pretty_name(expr.value));
    auto const and_ = std::get_if<typename ast::expr_t<P>::boolean_expr_t::and_t>(&x->value);
    if (not and_)
        return failure("boolean expression is not and_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(and_->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(and_->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto and_of(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_and_of(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_or_of(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::boolean_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not boolean_expr_t but ", pretty_name(expr.value));
    auto const or_ = std::get_if<typename ast::expr_t<P>::boolean_expr_t::or_t>(&x->value);
    if (not or_)
        return failure("boolean expression is not or_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(or_->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(or_->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto or_of(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_or_of(x, f1, f2);
    };
}

} // namespace dep0::testing
