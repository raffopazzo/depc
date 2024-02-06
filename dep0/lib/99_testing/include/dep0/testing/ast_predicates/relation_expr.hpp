#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F1, Predicate<ast::expr_t<P>> F2>
boost::test_tools::predicate_result is_gt(ast::expr_t<P> const& expr, F1&& f1, F2&& f2)
{
    auto const* x = std::get_if<typename ast::expr_t<P>::relation_expr_t>(&expr.value);
    if (not x)
        return failure("expression is not relation_expr_t but ", pretty_name(expr.value));
    auto const gt = std::get_if<typename ast::expr_t<P>::relation_expr_t::gt_t>(&x->value);
    if (not gt)
        return failure("relation not gt but ", pretty_name(x->value));
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
        return failure("relation not gte but ", pretty_name(x->value));
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
        return failure("relation not lt but ", pretty_name(x->value));
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
        return failure("relation not lte but ", pretty_name(x->value));
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
