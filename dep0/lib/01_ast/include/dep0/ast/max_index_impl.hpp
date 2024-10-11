#pragma once

#include "dep0/ast/max_index.hpp"

#include "dep0/match.hpp"

#include <numeric>

namespace dep0::ast {

namespace impl {

template <Properties P> std::size_t max_index(func_arg_t<P> const&);
template <Properties P> std::size_t max_index(body_t<P> const&);
template <Properties P> std::size_t max_index(expr_t<P> const&);
template <Properties P> std::size_t max_index(typename expr_t<P>::app_t const&);

template <Properties P>
std::size_t max_index(func_arg_t<P> const& x)
{
    return std::max(max_index(x.type), x.var ? x.var->idx : 0ul);
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
                    },
                    [](stmt_t<P>::impossible_t const& x)
                    {
                        return x.reason ? max_index(*x.reason) : 0ul;
                    }));
        });
}

template <Properties P>
std::size_t max_index(expr_t<P> const& x)
{
    return match(
        x.value,
        [] (expr_t<P>::typename_t const&) { return 0ul; },
        [] (expr_t<P>::true_t const&) { return 0ul; },
        [] (expr_t<P>::auto_t const&) { return 0ul; },
        [] (expr_t<P>::bool_t const&) { return 0ul; },
        [] (expr_t<P>::cstr_t const&) { return 0ul; },
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
        [] (expr_t<P>::string_literal_t const&) { return 0ul; },
        [] (expr_t<P>::boolean_expr_t const& x)
        {
            return match(
                x.value,
                [] (expr_t<P>::boolean_expr_t::not_t const& x)
                {
                    return max_index(x.expr.get());
                },
                [] (auto const& x)
                {
                    return std::max(max_index(x.lhs.get()), max_index(x.rhs.get()));
                });
        },
        [] (expr_t<P>::relation_expr_t const& x)
        {
            return match(
                x.value,
                [] (auto const& x)
                {
                    return std::max(max_index(x.lhs.get()), max_index(x.rhs.get()));
                });
        },
        [] (expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [] (auto const& x)
                {
                    return std::max(max_index(x.lhs.get()), max_index(x.rhs.get()));
                });
        },
        [] (expr_t<P>::var_t const& x)
        {
            return x.idx;
        },
        [] (expr_t<P>::global_t const&)
        {
            return 0ul;
        },
        [] (expr_t<P>::app_t const& x)
        {
            return max_index<P>(x);
        },
        [] (expr_t<P>::abs_t const& x)
        {
            return max_index<P>(x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
        },
        [] (expr_t<P>::pi_t const& x)
        {
            return max_index<P>(x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
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
                    return std::max(acc, max_index(v));
                });
        },
        [] (expr_t<P>::subscript_t const& x)
        {
            return std::max(max_index(x.array.get()), max_index(x.index.get()));
        },
        [] (expr_t<P>::because_t const& x)
        {
            return std::max(max_index(x.value.get()), max_index(x.reason.get()));
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

} // namespace impl

template <Properties P>
std::size_t max_index(
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    body_t<P> const* body)
{
    return std::accumulate(
        begin, end,
        std::max(impl::max_index(ret_type), body ? impl::max_index(*body) : 0ul),
        [] (std::size_t const acc, func_arg_t<P> const& arg)
        {
            return std::max(acc, impl::max_index(arg));
        });
}

} // namespace dep0::ast
