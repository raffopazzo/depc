/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/usage.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

#include <boost/scope/scope_exit.hpp>

#include <sstream>

namespace dep0::typecheck {

usage_t::usage_t(scope_map<expr_t::var_t, ast::qty_t> count)
    : count(std::move(count))
{
}

usage_t usage_t::merge(
    usage_t const& a,
    usage_t const& b,
    std::function<ast::qty_t(expr_t::var_t const&, ast::qty_t, ast::qty_t)> f)
{
    auto total_count = scope_map<expr_t::var_t, ast::qty_t>{};
    for (auto const& [var, qty]: a.count)
    {
        auto const inserted = total_count.try_emplace(var, qty).second;
        assert(inserted);
    }
    for (auto const& [var, qty]: b.count)
        if (auto const [it, inserted] = total_count.try_emplace(var, qty); not inserted)
            it->second = f(it->first, it->second, qty);
    return usage_t(std::move(total_count));
}

usage_t usage_t::extend() const
{
    return usage_t{count.extend()};
}

ast::qty_t usage_t::operator[](expr_t::var_t const& var) const
{
    ast::qty_t const* const old = count[var];
    return old ? *old : ast::qty_t::zero;
}

expected<std::true_type>
usage_t::try_add(
    context_lookup_t const& lookup,
    ast::qty_t const usage_multiplier,
    std::optional<source_loc_t> loc)
{
    using enum ast::qty_t;
    if (usage_multiplier == zero) // anything is allowed in an erased context
        return {};
    auto const old_qty = this->operator[](lookup.var);
    auto const new_qty = old_qty + one * usage_multiplier;
    if (new_qty > lookup.decl.qty)
    {
        // The user is trying to use something more than they could;
        // let's try to give them a sensible error message.
        // Firstly, this must mean that the context allowed for only either `zero` or `one`.
        // So there are overall two cases:
        // 1. if the context allowed for zero uses,
        //    then they must have attempted to use the thing at run-time;
        // 2. otherwise the context allowed for only one use and, in that case:
        //    a. either they tried to pass the variable to a function argument
        //       with multiplicity `many` (or something similar to that)
        //    b. or they are trying to use the thing more than once.
        char const* msg =
            lookup.decl.qty == zero ? "variable cannot be used at run-time"
            : old_qty == zero ? "cannot use linear variable in non-linear context"
            : "variable has already been used once";
        return dep0::error_t(msg, loc);
    }
    else
    {
        auto const [it, inserted] = count.try_emplace(lookup.var, new_qty);
        if (not inserted)
            it->second = new_qty;
        return {};
    }
}

expected<std::true_type> usage_t::try_add(ctx_t const& ctx, expr_t const& expr, ast::qty_t const usage_multiplier)
{
    if (usage_multiplier == ast::qty_t::zero) // anything is allowed in an erased context
        return {};
    auto const ok = [] { return expected<std::true_type>{}; };
    auto rollback = boost::scope::make_scope_exit([old=count, this] () mutable { count = std::move(old); });
    auto const result = match(
        expr.value,
        [&] (expr_t::typename_t const&) { return ok(); },
        [&] (expr_t::true_t const&) { return ok(); },
        [&] (expr_t::auto_t const&) { return ok(); },
        [&] (expr_t::bool_t const&) { return ok(); },
        [&] (expr_t::cstr_t const&) { return ok(); },
        [&] (expr_t::unit_t const&) { return ok(); },
        [&] (expr_t::i8_t const&) { return ok(); },
        [&] (expr_t::i16_t const&) { return ok(); },
        [&] (expr_t::i32_t const&) { return ok(); },
        [&] (expr_t::i64_t const&) { return ok(); },
        [&] (expr_t::u8_t const&) { return ok(); },
        [&] (expr_t::u16_t const&) { return ok(); },
        [&] (expr_t::u32_t const&) { return ok(); },
        [&] (expr_t::u64_t const&) { return ok(); },
        [&] (expr_t::boolean_constant_t const&) { return ok(); },
        [&] (expr_t::numeric_constant_t const&) { return ok(); },
        [&] (expr_t::string_literal_t const&) { return ok(); },
        [&] (expr_t::boolean_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t const& x)
                {
                    return try_add(ctx, x.expr.get(), usage_multiplier);
                },
                [&] (auto const& x)
                {
                    auto result = try_add(ctx, x.lhs.get(), usage_multiplier);
                    if (result)
                        result = try_add(ctx, x.rhs.get(), usage_multiplier);
                    return result;
                });
        },
        [&] (expr_t::relation_expr_t const& x)
        {
            return match(
                x.value,
                [&] (auto const& x)
                {
                    auto result = try_add(ctx, x.lhs.get(), usage_multiplier);
                    if (result)
                        result = try_add(ctx, x.rhs.get(), usage_multiplier);
                    return result;
                });
        },
        [&] (expr_t::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (auto const& x)
                {
                    auto result = try_add(ctx, x.lhs.get(), usage_multiplier);
                    if (result)
                        result = try_add(ctx, x.rhs.get(), usage_multiplier);
                    return result;
                });
        },
        [&] (expr_t::var_t const& v) -> expected<std::true_type>
        {
            if (auto lookup = context_lookup(ctx, v))
                return try_add(*lookup, usage_multiplier);
            else
            {
                std::ostringstream err;
                pretty_print<properties_t>(err << "unknown variable `", v) << "` when adding usages";
                return dep0::error_t(err.str());
            }
        },
        [&] (expr_t::global_t const&) { return ok(); },
        [&] (expr_t::app_t const& x)
        {
            auto result = try_add(ctx, x.func.get(), usage_multiplier);
            for (auto const& arg: x.args)
                if (result)
                    result = try_add(ctx, arg, usage_multiplier);
            return result;
        },
        [] (expr_t::abs_t const& x) -> expected<std::true_type>
        {
            // TODO look inside the body
            return dep0::error_t("adding usages of abs_t not yet implemented");
        },
        [&] (expr_t::pi_t const&) { return ok(); }, // types never really use anything
        [&] (expr_t::sigma_t const&) { return ok(); }, // types never really use anything
        [&] (expr_t::array_t const&) { return ok(); },
        [&] (expr_t::init_list_t const& x)
        {
            auto result = ok();
            for (auto const& v: x.values)
                if (result)
                    result = try_add(ctx, v, usage_multiplier);
            return result;
        },
        [&] (expr_t::subscript_t const& x)
        {
            auto result = try_add(ctx, x.object.get(), usage_multiplier);
            if (result)
                result = try_add(ctx, x.index.get(), usage_multiplier);
            return result;
        },
        [&] (expr_t::because_t const& x)
        {
            // reasons never use anything, so add usages from the value
            return try_add(ctx, x.value.get(), usage_multiplier);
        });
    rollback.set_active(result.has_error());
    return result;
}

expected<std::true_type> usage_t::try_add(ctx_t const& ctx, usage_t const& that)
{
    auto rollback = boost::scope::make_scope_exit([old=count, this] () mutable { count = std::move(old); });
    for (auto const& [var, qty]: that.count)
        if (auto lookup = context_lookup(ctx, var))
        {
            if (auto result = try_add(*lookup, ast::qty_t::one); not result)
                return result;
        }
        else
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << "unknown variable `", var) << "` when adding usages";
            return dep0::error_t(err.str());
        }
    rollback.set_active(false);
    return {};
}

} // namespace dep0::typecheck
