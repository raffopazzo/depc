/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/type_assign.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/check.hpp"
#include "private/derivation_rules.hpp"
#include "private/place_expression.hpp"
#include "private/proof_search.hpp"
#include "private/returns_from_all_branches.hpp"
#include "private/substitute.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"
#include "dep0/typecheck/subscript_access.hpp"

#include "dep0/ast/find_member_field.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/ast/views.hpp"

#include "dep0/match.hpp"

#include <boost/hana.hpp>

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

static expected<expr_t>
type_assign_global(
    env_t const& env,
    ctx_t const& ctx,
    expr_t::global_t global,
    source_loc_t const loc,
    ast::qty_t const usage_multiplier)
{
    auto const def = env[global];
    if (not def)
        return dep0::error_t("global symbol not found", loc);
    return match(
        *def,
        [&] (env_t::incomplete_type_t const&) -> expected<expr_t>
        {
            return make_legal_expr(derivation_rules::make_typename(), std::move(global));
        },
        [&] (type_def_t const&) -> expected<expr_t>
        {
            return make_legal_expr(derivation_rules::make_typename(), std::move(global));
        },
        [&] (axiom_t const& x) -> expected<expr_t>
        {
            if (usage_multiplier > ast::qty_t::zero)
                return dep0::error_t("axiom cannot be used at run-time", loc);
            return make_legal_expr(x.properties.sort.get(), std::move(global));
        },
        [&] (extern_decl_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(x.properties.sort.get(), std::move(global));
        },
        [&] (func_decl_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(x.properties.sort.get(), std::move(global));
        },
        [&] (func_def_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(x.properties.sort.get(), std::move(global));
        });
}

static expected<expr_t>
type_assign_var(
    env_t const& env,
    ctx_t const& ctx,
    source_text const name,
    source_loc_t const loc,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    if (auto lookup = context_lookup(ctx, expr_t::var_t{name}))
    {
        if (auto ok = usage.try_add(*lookup, usage_multiplier, loc); not ok)
            return ok.error();
        return make_legal_expr(lookup->decl.type, lookup->var);
    }
    if (auto const global = expr_t::global_t{std::nullopt, name}; env[global])
        return type_assign_global(env, ctx, global, loc, usage_multiplier);
    else
        return dep0::error_t("unknown variable", loc);
}

static expected<expr_t>
type_assign_scopeof(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t const& expr,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto const loc = expr.properties;
    auto const accept = [&] (expr_t expr) -> expected<expr_t>
    {
        return make_legal_expr(derivation_rules::make_scope_t(), expr_t::scopeof_t(std::move(expr)));
    };
    auto const reject = [&] (std::string err) -> expected<expr_t> { return error_t(std::move(err), loc); };
    if (auto const var = std::get_if<parser::expr_t::var_t>(&expr.value))
    {
        if (auto lookup = context_lookup(ctx, expr_t::var_t{var->name}))
        {
            if (not lookup->is_scoped)
                return reject("variable not bound to a scope");
            if (auto ok = usage.try_add(*lookup, usage_multiplier, loc); not ok)
                return ok.error();
            return accept(make_legal_expr(lookup->decl.type, lookup->var));
        }
        auto g = expr_t::global_t{std::nullopt, var->name};
        if (auto const p = env[g])
            return match(
                *p,
                [&] (env_t::incomplete_type_t) { return reject("a type definition has no scope"); },
                [&] (type_def_t const&) { return reject("a type definition has no scope"); },
                [&] (axiom_t const&) { return reject("an axiom has no scope"); },
                [&] (extern_decl_t const& x) { return accept(make_legal_expr(x.properties.sort.get(), std::move(g))); },
                [&] (func_decl_t const& x) { return accept(make_legal_expr(x.properties.sort.get(), std::move(g))); },
                [&] (func_def_t const& x) { return accept(make_legal_expr(x.properties.sort.get(), std::move(g))); });
        return dep0::error_t("unknown variable", loc);
    }
    else
    {
        // NOTE: mutable expression is allowed inside `scopeof()` because it's never really evaluated
        auto ok = type_assign(env, ctx, expr, ast::is_mutable_t::yes, usage, usage_multiplier);
        if (not ok)
            return std::move(ok.error());
        if (not ctx.is_scoped())
            return reject("temporary expression is not bound to a scope");
        return accept(std::move(*ok));
    }
}

