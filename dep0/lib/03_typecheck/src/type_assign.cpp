#include "private/type_assign.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/check.hpp"
#include "private/derivation_rules.hpp"
#include "private/returns_from_all_branches.hpp"

#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

expected<expr_t> type_assign(context_t const& ctx, parser::expr_t const& expr)
{
    auto const loc = expr.properties;
    return match(
        expr.value,
        [&] (parser::expr_t::typename_t) -> expected<expr_t>
        {
            return derivation_rules::make_typename();
        },
        [&] (parser::expr_t::bool_t) -> expected<expr_t>
        {
            return derivation_rules::make_bool();
        },
        [&] (parser::expr_t::unit_t) -> expected<expr_t>
        {
            return derivation_rules::make_unit();
        },
        [&] (parser::expr_t::i8_t) -> expected<expr_t>
        {
            return derivation_rules::make_i8();
        },
        [&] (parser::expr_t::i16_t) -> expected<expr_t>
        {
            return derivation_rules::make_i16();
        },
        [&] (parser::expr_t::i32_t) -> expected<expr_t>
        {
            return derivation_rules::make_i32();
        },
        [&] (parser::expr_t::i64_t) -> expected<expr_t>
        {
            return derivation_rules::make_i64();
        },
        [&] (parser::expr_t::u8_t) -> expected<expr_t>
        {
            return derivation_rules::make_u8();
        },
        [&] (parser::expr_t::u16_t) -> expected<expr_t>
        {
            return derivation_rules::make_u16();
        },
        [&] (parser::expr_t::u32_t) -> expected<expr_t>
        {
            return derivation_rules::make_u32();
        },
        [&] (parser::expr_t::u64_t) -> expected<expr_t>
        {
            return derivation_rules::make_u64();
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            return make_legal_expr(derivation_rules::make_bool(), expr_t::boolean_constant_t{x.value});
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            // One might argue that it is possible to type assign a numerical constant if,
            // in the current context, there is only 1 viable type in which the numerical constant would fit,
            // eg max of u64, provided the user did not define their own 64 bits integer;
            // but having this kind of context-sensitivity is also a bit suprising, so not really great;
            // therefore the decision is to always fail type-assignment of numerical constants;
            std::ostringstream err;
            err << "cannot assign a unique type to numeric constant without being context-sensitive";
            return error_t::from_error(dep0::error_t(err.str(), loc));
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    // Make sure we can assign a type to expressions like `x+1` or `1+x`,
                    // based on the type of `x`, even though `1` itself cannot be assigned a type.
                    if (auto lhs = type_assign(ctx, x.lhs.get()))
                    {
                        if (auto rhs = check_expr(ctx, x.rhs.get(), lhs->properties.sort.get()))
                            return make_legal_expr(
                                lhs->properties.sort.get(),
                                expr_t::arith_expr_t{
                                    expr_t::arith_expr_t::plus_t{
                                        std::move(*lhs),
                                        std::move(*rhs)}});
                        else
                            return std::move(rhs.error());
                    }
                    else if (auto rhs = type_assign(ctx, x.rhs.get()))
                    {
                        if (auto lhs = check_expr(ctx, x.lhs.get(), rhs->properties.sort.get()))
                            return make_legal_expr(
                                rhs->properties.sort.get(),
                                expr_t::arith_expr_t{
                                    expr_t::arith_expr_t::plus_t{
                                        std::move(*lhs),
                                        std::move(*rhs)}});
                        else
                            return std::move(lhs.error());
                    }
                    else
                    {
                        std::ostringstream err;
                        err << "cannot assign a unique type to arithmetic expression";
                        return error_t::from_error(dep0::error_t(
                            err.str(),
                            loc,
                            std::vector<dep0::error_t>{
                                std::move(lhs.error()),
                                std::move(rhs.error())
                            }));
                    }
                });
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto const var = expr_t::var_t{x.name};
            auto const val = ctx[var];
            if (not val)
            {
                std::ostringstream err;
                pretty_print<properties_t>(err << "unknown variable `", var) << '`';
                return error_t::from_error(dep0::error_t(err.str(), loc));
            }
            return make_legal_expr(
                match(
                    val->value,
                    [&] (type_def_t const&) -> sort_t { return derivation_rules::make_typename(); },
                    [&] (expr_t const& expr) -> sort_t { return expr.properties.sort.get(); }),
                var);
        },
        [&] (parser::expr_t::app_t const& x) -> expected<expr_t>
        {
            return type_assign_app(ctx, x, loc);
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            return type_assign_abs(ctx, f, loc, std::nullopt);
        },
        [&] (parser::expr_t::pi_t const& pi) -> expected<expr_t>
        {
            auto pi_ctx = ctx.extend();
            return check_pi_type(pi_ctx, pi.args, pi.ret_type.get());
        },
        [&] (parser::expr_t::array_t const&) -> expected<expr_t>
        {
            // the elemnt type of the array must be of sort types,
            // i.e. a "function argument" of type `typename`;
            // to have array of types (eg `{int, bool, bool}`),
            // we fisrt need to add cumulativity of types;
            // i.e. we can have an array of ints, eg {1,2,3},
            // but not an array of types, eg {int, bool, bool};
            // with cumulativity we can have both
            return make_legal_expr(
                make_legal_expr(
                    kind_t{}, // TODO need to add a test to make sure this is correct
                    expr_t::pi_t{
                        std::vector{
                            make_legal_func_arg(derivation_rules::make_typename()),
                            make_legal_func_arg(derivation_rules::make_u64())
                        },
                        derivation_rules::make_typename()}),
                expr_t::array_t{});
        },
        [&] (parser::expr_t::init_list_t const& init_list) -> expected<expr_t>
        {
            std::ostringstream err;
            err << "cannot assign a unique type to an initializer list expression";
            return error_t::from_error(dep0::error_t(err.str(), loc));
        },
        [&] (parser::expr_t::subscript_t const& subscript) -> expected<expr_t>
        {
            auto array = type_assign(ctx, subscript.array.get());
            if (not array)
                return std::move(array.error());
            return match(
                array->properties.sort.get(),
                [&] (expr_t& t) -> expected<expr_t>
                {
                    ast::beta_delta_normalize(ctx.delta_reduction_context(), t);
                    auto const app = get_if_app_of_array(t);
                    if (not app)
                    {
                        std::ostringstream err;
                        pretty_print(err << "cannot index into non-array expression of type `", t) << '`';
                        return error_t::from_error(dep0::error_t(err.str(), loc));
                    }
                    auto index = check_expr(ctx, subscript.index.get(), derivation_rules::make_u64());
                    if (not index)
                        return std::move(index.error());
                    ast::beta_delta_normalize(ctx.delta_reduction_context(), *index);
                    if (auto const i = std::get_if<expr_t::numeric_constant_t>(&index->value))
                        if (auto const n = std::get_if<expr_t::numeric_constant_t>(&app->args.at(1ul).value))
                            if (i->value < n->value)
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
                                pretty_print<properties_t>(err << "array index `", *i) << '`';
                                pretty_print<properties_t>(err << " is not within bounds `", *n) << '`';
                                return error_t::from_error(dep0::error_t(err.str(), loc));
                            }
                    // TODO either index or size (or both) are run-time values; we need to add proof-search for `i < n`
                    std::ostringstream err;
                    pretty_print(err << "cannot verify that array index `", *index) << '`';
                    pretty_print(err << " is within bounds `", app->args.at(1ul)) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc));
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "cannot index into expression of sort `", kind_t{}) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc));
                });
        });
}

