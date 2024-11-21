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

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_eq(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const eq = std::get_if<typename ast::expr_t<P>::relation_expr_t::eq_t>(&x->value);
    if (not eq)
        return failure("relation not eq_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(eq->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(eq->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto eq(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_eq(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_neq(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const neq = std::get_if<typename ast::expr_t<P>::relation_expr_t::neq_t>(&x->value);
    if (not neq)
        return failure("relation not neq_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(neq->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(neq->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto neq(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_neq(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_gt(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const gt = std::get_if<typename ast::expr_t<P>::relation_expr_t::gt_t>(&x->value);
    if (not gt)
        return failure("relation not gt_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(gt->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(gt->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto gt(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_gt(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_gte(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const gte = std::get_if<typename ast::expr_t<P>::relation_expr_t::gte_t>(&x->value);
    if (not gte)
        return failure("relation not gte_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(gte->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(gte->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto gte(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_gte(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_lt(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const lt = std::get_if<typename ast::expr_t<P>::relation_expr_t::lt_t>(&x->value);
    if (not lt)
        return failure("relation not lt_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(lt->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(lt->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto lt(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_lt(x, f1, f2);
    };
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_lte(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const lte = std::get_if<typename ast::expr_t<P>::relation_expr_t::lte_t>(&x->value);
    if (not lte)
        return failure("relation not lte_t but ", pretty_name(x->value));
    if (auto const result = std::forward<F1>(f1)(lte->lhs.get()); not result)
        return failure("on the left-hand side: ", result.message());
    if (auto const result = std::forward<F2>(f2)(lte->rhs.get()); not result)
        return failure("on the right-hand side: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
constexpr auto lte(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (ast::expr_t<P> const& x)
    {
        return is_lte(x, f1, f2);
    };
}

} // namespace dep0::testing
