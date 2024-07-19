#include "private/type_assign.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/check.hpp"
#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/returns_from_all_branches.hpp"
#include "private/substitute.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

#include <boost/hana.hpp>

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

/**
 * Assign a type to a pair of expressions that are connected by a binary relation or operator, eg `x+1` or `1 < x`.
 *
 * In this example `1` on its own cannot be assigned a unique type, but if `x` can, then `1` must have the same type.
 *
 * @return The result of each individual type-assignment; either one or both might have either failed or succeded.
 */
static
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
    auto b = a
        ? check_expr(env, ctx, rhs, a->properties.sort.get(), is_mutable_allowed, usage += tmp_usage_a, usage_multiplier)
        : type_assign(env, ctx, rhs, is_mutable_allowed, tmp_usage_b, usage_multiplier);
    if (b and not a)
        a = check_expr(env, ctx, lhs, b->properties.sort.get(), is_mutable_allowed, usage += tmp_usage_b, usage_multiplier);
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
    // common code between var_t and global_t cases
    auto const type_assign_global = [&] (expr_t::global_t global) -> expected<expr_t>
    {
        auto const def = env[global];
        if (not def)
            return error_t::from_error(dep0::error_t("global symbol not found", loc));
        return match(
            *def,
            [&] (type_def_t const&) -> expected<expr_t>
            {
                return make_legal_expr(derivation_rules::make_typename(), std::move(global));
            },
            [&] (axiom_t const& x) -> expected<expr_t>
            {
                if (usage_multiplier > ast::qty_t::zero)
                    return error_t::from_error(dep0::error_t("axiom cannot be used at run-time", loc));
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
    };
    return match(
        expr.value,
        [] (parser::expr_t::typename_t) -> expected<expr_t> { return derivation_rules::make_typename(); },
        [] (parser::expr_t::true_t) -> expected<expr_t> { return derivation_rules::make_true_t(); },
        [&] (parser::expr_t::auto_t) -> expected<expr_t>
        {
            return error_t::from_error(dep0::error_t("auto expressions have no unique type", loc));
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
            return error_t::from_error(dep0::error_t("numeric constants have no unique type", loc));
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
                            },
                            [&] (boost::hana::type<parser::expr_t::boolean_expr_t::xor_t>)
                            {
                                return expr_t::boolean_expr_t{
                                    expr_t::boolean_expr_t::xor_t{
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
                        return error_t::from_error(std::move(lhs.has_error() ? lhs.error() : rhs.error()));
                    else
                        return error_t::from_error(dep0::error_t(
                            "relation expression cannot be assigned a type",
                            loc, {std::move(lhs.error()), std::move(rhs.error())}));
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    auto [lhs, rhs] =
                        type_assign_pair(
                            env, ctx, x.lhs.get(), x.rhs.get(), is_mutable_allowed, usage, usage_multiplier);
                    if (lhs and rhs)
                    {
                        auto type = lhs->properties.sort.get(); // about to move from lhs, take a copy
                        return make_legal_expr(
                            std::move(type),
                            expr_t::arith_expr_t{
                                expr_t::arith_expr_t::plus_t{
                                    std::move(*lhs),
                                    std::move(*rhs)}});
                    }
                    else if (lhs.has_error() xor rhs.has_error()) // if only 1 error, just forward that one
                        return error_t::from_error(std::move(lhs.has_error() ? lhs.error() : rhs.error()));
                    else
                        return error_t::from_error(dep0::error_t(
                            "arithmetic expression cannot be assigned a type",
                            loc, {std::move(lhs.error()), std::move(rhs.error())}));
                });
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            if (auto var = expr_t::var_t{x.name}; auto const expr = ctx[var])
            {
                // if multiplier is zero nothing is really used and type-assignment always succeeds;
                // so don't even bother looking up things and adding them up
                if (usage_multiplier > ast::qty_t::zero)
                {
                    auto const prev = usage[var];
                    if (usage.add(var, ast::qty_t::one * usage_multiplier) > expr->value.qty)
                    {
                        // The user has now used something more than they could;
                        // let's try to give them a sensible error message.
                        // Firstly, this must mean that the context allowed for only either `zero` or `one`.
                        // So there are overall two cases:
                        // 1. if the context allowed for zero uses,
                        //    then they must have attempted to use the thing at run-time;
                        // 2. otherwise the context allowed for only one use and, in that case:
                        //    a. either they tried to pass the variable to a function argument
                        //       with multiplicity `many` (or something similar to that)
                        //    b. or they used the thing more than once.
                        char const* msg =
                            expr->value.qty == ast::qty_t::zero ? "variable cannot be used at run-time"
                            : prev == ast::qty_t::zero ? "cannot use linear variable in non-linear context"
                            : "variable has already been used once";
                        return error_t::from_error(dep0::error_t(msg, loc));
                    }
                }
                return make_legal_expr(expr->value.type, std::move(var));
            }
            if (auto const global = expr_t::global_t{std::nullopt, x.name}; env[global])
                return type_assign_global(global);
            else
                return error_t::from_error(dep0::error_t("unknown variable", loc));
        },
        [&] (parser::expr_t::global_t const& global) -> expected<expr_t>
        {
            return type_assign_global(expr_t::global_t{global.module_name, global.name});
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
        [] (parser::expr_t::array_t) -> expected<expr_t>
        {
            return derivation_rules::make_array();
        },
        [&] (parser::expr_t::init_list_t const& init_list) -> expected<expr_t>
        {
            return error_t::from_error(dep0::error_t("initializer lists have no unique type", loc));
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
            auto array =
                type_assign(
                    env, ctx, subscript.array.get(), is_mutable_allowed,
                    usage, usage_multiplier * ast::qty_t::many);
            if (not array)
                return std::move(array.error());
            return match(
                array->properties.sort.get(),
                [&] (expr_t& t) -> expected<expr_t>
                {
                    beta_delta_normalize(env, ctx, t);
                    auto const app = get_if_app_of_array(t);
                    if (not app)
                    {
                        std::ostringstream err;
                        pretty_print(err << "cannot index into non-array expression of type `", t) << '`';
                        return error_t::from_error(dep0::error_t(err.str(), loc));
                    }
                    auto index =
                        check_expr(
                            env, ctx,
                            subscript.index.get(),
                            derivation_rules::make_u64(),
                            is_mutable_allowed, usage, usage_multiplier);
                    if (not index)
                        return std::move(index.error());
                    beta_delta_normalize(env, ctx, *index);
                    auto const proof_type =
                        derivation_rules::make_true_t(
                            derivation_rules::make_relation_expr(
                                expr_t::relation_expr_t::lt_t{*index, app->args[1]}));
                    if (search_proof(env, ctx, proof_type, is_mutable_allowed, usage, ast::qty_t::zero))
                    {
                        // we're about to move from `array`, which holds the element type; so must take a copy
                        auto element_type = app->args.at(0ul);
                        return make_legal_expr(
                            element_type,
                            expr_t::subscript_t{std::move(*array), std::move(*index)});
                    }
                    else
                    {
                        std::ostringstream err;
                        pretty_print(err << "cannot verify that array index `", *index) << '`';
                        pretty_print(err << " is within bounds `", app->args.at(1ul)) << '`';
                        return error_t::from_error(dep0::error_t(err.str(), loc));
                    }
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "cannot index into expression of sort `", kind_t{}) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc));
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
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc));
    };
    auto func = type_assign(env, ctx, app.func.get(), is_mutable_allowed, usage, usage_multiplier);
    if (not func)
        return std::move(func.error());
    auto func_type = [&] () -> expected<expr_t::pi_t>
    {
        if (auto* const type = std::get_if<expr_t>(&func->properties.sort.get()))
        {
            beta_delta_normalize(env, ctx, *type);
            if (auto const pi = std::get_if<expr_t::pi_t>(&type->value))
                return *pi;
        }
        std::ostringstream err;
        pretty_print(err << "cannot invoke expression of type `", func->properties.sort.get()) << '`';
        return error(err.str());
    }();
    if (not func_type)
        return std::move(func_type.error());
    if (func_type->args.size() != app.args.size())
    {
        std::ostringstream err;
        err << "passed " << app.args.size() << " arguments but was expecting " << func_type->args.size();
        return error(err.str());
    }
    if (func_type->is_mutable == ast::is_mutable_t::yes and is_mutable_allowed == ast::is_mutable_t::no)
        return error("cannot invoke mutable function inside immutable context");
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
    return make_legal_expr(
        std::move(func_type->ret_type.get()),
        expr_t::app_t{std::move(*func), std::move(args)});
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
                make_legal_func_decl(location, *func_type, *name, std::get<expr_t::pi_t>(func_type->value)));
        if (not ok)
            return error_t::from_error(std::move(ok.error()));
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
            return error_t::from_error(dep0::error_t(err.str(), location));
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