std::pair<expected<expr_t>, expected<expr_t>>
type_assign_pair(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t const& lhs,
    parser::expr_t const& rhs,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    // If we can assign a type to lhs, we check rhs against that, and we are done.
    // Otherwise assign a type to rhs and check lhs against that, and we are done.
    // In either case, type-assignment is speculative so we use a temporary usage object;
    // only if type-assignment succeeds we commit to the real usage object.
    auto tmp_usage_a = usage.extend();
    auto tmp_usage_b = usage.extend();
    auto a = type_assign(env, ctx, lhs, is_mutable_allowed, tmp_usage_a, usage_multiplier);
    if (a)
        if (auto ok = usage.try_add(ctx, tmp_usage_a); not ok)
            return {ok.error(), ok.error()};
    auto b = a
        ? check_expr(env, ctx, rhs, a->properties.sort.get(), is_mutable_allowed, usage, usage_multiplier)
        : type_assign(env, ctx, rhs, is_mutable_allowed, tmp_usage_b, usage_multiplier);
    if (b and not a)
    {
        if (auto ok = usage.try_add(ctx, tmp_usage_b); not ok)
            return {ok.error(), ok.error()};
        a = check_expr(env, ctx, lhs, b->properties.sort.get(), is_mutable_allowed, usage, usage_multiplier);
    }
    return std::pair{std::move(a), std::move(b)};
}

