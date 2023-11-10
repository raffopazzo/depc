#pragma once

#include "dep0/ast/max_index.hpp"

#include "dep0/match.hpp"

#include <numeric>

namespace dep0::ast {

template <Properties P>
std::size_t max_index(
    typename type_t<P>::arr_t::arg_const_iterator const begin,
    typename type_t<P>::arr_t::arg_const_iterator const end,
    sort_t<P> const& ret_type)
{
    return std::accumulate(
        begin, end,
        match(
            ret_type,
            [] (ast::typename_t) { return 0ul; },
            [] (type_t<P> const& t) { return max_index(t); }),
        [] (std::size_t const acc, func_arg_t<P> const& arg)
        {
            return std::max(acc, max_index(arg));
        });
}

template <Properties P>
std::size_t max_index(type_t<P> const& type)
{
    return match(
        type.value,
        [] (typename type_t<P>::bool_t const&) { return 0ul; },
        [] (typename type_t<P>::unit_t const&) { return 0ul; },
        [] (typename type_t<P>::i8_t const&) { return 0ul; },
        [] (typename type_t<P>::i16_t const&) { return 0ul; },
        [] (typename type_t<P>::i32_t const&) { return 0ul; },
        [] (typename type_t<P>::i64_t const&) { return 0ul; },
        [] (typename type_t<P>::u8_t const&) { return 0ul; },
        [] (typename type_t<P>::u16_t const&) { return 0ul; },
        [] (typename type_t<P>::u32_t const&) { return 0ul; },
        [] (typename type_t<P>::u64_t const&) { return 0ul; },
        [] (typename type_t<P>::var_t const& x) { return x.name.idx; },
        [] (typename type_t<P>::arr_t const& x) { return max_index(x.args.begin(), x.args.end(), x.ret_type.get()); });
}

template <Properties P>
std::size_t max_index(func_arg_t<P> const& x)
{
    return match(
        x.value,
        [] (typename func_arg_t<P>::type_arg_t const& x)
        {
            return x.var ? x.var->name.idx : 0ul;
        },
        [] (typename func_arg_t<P>::term_arg_t const& x)
        {
            return std::max(max_index(x.type), x.var ? x.var->name.idx : 0ul);
        });
}

template <Properties P>
std::size_t max_index(body_t<P> const& x)
{
    return std::accumulate(
        x.stmts.begin(), x.stmts.end(),
        0ul,
        [] (std::size_t const acc, stmt_t<P> const& s)
        {
            return std::max(
                acc,
                match(
                    s.value,
                    [] (expr_t<P>::app_t const& x)
                    {
                        return max_index<P>(x);
                    },
                    [] (stmt_t<P>::if_else_t const& if_)
                    {
                        return std::max(
                            max_index(if_.cond),
                            std::max(
                                max_index(if_.true_branch),
                                if_.false_branch ? max_index(*if_.false_branch) : 0ul));
                    },
                    [] (stmt_t<P>::return_t const& ret)
                    {
                        return ret.expr ? max_index(*ret.expr) : 0ul;
                    }));
        });
}

template <Properties P>
std::size_t max_index(expr_t<P> const& x)
{
    return match(
        x.value,
        [&] (expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t<P>::arith_expr_t::plus_t const& x)
                {
                    return std::max(max_index(x.lhs.get()), max_index(x.rhs.get()));
                });
        },
        [&] (expr_t<P>::boolean_constant_t const&) { return 0ul; },
        [&] (expr_t<P>::numeric_constant_t const&) { return 0ul; },
        [&] (expr_t<P>::var_t const& x)
        {
            return x.name.idx;
        },
        [&] (expr_t<P>::app_t const& x)
        {
            return max_index<P>(x);
        },
        [&] (expr_t<P>::abs_t const& x)
        {
            return std::accumulate(
                x.args.begin(), x.args.end(),
                max_index(x.body),
                [] (std::size_t const acc, func_arg_t<P> const& arg)
                {
                    return std::max(acc, max_index(arg));
                });
        },
        [&] (type_t<P> const& x)
        {
            return max_index(x);
        });
}

template <Properties P>
std::size_t max_index(typename expr_t<P>::app_t const& x)
{
    return std::accumulate(
        x.args.begin(), x.args.end(),
        max_index(x.func.get()),
        [] (std::size_t const acc, expr_t<P> const& arg)
        {
            return std::max(acc, max_index(arg));
        });
}

} // namespace dep0::ast
