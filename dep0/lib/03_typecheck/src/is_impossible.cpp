/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/is_impossible.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <ranges>

namespace dep0::typecheck {

namespace impl {

static bool is_impossible(stmt_t const&);

static bool is_impossible(expr_t const&);
static bool is_impossible(expr_t::typename_t const&) { return false; }
static bool is_impossible(expr_t::true_t const&) { return false; }
static bool is_impossible(expr_t::auto_t const&) { return false; }
static bool is_impossible(expr_t::bool_t const&) { return false; }
static bool is_impossible(expr_t::cstr_t const&) { return false; }
static bool is_impossible(expr_t::unit_t const&) { return false; }
static bool is_impossible(expr_t::i8_t const&) { return false; }
static bool is_impossible(expr_t::i16_t const&) { return false; }
static bool is_impossible(expr_t::i32_t const&) { return false; }
static bool is_impossible(expr_t::i64_t const&) { return false; }
static bool is_impossible(expr_t::u8_t const&) { return false; }
static bool is_impossible(expr_t::u16_t const&) { return false; }
static bool is_impossible(expr_t::u32_t const&) { return false; }
static bool is_impossible(expr_t::u64_t const&) { return false; }
static bool is_impossible(expr_t::boolean_constant_t const&) { return false; }
static bool is_impossible(expr_t::numeric_constant_t const&) { return false; }
static bool is_impossible(expr_t::string_literal_t const&) { return false; }
static bool is_impossible(expr_t::boolean_expr_t const&);
static bool is_impossible(expr_t::relation_expr_t const&);
static bool is_impossible(expr_t::arith_expr_t const&);
static bool is_impossible(expr_t::var_t const&) { return false; }
static bool is_impossible(expr_t::global_t const&) { return false; }
static bool is_impossible(expr_t::app_t const&);
static bool is_impossible(expr_t::abs_t const&);
static bool is_impossible(expr_t::pi_t const&);
static bool is_impossible(expr_t::sigma_t const&);
static bool is_impossible(expr_t::array_t const&) { return false; }
static bool is_impossible(expr_t::init_list_t const&);
static bool is_impossible(expr_t::subscript_t const&);
static bool is_impossible(expr_t::because_t const&);

static bool is_impossible(
    std::vector<func_arg_t>::const_iterator begin,
    std::vector<func_arg_t>::const_iterator end,
    expr_t const* ret_type, // nullptr for sigma-types
    body_t const*);

bool is_impossible(stmt_t const& s)
{
    return match(
        s.value,
        [] (expr_t::app_t const& x) { return impl::is_impossible(x); },
        [] (stmt_t::if_else_t const& x)
        {
            // an if-statement is impossible if the condition is impossible or
            // both branches are present and both impossible.
            return impl::is_impossible(x.cond) or
                x.false_branch and is_impossible(x.true_branch) and is_impossible(*x.false_branch);
        },
        [] (stmt_t::return_t const& x)
        {
            return x.expr and impl::is_impossible(*x.expr);
        },
        [] (stmt_t::impossible_t const&)
        {
            return true;
        });
}

bool is_impossible(expr_t const& x)
{
    return match(x.value, [] (auto const& x) { return is_impossible(x); });
}

bool is_impossible(expr_t::boolean_expr_t const& x)
{
    return match(
        x.value,
        [] (expr_t::boolean_expr_t::not_t const& x)
        {
            return is_impossible(x.expr.get());
        },
        [] (auto const& x)
        {
            return is_impossible(x.lhs.get()) or is_impossible(x.rhs.get());
        });
}

bool is_impossible(expr_t::relation_expr_t const& x)
{
    return match(
        x.value,
        [] (auto const& x)
        {
            return is_impossible(x.lhs.get()) or is_impossible(x.rhs.get());
        });
}

bool is_impossible(expr_t::arith_expr_t const& x)
{
    return match(
        x.value,
        [] (auto const& x)
        {
            return is_impossible(x.lhs.get()) or is_impossible(x.rhs.get());
        });
}

bool is_impossible(expr_t::app_t const& x)
{
    if (is_impossible(x.func.get()))
        return true;
    // special case for a lambda that contains an impossible body
    if (auto const abs = std::get_if<expr_t::abs_t>(&x.func.get().value))
        if (is_impossible(abs->body))
            return true;
    return std::ranges::any_of(x.args, [] (expr_t const& x) { return is_impossible(x); });
}

bool is_impossible(expr_t::abs_t const& x)
{
    // Passing nullptr to body because constructing a lambda is impossible only if
    // its argument types or return type are impossible to construct.
    // It is still possible to construct a lambda whose body is impossible;
    // it is impossible to invoke it, but it can still be constructed.
    return is_impossible(x.args.begin(), x.args.end(), &x.ret_type.get(), nullptr);
}

bool is_impossible(expr_t::pi_t const& x)
{
    return is_impossible(x.args.begin(), x.args.end(), &x.ret_type.get(), nullptr);
}

bool is_impossible(expr_t::sigma_t const& x)
{
    return is_impossible(x.args.begin(), x.args.end(), nullptr, nullptr);
}

bool is_impossible(expr_t::init_list_t const& x)
{
    return std::ranges::any_of(x.values, [] (expr_t const& x) { return is_impossible(x); });
}

bool is_impossible(expr_t::subscript_t const& x)
{
    return is_impossible(x.array.get()) or is_impossible(x.index.get());
}

bool is_impossible(expr_t::because_t const& x)
{
    return is_impossible(x.value.get());
}

bool is_impossible(
    std::vector<func_arg_t>::const_iterator begin,
    std::vector<func_arg_t>::const_iterator end,
    expr_t const* ret_type,
    body_t const* body)
{
    return std::any_of(begin, end, [] (func_arg_t const& x) { return is_impossible(x.type); })
        or ret_type and is_impossible(*ret_type)
        or body and is_impossible(*body);
}

} // namespace impl

bool is_impossible(body_t const& body)
{
    return is_impossible(body.stmts.begin(), body.stmts.end());
}

bool is_impossible(std::vector<stmt_t>::const_iterator const begin, std::vector<stmt_t>::const_iterator const end)
{
    for (auto const& stmt: std::ranges::subrange(begin, end))
    {
        if (impl::is_impossible(stmt))
            return true;
        // An `if` without else-branch whose both condition and true-branch are possible,
        // means that the entire body is possible, because a possible execution path exists.
        // Perhaps the remaining statements below the `if` (i.e. the implicit else-branch)
        // are impossible (and should be removed) but the body as a whole is still possible.
        if (auto const if_else = std::get_if<stmt_t::if_else_t>(&stmt.value))
            if (not if_else->false_branch)
                if (not impl::is_impossible(if_else->cond) and not is_impossible(if_else->true_branch))
                    return false;
    }
    return false;
}

} // namespace dep0::typecheck