expected<expr_t>
type_assign(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t const& expr,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto const loc = expr.properties;
    return match(
        expr.value,
        [] (parser::expr_t::typename_t) -> expected<expr_t> { return derivation_rules::make_typename(); },
        [] (parser::expr_t::true_t) -> expected<expr_t> { return derivation_rules::make_true_t(); },
        [&] (parser::expr_t::auto_t) -> expected<expr_t>
        {
            return dep0::error_t("auto expressions have no unique type", loc);
        },
        [] (parser::expr_t::bool_t) -> expected<expr_t> { return derivation_rules::make_bool(); },
        [] (parser::expr_t::cstr_t) -> expected<expr_t> { return derivation_rules::make_cstr(); },
        [] (parser::expr_t::unit_t) -> expected<expr_t> { return derivation_rules::make_unit(); },
        [] (parser::expr_t::i8_t) -> expected<expr_t> { return derivation_rules::make_i8(); },
        [] (parser::expr_t::i16_t) -> expected<expr_t> { return derivation_rules::make_i16(); },
        [] (parser::expr_t::i32_t) -> expected<expr_t> { return derivation_rules::make_i32(); },
        [] (parser::expr_t::i64_t) -> expected<expr_t> { return derivation_rules::make_i64(); },
        [] (parser::expr_t::u8_t) -> expected<expr_t> { return derivation_rules::make_u8(); },
        [] (parser::expr_t::u16_t) -> expected<expr_t> { return derivation_rules::make_u16(); },
        [] (parser::expr_t::u32_t) -> expected<expr_t> { return derivation_rules::make_u32(); },
        [] (parser::expr_t::u64_t) -> expected<expr_t> { return derivation_rules::make_u64(); },
        [] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(derivation_rules::make_bool(), expr_t::boolean_constant_t{x.value});
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return dep0::error_t("numeric constants have no unique type", loc);
        },
        [&] (parser::expr_t::string_literal_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(derivation_rules::make_cstr(), expr_t::string_literal_t{x.value});
        },
        [&] (parser::expr_t::boolean_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::boolean_expr_t::not_t const& x) -> expected<expr_t>
                {
                    auto expr =
                        check_expr(
                            env, ctx,
                            x.expr.get(),
                            derivation_rules::make_bool(),
                            is_mutable_allowed,
                            usage, usage_multiplier);
                    if (expr)
                        return make_legal_expr(
                            derivation_rules::make_bool(),
                            expr_t::boolean_expr_t{
                                expr_t::boolean_expr_t::not_t{
                                    std::move(*expr)}});
                    else
                        return std::move(expr.error());
                },
                [&] <typename T> (T const& x) -> expected<expr_t>
                {
                    auto lhs =
                        check_expr(
                            env, ctx, x.lhs.get(), derivation_rules::make_bool(),
                            is_mutable_allowed, usage, usage_multiplier);
                    if (not lhs)
                        return std::move(lhs.error());
                    auto rhs =
                        check_expr(
                            env, ctx, x.rhs.get(), derivation_rules::make_bool(),
                            is_mutable_allowed, usage, usage_multiplier);
                    if (not rhs)
                        return std::move(rhs.error());
                    return make_legal_expr(
                        derivation_rules::make_bool(),
                        boost::hana::overload(
                            [&] (boost::hana::type<parser::expr_t::boolean_expr_t::and_t>)
                            {
                                return expr_t::boolean_expr_t{
                                    expr_t::boolean_expr_t::and_t{
                                        std::move(*lhs),
                                        std::move(*rhs)
                                    }};
                            },
                            [&] (boost::hana::type<parser::expr_t::boolean_expr_t::or_t>)
                            {
                                return expr_t::boolean_expr_t{
                                    expr_t::boolean_expr_t::or_t{
                                        std::move(*lhs),
                                        std::move(*rhs)
                                    }};
                            })(boost::hana::type_c<T>));
                });
        },
        [&] (parser::expr_t::relation_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] <typename T> (T const& x) -> expected<expr_t>
                {
                    auto [lhs, rhs] =
                        type_assign_pair(
                            env, ctx, x.lhs.get(), x.rhs.get(), is_mutable_allowed, usage, usage_multiplier);
                    if (lhs and rhs)
                        return make_legal_expr(
                            derivation_rules::make_bool(),
                            boost::hana::overload(
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::eq_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::eq_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                },
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::neq_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::neq_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                },
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::gt_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::gt_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                },
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::gte_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::gte_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                },
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::lt_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::lt_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                },
                                [&] (boost::hana::type<parser::expr_t::relation_expr_t::lte_t>)
                                {
                                    return expr_t::relation_expr_t{
                                        expr_t::relation_expr_t::lte_t{
                                            std::move(*lhs),
                                            std::move(*rhs)}};
                                })(boost::hana::type_c<T>));
                    else if (lhs.has_error() xor rhs.has_error()) // if only 1 error, just forward that one
                        return std::move(lhs.has_error() ? lhs.error() : rhs.error());
                    else if (lhs.error() == rhs.error()) // ...or if the error message is the same
                        return std::move(lhs.error());
                    else
                        return dep0::error_t(
                            "relation expression cannot be assigned a type",
                            loc, {std::move(lhs.error()), std::move(rhs.error())});
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return check_or_assign(env, ctx, x, loc, nullptr, is_mutable_allowed, usage, usage_multiplier);
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            return type_assign_var(env, ctx, x.name, loc, usage, usage_multiplier);
        },
        [&] (parser::expr_t::global_t const& g) -> expected<expr_t>
        {
            return type_assign_global(env, ctx, expr_t::global_t{g.module_name, g.name}, loc, usage_multiplier);
        },
        [&] (parser::expr_t::app_t const& x) -> expected<expr_t>
        {
            return type_assign_app(env, ctx, x, loc, is_mutable_allowed, usage, usage_multiplier);
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            return type_assign_abs(env, ctx, f, loc, std::nullopt, usage, usage_multiplier);
        },
        [&] (parser::expr_t::pi_t const& pi) -> expected<expr_t>
        {
            auto pi_ctx = ctx.extend();
            return check_pi_type(env, pi_ctx, loc, pi.is_mutable, pi.args, pi.ret_type.get());
        },
        [&] (parser::expr_t::sigma_t const& sigma) -> expected<expr_t>
        {
            auto sigma_ctx = ctx.extend();
            return check_sigma_type(env, sigma_ctx, loc, sigma);
        },
        [] (parser::expr_t::ref_t) -> expected<expr_t> { return derivation_rules::make_ref_t(); },
        [] (parser::expr_t::scope_t) -> expected<expr_t> { return derivation_rules::make_scope_t(); },
        [&] (parser::expr_t::addressof_t const& x) -> expected<expr_t>
        {
            auto expr = type_assign(env, ctx, x.expr.get(), is_mutable_allowed, usage, usage_multiplier);
            if (not expr)
                return expr;
            return match(
                is_place_expression(*expr),
                [&] <typename T> (T const&) -> expected<expr_t>
                requires (
                    std::is_same_v<T, is_place_expression_result::no_t> or
                    std::is_same_v<T, is_place_expression_result::var_t>)
                {
                    auto scope = type_assign_scopeof(env, ctx, x.expr.get(), usage, usage_multiplier);
                    if (not scope)
                        return dep0::error_t("cannot take address of expression", loc, {std::move(scope.error())});
                    auto element_type =
                        std::get<expr_t>(
                            std::get<expr_t::scopeof_t>(
                                scope->value).expr.get().properties.sort.get());
                    return derivation_rules::make_addressof(
                        std::move(element_type),
                        std::move(std::get<expr_t::scopeof_t>(scope->value)));
                },
                [&] (is_place_expression_result::deref_t const& deref) -> expected<expr_t>
                {
                    auto ref_type = deref.expr.properties.sort.get(); // take a copy before moving `expr
                    return make_legal_expr(std::move(ref_type), expr_t::addressof_t{std::move(*expr)});
                },
                [&] (auto const&) -> expected<expr_t>
                {
                    return dep0::error_t("place expression not yet handled", loc);
                });
        },
        [&] (parser::expr_t::deref_t const& x) -> expected<expr_t>
        {
            auto ref = type_assign(env, ctx, x.expr.get(), is_mutable_allowed, usage, usage_multiplier);
            if (not ref)
                return ref;
            auto& ref_type = std::get<expr_t>(ref->properties.sort.get());
            beta_delta_normalize(env, ctx, ref_type);
            auto const view = ast::get_if_ref(ref_type);
            if (not view)
            {
                std::ostringstream err;
                pretty_print(err << "cannot dereference non-reference type `", ref_type) << '`';
                return dep0::error_t(err.str(), loc);
            }
            return make_legal_expr(view->element_type.get(), expr_t::deref_t{std::move(*ref)});
        },
        [&] (parser::expr_t::scopeof_t const& x) -> expected<expr_t>
        {
            return type_assign_scopeof(env, ctx, x.expr.get(), usage, usage_multiplier);
        },
        [] (parser::expr_t::array_t) -> expected<expr_t>
        {
            return derivation_rules::make_array();
        },
        [&] (parser::expr_t::init_list_t const& init_list) -> expected<expr_t>
        {
            return dep0::error_t("initializer lists have no unique type", loc);
        },
        [&] (parser::expr_t::member_t const& member) -> expected<expr_t>
        {
            auto obj = type_assign(env, ctx, member.object.get(), is_mutable_allowed, usage, usage_multiplier);
            if (not obj)
                return std::move(obj.error());
            if (auto const ty = std::get_if<expr_t>(&obj->properties.sort.get()))
                if (auto const g = std::get_if<expr_t::global_t>(&ty->value))
                    if (auto const type_def = std::get_if<type_def_t>(env[*g]))
                        if (auto const s = std::get_if<type_def_t::struct_t>(&type_def->value))
                            if (auto const it = ast::find_member_field<properties_t>(member.field, *s);
                                it != s->fields.end())
                            {
                                auto field_type = [&]
                                {
                                    auto fields = std::vector(s->fields.begin(), std::next(it));
                                    for (auto const j: std::views::iota(0, std::distance(s->fields.begin(), it)))
                                        substitute(
                                            fields[j].var,
                                            make_legal_expr(
                                                fields[j].type,
                                                expr_t::member_t{*obj, fields[j].var.name}),
                                            fields.begin() + j + 1,
                                            fields.end());
                                    return std::move(fields.back().type);
                                }();
                                return make_legal_expr(field_type, expr_t::member_t{std::move(*obj), member.field});
                            }
                            else
                            {
                                std::ostringstream err;
                                pretty_print<properties_t>(err << "struct `", *g)
                                    << "` has no field named `" << member.field << '`';
                                return dep0::error_t(err.str(), loc);
                            }
            std::ostringstream err;
            pretty_print(err << "invalid member access on value of type `", obj->properties.sort.get()) << '`';
            return dep0::error_t(err.str(), loc);
        },
        [&] (parser::expr_t::subscript_t const& subscript) -> expected<expr_t>
        {
            // We don't know how to allow the use of the subscript operator on a linear array.
            // Intuitively, one could argue that it should be possible to spend each element only once;
            // but we don't have inference rules to establish that and it might also require
            // keeping track at runtime of which element has been spent.
            // So, for now, we require that the array has quantity of either `zero` or `many`.
            // It should still be possible to allow structured binding of a linear array to extract
            // all its elements at once, provided that the size of the array is known at compile-time.
            // Rust has similar limitations.
            // The exact same reasoning applies also if the object of the subscript operator is a tuple.
            auto obj =
                type_assign(
                    env, ctx, subscript.object.get(), is_mutable_allowed,
                    usage, usage_multiplier * ast::qty_t::many);
            if (not obj)
                return std::move(obj.error());
            return match(
                obj->properties.sort.get(),
                [&] (expr_t& t) -> expected<expr_t>
                {
                    beta_delta_normalize(env, ctx, t);
                    return match(
                        has_subscript_access(t),
                        [&] (has_subscript_access_result::no_t) -> expected<expr_t>
                        {
                            std::ostringstream err;
                            pretty_print(err << "subscript operator not applicable to expression of type `", t) << '`';
                            return dep0::error_t(err.str(), loc);
                        },
                        [&] (has_subscript_access_result::sigma_t const& sigma) -> expected<expr_t>
                        {
                            auto idx =
                                check_expr(
                                    env, ctx,
                                    subscript.index.get(),
                                    derivation_rules::make_u64(),
                                    is_mutable_allowed, usage, usage_multiplier);
                            if (not idx)
                                return std::move(idx.error());
                            auto const constant = std::get_if<expr_t::numeric_constant_t>(&idx->value);
                            if (not constant)
                                return dep0::error_t(
                                    "tuple object can only be accessed via numeric literal",
                                    subscript.index.get().properties);
                            auto const idx_value = constant->value.template convert_to<std::uint64_t>();
                            if (idx_value >= sigma.args.size())
                                return dep0::error_t("invalid tuple index", loc);
                            // the element type may depend on values of previous arguments so need substitution
                            // e.g. the *type* of `x[2]` may depend on the *value* of `x[1]`,
                            // whose type may itself depend on the *value* of `x[0]`
                            auto el_type = [&]
                            {
                                auto args = std::vector(sigma.args.begin(), sigma.args.begin() + idx_value + 1);
                                for (auto const j: std::views::iota(0ul, idx_value))
                                    if (args[j].var)
                                        substitute(
                                            *args[j].var,
                                            derivation_rules::make_subscript(*obj, j, args[j].type),
                                            args.begin() + j + 1,
                                            args.end());
                                return std::move(args[idx_value].type);
                            }();
                            return make_legal_expr(
                                std::move(el_type),
                                expr_t::subscript_t{std::move(*obj), std::move(*idx)});
                        },
                        [&] (has_subscript_access_result::array_t const& array) -> expected<expr_t>
                        {
                            auto idx =
                                check_expr(
                                    env, ctx,
                                    subscript.index.get(),
                                    derivation_rules::make_u64(),
                                    is_mutable_allowed, usage, usage_multiplier);
                            if (not idx)
                                return std::move(idx.error());
                            beta_delta_normalize(env, ctx, *idx);
                            auto const proof_type =
                                derivation_rules::make_true_t(
                                    derivation_rules::make_relation_expr(
                                        expr_t::relation_expr_t::lt_t{*idx, array.size}));
                            if (search_proof(env, ctx, proof_type, ast::is_mutable_t::no, usage, ast::qty_t::zero))
                            {
                                // we're about to move from `obj`, which holds the element type; so must take a copy
                                auto el_type = array.element_type;
                                return make_legal_expr(
                                    std::move(el_type),
                                    expr_t::subscript_t{std::move(*obj), std::move(*idx)});
                            }
                            else
                            {
                                std::ostringstream err;
                                pretty_print(err << "cannot verify that array index `", *idx) << '`';
                                pretty_print(err << " is within bounds `", array.size) << '`';
                                return dep0::error_t(err.str(), loc);
                            }
                        });
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "cannot index into expression of sort `", kind_t{}) << '`';
                    return dep0::error_t(err.str(), loc);
                });
        },
        [&] (parser::expr_t::because_t const& x) -> expected<expr_t>
        {
            // reasons consume zero resources and can contain mutable operations (its type cannot; but the value can)
            auto reason = type_assign(env, ctx, x.reason.get(), ast::is_mutable_t::yes, usage, ast::qty_t::zero);
            if (not reason)
                return std::move(reason.error());
            auto ctx2 = ctx.extend();
            if (auto const reason_type = std::get_if<expr_t>(&reason->properties.sort.get()))
                ctx2.add_unnamed(*reason_type);
            auto value = type_assign(env, ctx2, x.value.get(), is_mutable_allowed, usage, usage_multiplier);
            if (not value)
                return std::move(value.error());
            auto sort = value->properties.sort.get();
            return make_legal_expr(std::move(sort), expr_t::because_t{std::move(*value), std::move(*reason)});
        });
}

