#pragma once

#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"

#include <boost/container_hash/hash.hpp>

#include <algorithm>
#include <numeric>

namespace dep0::ast {

namespace impl {

static std::size_t combine(std::size_t a, std::size_t const b)
{
    boost::hash_combine(a, b);
    return a;
}

static std::size_t combine(std::size_t a, std::size_t const b, std::size_t const c)
{
    boost::hash_combine(a, b);
    boost::hash_combine(a, c);
    return a;
}

template <Properties P> std::size_t hash_code(body_t<P> const&);
template <Properties P> std::size_t hash_code(stmt_t<P> const&);
template <Properties P> std::size_t hash_code(typename expr_t<P>::app_t const&);
template <Properties P> std::size_t hash_code(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body);

template <Properties P>
std::size_t hash_code(body_t<P> const& x)
{
    return std::accumulate(
        x.stmts.begin(), x.stmts.end(),
        0ul,
        [] (std::size_t const acc, stmt_t<P> const& s)
        {
            return combine(acc, hash_code(s));
        });
}

template <Properties P>
std::size_t hash_code(stmt_t<P> const& x)
{
    return combine(
        x.value.index(),
        match(
            x.value,
            [] (expr_t<P>::app_t const& x)
            {
                return hash_code<P>(x);
            },
            [] (stmt_t<P>::if_else_t const& if_)
            {
                return combine(
                    hash_code(if_.cond),
                    hash_code(if_.true_branch),
                    if_.false_branch ? hash_code(*if_.false_branch) : 0ul);
            },
            [] (stmt_t<P>::return_t const& ret)
            {
                return ret.expr ? hash_code(*ret.expr) : 0ul;
            }));
}

template <Properties P>
std::size_t hash_code(typename expr_t<P>::app_t const& x)
{
    return std::accumulate(
        x.args.begin(), x.args.end(),
        hash_code(x.func.get()),
        [] (std::size_t const acc, expr_t<P> const& arg)
        {
            return combine(acc, hash_code(arg));
        });
}

template <Properties P>
std::size_t hash_code(
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    body_t<P> const* body)
{
    return std::accumulate(
        begin, end,
        combine(hash_code(ret_type), body ? hash_code(*body) : 0ul),
        [] (std::size_t const acc, func_arg_t<P> const& arg)
        {
            return combine(acc, hash_code(arg.type));
        });
}

} // namespace impl

template <Properties P>
std::size_t hash_code(expr_t<P> const& x)
{
    return impl::combine(
        x.value.index(),
        match(
            x.value,
            [] (expr_t<P>::typename_t const&) { return 0ul; },
            [] (expr_t<P>::true_t const&) { return 0ul; },
            [] (expr_t<P>::auto_t const&) { return 0ul; },
            [] (expr_t<P>::bool_t const&) { return 0ul; },
            [] (expr_t<P>::unit_t const&) { return 0ul; },
            [] (expr_t<P>::i8_t const&) { return 0ul; },
            [] (expr_t<P>::i16_t const&) { return 0ul; },
            [] (expr_t<P>::i32_t const&) { return 0ul; },
            [] (expr_t<P>::i64_t const&) { return 0ul; },
            [] (expr_t<P>::u8_t const&) { return 0ul; },
            [] (expr_t<P>::u16_t const&) { return 0ul; },
            [] (expr_t<P>::u32_t const&) { return 0ul; },
            [] (expr_t<P>::u64_t const&) { return 0ul; },
            [] (expr_t<P>::boolean_constant_t const&) { return 0ul; },
            [] (expr_t<P>::numeric_constant_t const&) { return 0ul; },
            [] (expr_t<P>::boolean_expr_t const& x)
            {
                return impl::combine(
                    x.value.index(),
                    match(
                        x.value,
                        [] (expr_t<P>::boolean_expr_t::not_t const& x)
                        {
                            return hash_code(x.expr.get());
                        },
                        [] (auto const& x)
                        {
                            return impl::combine(hash_code(x.lhs.get()), hash_code(x.rhs.get()));
                        }));
            },
            [] (expr_t<P>::relation_expr_t const& x)
            {
                return impl::combine(
                    x.value.index(),
                    match(
                        x.value,
                        [] (auto const& x)
                        {
                            return impl::combine(hash_code(x.lhs.get()), hash_code(x.rhs.get()));
                        }));
            },
            [] (expr_t<P>::arith_expr_t const& x)
            {
                return impl::combine(
                    x.value.index(),
                    match(
                        x.value,
                        [] (auto const& x)
                        {
                            return impl::combine(hash_code(x.lhs.get()), hash_code(x.rhs.get()));
                        }));
            },
            [] (expr_t<P>::var_t const&) { return 0ul; },
            [] (expr_t<P>::global_t const&) { return 0ul; },
            [] (expr_t<P>::app_t const& x) { return impl::hash_code<P>(x); },
            [] (expr_t<P>::abs_t const& x)
            {
                return impl::hash_code<P>(x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
            },
            [] (expr_t<P>::pi_t const& x)
            {
                return impl::hash_code<P>(x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
            },
            [] (expr_t<P>::array_t const&)
            {
                return 0ul;
            },
            [] (expr_t<P>::init_list_t const& x)
            {
                return std::accumulate(
                    x.values.begin(), x.values.end(),
                    0ul,
                    [] (std::size_t const acc, expr_t<P> const& v)
                    {
                        return impl::combine(acc, hash_code(v));
                    });
            },
            [] (expr_t<P>::subscript_t const& x)
            {
                return impl::combine(hash_code(x.array.get()), hash_code(x.index.get()));
            }));
}

} // namespace dep0::ast

