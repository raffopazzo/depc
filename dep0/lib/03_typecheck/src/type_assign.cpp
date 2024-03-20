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
    environment_t const& env,
    context_t const& ctx,
    parser::expr_t const& lhs,
    parser::expr_t const& rhs)
{
    auto a = type_assign(env, ctx, lhs);
    auto b = a
        ? check_expr(env, ctx, rhs, a->properties.sort.get())
        : type_assign(env, ctx, rhs);
    if (b and not a)
        a = check_expr(env, ctx, lhs, b->properties.sort.get());
    return std::pair{std::move(a), std::move(b)};
}

expected<expr_t> type_assign(environment_t const& env, context_t const& ctx, parser::expr_t const& expr)
{
    auto const loc = expr.properties;
    return match(
        expr.value,
        [] (parser::expr_t::typename_t) -> expected<expr_t> { return derivation_rules::make_typename(); },
        [] (parser::expr_t::true_t) -> expected<expr_t> { return derivation_rules::make_true_t(); },
        [] (parser::expr_t::bool_t) -> expected<expr_t> { return derivation_rules::make_bool(); },
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
            std::ostringstream err;
            err << "cannot assign a unique type to numeric constant without being context-sensitive";
            return error_t::from_error(dep0::error_t(err.str(), loc));
        },
        [&] (parser::expr_t::boolean_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::boolean_expr_t::not_t const& x) -> expected<expr_t>
                {
                    if (auto expr = check_expr(env, ctx, x.expr.get(), derivation_rules::make_bool()))
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
                    auto lhs = check_expr(env, ctx, x.lhs.get(), derivation_rules::make_bool());
                    if (not lhs)
                        return std::move(lhs.error());
                    auto rhs = check_expr(env, ctx, x.rhs.get(), derivation_rules::make_bool());
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
                    auto [lhs, rhs] = type_assign_pair(env, ctx, x.lhs.get(), x.rhs.get());
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
                    else
                    {
                        std::ostringstream err;
                        err << "cannot assign a unique type to relation expression";
                        std::vector<dep0::error_t> reasons;
                        if (lhs.has_error()) reasons.push_back(std::move(lhs.error()));
                        if (rhs.has_error()) reasons.push_back(std::move(rhs.error()));
                        return error_t::from_error(dep0::error_t(err.str(), loc, std::move(reasons)));
                    }
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    auto [lhs, rhs] = type_assign_pair(env, ctx, x.lhs.get(), x.rhs.get());
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
                    else
                    {
                        std::ostringstream err;
                        err << "cannot assign a unique type to arithmetic expression";
                        std::vector<dep0::error_t> reasons;
                        if (lhs.has_error()) reasons.push_back(std::move(lhs.error()));
                        if (rhs.has_error()) reasons.push_back(std::move(rhs.error()));
                        return error_t::from_error(dep0::error_t(err.str(), loc, std::move(reasons)));
                    }
                });
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            {
                auto var = expr_t::var_t{x.name};
                if (auto const expr = ctx[var])
                    return make_legal_expr(expr->value.properties.sort.get(), std::move(var));
            }
            {
                auto global = expr_t::global_t{x.name};
                if (auto const def = env[global])
                    return make_legal_expr(
                        match(
                            *def,
                            [] (type_def_t const&) -> sort_t { return derivation_rules::make_typename(); },
                            [] (func_decl_t const& x) -> sort_t { return x.properties.sort.get(); },
                            [] (func_def_t const& x) -> sort_t { return x.properties.sort.get(); }),
                        std::move(global));
            }
            return error_t::from_error(dep0::error_t("unknown variable", loc));
        },
        [&] (parser::expr_t::global_t const& x) -> expected<expr_t>
        {
            std::ostringstream err;
            err << "unexpected global name from parser: " << x.name;
            return error_t::from_error(dep0::error_t(err.str(), loc));
        },
        [&] (parser::expr_t::app_t const& x) -> expected<expr_t>
        {
            return type_assign_app(env, ctx, x, loc);
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            return type_assign_abs(env, ctx, f, loc, std::nullopt);
        },
        [&] (parser::expr_t::pi_t const& pi) -> expected<expr_t>
        {
            auto pi_ctx = ctx.extend();
            return check_pi_type(env, pi_ctx, loc, pi.args, pi.ret_type.get());
        },
        [] (parser::expr_t::array_t) -> expected<expr_t>
        {
            return derivation_rules::make_array();
        },
        [&] (parser::expr_t::init_list_t const& init_list) -> expected<expr_t>
        {
            std::ostringstream err;
            err << "cannot assign a unique type to an initializer list expression";
            return error_t::from_error(dep0::error_t(err.str(), loc));
        },
        [&] (parser::expr_t::subscript_t const& subscript) -> expected<expr_t>
        {
            auto array = type_assign(env, ctx, subscript.array.get());
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
                    auto index = check_expr(env, ctx, subscript.index.get(), derivation_rules::make_u64());
                    if (not index)
                        return std::move(index.error());
                    beta_delta_normalize(env, ctx, *index);
                    auto const proof_type =
                        make_legal_expr(
                            derivation_rules::make_typename(),
                            expr_t::app_t{
                                derivation_rules::make_true_t(),
                                {
                                    make_legal_expr(
                                        derivation_rules::make_bool(),
                                        expr_t::relation_expr_t{
                                            expr_t::relation_expr_t::lt_t{*index, app->args[1]}
                                        })
                                }});
                    if (proof_search(env, ctx, proof_type))
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
        });
}

expected<expr_t>
type_assign_app(
    environment_t const& env,
    context_t const& ctx,
    parser::expr_t::app_t const& app,
    source_loc_t const& loc)
{
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc));
    };
    auto func = type_assign(env, ctx, app.func.get());
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
    std::vector<expr_t> args;
    for (auto const i: std::views::iota(0ul, func_type->args.size()))
    {
        auto arg = check_expr(env, ctx, app.args[i], func_type->args[i].type);
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
    environment_t const& env,
    context_t const& ctx,
    parser::expr_t::abs_t const& f,
    source_loc_t const& location,
    std::optional<source_text> const& name)
{
    auto f_ctx = ctx.extend();
    auto func_type = check_pi_type(env, f_ctx, location, f.args, f.ret_type.get());
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
                expr_t::global_t{*name},
                make_legal_func_decl(location, *func_type, *name, std::get<expr_t::pi_t>(func_type->value)));
        if (not ok)
            return error_t::from_error(std::move(ok.error()));
    }
    auto [arg_types, ret_type] = std::get<expr_t::pi_t>(func_type->value);
    auto body = check_body(f_env, proof_state_t(f_ctx, ret_type.get()), f.body);
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
            std::move(arg_types),
            std::move(ret_type),
            std::move(*body)
        });
}

} // namespace dep0::typecheck