expected<expr_t>
type_assign_app(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t::app_t const& app,
    source_loc_t const& loc,
    ast::is_mutable_t const is_mutable_allowed,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto func = type_assign(env, ctx, app.func.get(), is_mutable_allowed, usage, usage_multiplier);
    if (not func)
        return std::move(func.error());
    // We need to ensure that the result type and the type of `func` are both consistent with argument substitutions.
    // We also want to avoid making unnecessary copies so we grab a pointer.
    expr_t::pi_t* const func_type = [&] () -> expr_t::pi_t*
    {
        if (auto* const type = std::get_if<expr_t>(&func->properties.sort.get()))
        {
            beta_delta_normalize(env, ctx, *type);
            return std::get_if<expr_t::pi_t>(&type->value);
        }
        else
            return nullptr;
    }();
    if (not func_type)
    {
        std::ostringstream err;
        pretty_print(err << "cannot invoke expression of type `", func->properties.sort.get()) << '`';
        return dep0::error_t(err.str(), loc);
    }
    if (func_type->args.size() != app.args.size())
    {
        std::ostringstream err;
        err << "passed " << app.args.size() << " arguments but was expecting " << func_type->args.size();
        return dep0::error_t(err.str(), loc);
    }
    if (func_type->is_mutable == ast::is_mutable_t::yes and is_mutable_allowed == ast::is_mutable_t::no)
        return dep0::error_t("cannot invoke mutable function inside immutable context", loc);
    std::vector<expr_t> args;
    for (auto const i: std::views::iota(0ul, func_type->args.size()))
    {
        auto arg =
            check_expr(
                env, ctx,
                app.args[i],
                func_type->args[i].type,
                is_mutable_allowed,
                usage,
                usage_multiplier * func_type->args[i].qty);
        if (not arg)
            return std::move(arg.error());
        if (func_type->args[i].var)
            substitute(
                *func_type->args[i].var,
                *arg,
                func_type->args.begin() + i + 1,
                func_type->args.end(),
                func_type->ret_type.get(),
                nullptr);
        args.push_back(std::move(*arg));
    }
    auto result_type = func_type->ret_type.get(); // we're about to move from `func`, so take a copy
    return make_legal_expr(std::move(result_type), expr_t::app_t{std::move(*func), std::move(args)});
}