expected<expr_t> type_assign_app(context_t const& ctx, parser::expr_t::app_t const& app, source_loc_t const& loc)
{
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc));
    };
    auto func = type_assign(ctx, app.func.get());
    if (not func)
        return std::move(func.error());
    auto func_type = [&] () -> expected<expr_t::pi_t>
    {
        if (auto* const type = std::get_if<expr_t>(&func->properties.sort.get()))
        {
            ast::beta_delta_normalize(ctx.delta_reduction_context(), *type);
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
        auto arg = check_expr(ctx, app.args[i], func_type->args[i].type);
        if (not arg)
            return std::move(arg.error());
        if (func_type->args[i].var)
            substitute<properties_t>(
                *func_type->args[i].var,
                *arg,
                func_type->args.begin() + i + 1,
                func_type->args.end(),
                func_type->ret_type.get(),
                nullptr);
        args.push_back(std::move(*arg));
    }
    return make_legal_expr(std::move(func_type->ret_type.get()), expr_t::app_t{std::move(*func), std::move(args)});
}

expected<expr_t> type_assign_abs(
    context_t const& ctx,
    parser::expr_t::abs_t const& f,
    source_loc_t const& location,
    std::optional<source_text> const& name)
{
    auto f_ctx = ctx.extend();
    auto func_type = check_pi_type(f_ctx, f.args, f.ret_type.get());
    if (not func_type)
        return std::move(func_type.error());
    // if a function has a name it can call itself recursively;
    // to typecheck recursive functions we need to add them to the current function context before checking the body
    if (name)
    {
        auto const func_name = expr_t::var_t{*name};
        if (auto ok = f_ctx.try_emplace(func_name, location, make_legal_expr(*func_type, func_name)); not ok)
            return error_t::from_error(std::move(ok.error()));
    }
    auto [arg_types, ret_type] = std::get<expr_t::pi_t>(func_type->value);
    auto body = check_body(proof_state_t(f_ctx, ret_type.get()), f.body);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not returns_from_all_branches(*body))
    {
        if (not is_beta_delta_equivalent(f_ctx, ret_type.get(), derivation_rules::make_unit()))
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
