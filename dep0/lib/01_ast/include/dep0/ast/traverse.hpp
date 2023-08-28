#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P, typename OnEnter, typename OnExit> void traverse(module_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(type_def_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_def_t<P>::integer_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(func_def_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(type_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::bool_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::unit_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::i8_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::i16_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::i32_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::i64_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::u8_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::u16_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::u32_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::u64_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::var_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename type_t<P>::arr_t const& x, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(body_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(stmt_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename stmt_t<P>::if_else_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename stmt_t<P>::return_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(expr_t<P> const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::arith_expr_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::arith_expr_t::plus_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::boolean_constant_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::numeric_constant_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::var_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::app_t const&, OnEnter&&, OnExit&&);
template <Properties P, typename OnEnter, typename OnExit> void traverse(typename expr_t<P>::abs_t const&, OnEnter&&, OnExit&&);

namespace detail {

template <typename T, typename F>
void try_visit(T const& x, F&& f)
{
    if constexpr (std::invocable<F, T const&>)
        std::forward<F>(f)(x);
}

} // namespace detail

template <Properties P, typename OnEnter, typename OnExit>
void traverse(module_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    std::ranges::for_each(x.type_defs, [&] (auto const& x) { traverse(x, f, g); });
    std::ranges::for_each(x.func_defs, [&] (auto const& x) { traverse(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(type_def_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_def_t<P>::integer_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(func_def_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    traverse<P>(x.value, f, g);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(type_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::bool_t const& x, OnEnter&& f, OnExit&& g)
{ 
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::unit_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::i8_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::i16_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::i32_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::i64_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::u8_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::u16_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::u32_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::u64_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::var_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}


template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename type_t<P>::arr_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(body_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    std::ranges::for_each(x.stmts, [&] (auto const& x) { traverse(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(stmt_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename stmt_t<P>::if_else_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    traverse(x.cond, f, g);
    traverse(x.true_branch, f, g);
    if (x.false_branch)
        traverse(*x.false_branch, f, g);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename stmt_t<P>::return_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    if (x.expr)
        traverse(*x.expr, f, g);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(expr_t<P> const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::arith_expr_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::arith_expr_t::plus_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    traverse(x.lhs.get(), f, g);
    traverse(x.rhs.get(), f, g);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::boolean_constant_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::numeric_constant_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::var_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::app_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    traverse(x.func.get(), f, g);
    std::ranges::for_each(x.args, [&] (auto const& x) { traverse(x, f, g); });
    detail::try_visit(x, g);
}

template <Properties P, typename OnEnter, typename OnExit>
void traverse(typename expr_t<P>::abs_t const& x, OnEnter&& f, OnExit&& g)
{
    detail::try_visit(x, f);
    traverse(x.ret_type, f, g);
    traverse(x.body, f, g);
    detail::try_visit(x, g);
}

} // namespace dep0::ast