expected<expr_t> type_assign_abs(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t::abs_t const& f,
    source_loc_t const& location,
    std::optional<source_text> const& name,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto f_ctx = ctx.extend();
    auto func_type = check_pi_type(env, f_ctx, location, f.is_mutable, f.args, f.ret_type.get());
    if (not func_type)
        return std::move(func_type.error());
    // If a function has a name it can call itself recursively;
    // to typecheck recursive functions we need to add them to the current environment or context,
    // depending on whether they are global or local functions;
    // but we cannot check the body yet, so the best we can do is to add a function declaration;
    // currently we only support global functions, so we add the declaration to an extended environment.
    auto f_env = env.extend();
    if (name)
    {
        auto ok =
            f_env.try_emplace(
                expr_t::global_t{std::nullopt, *name},
                make_legal_func_decl(location, *func_type, *name, std::nullopt, std::get<expr_t::pi_t>(func_type->value)));
        if (not ok)
            return std::move(ok.error());
    }
    auto [is_mutable, arg_types, ret_type] = std::get<expr_t::pi_t>(func_type->value);
    auto body = check_body(f_env, proof_state_t(f_ctx, ret_type.get()), f.body, is_mutable, usage, usage_multiplier);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not returns_from_all_branches(*body))
    {
        if (not is_beta_delta_equivalent(env, f_ctx, ret_type.get(), derivation_rules::make_unit()))
        {
            std::ostringstream err;
            if (name)
                err << "in function `" << *name << "` ";
            err << "missing return statement";
            return dep0::error_t(err.str(), location);
        }
    }
    return make_legal_expr(
        std::move(*func_type),
        expr_t::abs_t{
            is_mutable,
            std::move(arg_types),
            std::move(ret_type),
            std::move(*body)
        });
}

} // namespace dep0::typecheck
