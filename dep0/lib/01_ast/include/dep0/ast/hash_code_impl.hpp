#pragma once

#include "dep0/ast/hash_code.hpp"

#include "dep0/match.hpp"

#include <boost/container_hash/hash.hpp>

#include <map>
#include <ranges>

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

/**
 * Helper object to keep track of unique IDs assigned in place of ordinary variable names.
 * This is required to ensure that the hash code of alpha-equivalent expressions are the same.
 * It works somewhat similarly to a DeBrujin index in the sense that each binder is assigned a unique ID.
 * Under this mapping, the hash code of a variable name is either:
 *   - its unique ID, if it is bound to a function argument;
 *   - or the hash of its actual name, if it is a free variable.
 */
template <typename P>
class hash_code_state_t
{
    std::size_t next_id = 0ul;
    std::map<typename expr_t<P>::var_t, std::size_t> unique_var_id;

public:
    void store(typename expr_t<P>::var_t const& x)
    {
        unique_var_id[x] = next_id++;
    }

    std::optional<std::size_t> find(typename expr_t<P>::var_t const& x)
    {
        auto const it = unique_var_id.find(x);
        if (it == unique_var_id.end())
            return std::nullopt;
        else
            return it->second;
    }
};

template <Properties P> std::size_t hash_code_impl(hash_code_state_t<P>&, body_t<P> const&);
template <Properties P> std::size_t hash_code_impl(hash_code_state_t<P>&, stmt_t<P> const&);
template <Properties P> std::size_t hash_code_impl(hash_code_state_t<P>&, typename expr_t<P>::app_t const&);
template <Properties P> std::size_t hash_code_impl(
    hash_code_state_t<P>&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body);

template <Properties P>
std::size_t hash_code_impl(hash_code_state_t<P>& state, body_t<P> const& x)
{
    std::size_t result = 0ul;
    for (auto const& s: x.stmts)
        boost::hash_combine(result, hash_code_impl(state, s));
    return result;
}

template <Properties P>
std::size_t hash_code_impl(hash_code_state_t<P>& state, stmt_t<P> const& x)
{
    return combine(
        x.value.index(),
        match(
            x.value,
            [&] (expr_t<P>::app_t const& x)
            {
                return hash_code_impl<P>(state, x);
            },
            [&] (stmt_t<P>::if_else_t const& if_)
            {
                return combine(
                    hash_code_impl(state, if_.cond),
                    hash_code_impl(state, if_.true_branch),
                    if_.false_branch ? hash_code_impl(state, *if_.false_branch) : 0ul);
            },
            [&] (stmt_t<P>::return_t const& ret)
            {
                return ret.expr ? hash_code_impl(state, *ret.expr) : 0ul;
            }));
}

template <Properties P>
std::size_t hash_code_impl(hash_code_state_t<P>& state, typename expr_t<P>::app_t const& x)
{
    std::size_t result = hash_code_impl(state, x.func.get());
    for (auto const& v: x.args)
        boost::hash_combine(result, hash_code_impl(state, v));
    return result;
}

template <Properties P>
std::size_t hash_code_impl(
    hash_code_state_t<P>& state,
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    body_t<P> const* body)
{
    std::size_t result = 0ul;
    for (auto const& arg: std::ranges::subrange(begin, end))
    {
        if (arg.var)
            state.store(*arg.var);
        boost::hash_combine(result, boost::hash_value(arg.qty));
        boost::hash_combine(result, hash_code_impl(state, arg.type));
    }
    boost::hash_combine(result, hash_code_impl(state, ret_type));
    if (body)
        boost::hash_combine(result, hash_code_impl(state, *body));
    return result;
}

template <Properties P>
std::size_t hash_code_impl(hash_code_state_t<P>& state, expr_t<P> const& x)
{
    return combine(
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
            [] (expr_t<P>::boolean_constant_t const& x) { return static_cast<std::size_t>(x.value); },
            [] (expr_t<P>::numeric_constant_t const& x) { return hash_value(x.value); },
            [&] (expr_t<P>::boolean_expr_t const& x)
            {
                return combine(
                    x.value.index(),
                    match(
                        x.value,
                        [&] (expr_t<P>::boolean_expr_t::not_t const& x)
                        {
                            return hash_code_impl(state, x.expr.get());
                        },
                        [&] (auto const& x)
                        {
                            return combine(hash_code_impl(state, x.lhs.get()), hash_code_impl(state, x.rhs.get()));
                        }));
            },
            [&] (expr_t<P>::relation_expr_t const& x)
            {
                return combine(
                    x.value.index(),
                    match(
                        x.value,
                        [&] (auto const& x)
                        {
                            return combine(hash_code_impl(state, x.lhs.get()), hash_code_impl(state, x.rhs.get()));
                        }));
            },
            [&] (expr_t<P>::arith_expr_t const& x)
            {
                return combine(
                    x.value.index(),
                    match(
                        x.value,
                        [&] (auto const& x)
                        {
                            return combine(hash_code_impl(state, x.lhs.get()), hash_code_impl(state, x.rhs.get()));
                        }));
            },
            [&] (expr_t<P>::var_t const& x)
            {
                if (auto const id = state.find(x))
                    return *id;
                else
                    return combine(x.idx, boost::hash_value(x.name.view()));
            },
            [] (expr_t<P>::global_t const& x) { return boost::hash_value(x.name.view()); },
            [&] (expr_t<P>::app_t const& x) { return hash_code_impl<P>(state, x); },
            [&] (expr_t<P>::abs_t const& x)
            {
                return hash_code_impl<P>(state, x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
            },
            [&] (expr_t<P>::pi_t const& x)
            {
                return hash_code_impl<P>(state, x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
            },
            [] (expr_t<P>::array_t const&)
            {
                return 0ul;
            },
            [&] (expr_t<P>::init_list_t const& x)
            {
                std::size_t result = 0ul;
                for (auto const& v: x.values)
                    boost::hash_combine(result, hash_code_impl(state, v));
                return result;
            },
            [&] (expr_t<P>::subscript_t const& x)
            {
                return combine(hash_code_impl(state, x.array.get()), hash_code_impl(state, x.index.get()));
            }));
}

} // namespace impl

template <Properties P>
std::size_t hash_code(expr_t<P> const& x)
{
    impl::hash_code_state_t<P> state;
    return impl::hash_code_impl(state, x);
}

} // namespace dep0::ast

