#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P, typename F> void traverse(module_t<P> const&, F&&);
template <Properties P, typename F> void traverse(type_def_t<P> const&, F&&);
template <Properties P, typename F> void traverse(typename type_def_t<P>::integer_t const&, F&&);
template <Properties P, typename F> void traverse(func_def_t<P> const&, F&&);
template <Properties P, typename F> void traverse(type_t<P> const&, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::bool_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::unit_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::i8_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::i16_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::i32_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::i64_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::u8_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::u16_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::u32_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::u64_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::var_t const& x, F&&);
template <Properties P, typename F> void traverse(typename type_t<P>::arr_t const& x, F&&);
template <Properties P, typename F> void traverse(body_t<P> const&, F&&);
template <Properties P, typename F> void traverse(stmt_t<P> const&, F&&);
template <Properties P, typename F> void traverse(typename stmt_t<P>::if_else_t const&, F&&);
template <Properties P, typename F> void traverse(typename stmt_t<P>::return_t const&, F&&);
template <Properties P, typename F> void traverse(expr_t<P> const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::arith_expr_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::arith_expr_t::plus_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::boolean_constant_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::numeric_constant_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::var_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::app_t const&, F&&);
template <Properties P, typename F> void traverse(typename expr_t<P>::abs_t const&, F&&);

namespace detail {

template <typename T, typename F>
void try_visit(T const& x, F&& f)
{
    if constexpr (std::invocable<F, T const&>)
        std::forward<F>(f)(x);
}

} // namespace detail

template <Properties P, typename F>
void traverse(module_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    std::ranges::for_each(x.type_defs, [&] (auto const& x) { traverse(x, f); });
    std::ranges::for_each(x.func_defs, [&] (auto const& x) { traverse(x, f); });
}

template <Properties P, typename F>
void traverse(type_def_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f); });
}

template <Properties P, typename F>
void traverse(typename type_def_t<P>::integer_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(func_def_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    traverse<P>(x.value, f);
}

template <Properties P, typename F>
void traverse(type_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f); });
}

template <Properties P, typename F>
void traverse(typename type_t<P>::bool_t const& x, F&& f)
{ 
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::unit_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::i8_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::i16_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::i32_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::i64_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::u8_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::u16_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::u32_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::u64_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename type_t<P>::var_t const& x, F&& f)
{
    detail::try_visit(x, f);
}


template <Properties P, typename F>
void traverse(typename type_t<P>::arr_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(body_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    std::ranges::for_each(x.stmts, [&] (auto const& x) { traverse(x, f); });
}

template <Properties P, typename F>
void traverse(stmt_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f); });
}

template <Properties P, typename F>
void traverse(typename stmt_t<P>::if_else_t const& x, F&& f)
{
    detail::try_visit(x, f);
    traverse(x.cond, f);
    traverse(x.true_branch, f);
    if (x.false_branch)
        traverse(*x.false_branch, f);
}

template <Properties P, typename F>
void traverse(typename stmt_t<P>::return_t const& x, F&& f)
{
    detail::try_visit(x, f);
    if (x.expr)
        traverse(*x.expr, f);
}

template <Properties P, typename F>
void traverse(expr_t<P> const& x, F&& f)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f); });
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::arith_expr_t const& x, F&& f)
{
    detail::try_visit(x, f);
    match(x.value, [&] (auto const& x) { traverse<P>(x, f); });
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::arith_expr_t::plus_t const& x, F&& f)
{
    detail::try_visit(x, f);
    traverse(x.lhs.get(), f);
    traverse(x.rhs.get(), f);
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::boolean_constant_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::numeric_constant_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::var_t const& x, F&& f)
{
    detail::try_visit(x, f);
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::app_t const& x, F&& f)
{
    detail::try_visit(x, f);
    std::ranges::for_each(x.args, [&] (auto const& x) { traverse(x, f); });
}

template <Properties P, typename F>
void traverse(typename expr_t<P>::abs_t const& x, F&& f)
{
    detail::try_visit(x, f);
    traverse(x.ret_type, f);
    traverse(x.body, f);
}

} // namespace dep0::ast
