#include "dep0/typecheck/check.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/check.hpp"
#include "private/cpp_int_limits.hpp"
#include "private/derivation_rules.hpp"
#include "private/returns_from_all_branches.hpp"
#include "private/type_assign.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

expected<module_t> check(parser::module_t const& x) noexcept
{
    environment_t env;
    std::vector<expr_t::global_t> decls; // we must check that all function declarations have been defined
    auto entries =
        fmap_or_error(
            x.entries,
            [&] (parser::module_t::entry_t const& v)
            {
                using entry_t = typecheck::module_t::entry_t;
                return match(
                    v,
                    [&] (parser::type_def_t const& t) -> expected<entry_t> { return check_type_def(env, t); },
                    [&] (parser::func_decl_t const& x) -> expected<entry_t>
                    {
                        auto const& result = check_func_decl(env, x);
                        if (result)
                            decls.push_back(expr_t::global_t{x.name});
                        return result;
                    },
                    [&] (parser::func_def_t const& f) -> expected<entry_t> { return check_func_def(env, f); });
            });
    if (not entries)
        return std::move(entries.error());
    // we now must check that all function declarations have been defined:
    // remove all names that have been defined and if there is any leftover type-checking must fail
    auto const new_end =
        std::remove_if(
            decls.begin(), decls.end(),
            [&] (expr_t::global_t const& name)
            {
                auto const* const prev = env[name];
                return prev and std::holds_alternative<func_def_t>(*prev);
            });
    if (decls.begin() != new_end)
    {
        std::vector<dep0::error_t> reasons;
        for (auto const& name: std::ranges::subrange(decls.begin(), new_end))
        {
            auto const* const p = env[name];
            assert(p);
            auto const loc = match(*p, [] (auto const& x) { return x.properties.origin; });
            reasons.push_back(dep0::error_t("function has been declared but not defined", loc));
        }
        return error_t::from_error(dep0::error_t("module is not valid", std::nullopt, std::move(reasons)));
    }
    return make_legal_module(std::move(*entries));
}

// implementation of private functions

expected<type_def_t> check_type_def(environment_t& env, parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [&] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            if (x.max_abs_value and *x.max_abs_value > cpp_int_max(x.sign, x.width))
                return error_t::from_error(
                    dep0::error_t(
                        "upper/lower bounds do not fit inside bit width",
                        type_def.properties));
            auto const result =
                make_legal_type_def(
                    type_def.properties,
                    type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value});
            if (auto ok = env.try_emplace(expr_t::global_t{x.name}, result); not ok)
                return error_t::from_error(std::move(ok.error()));
            return result;
        });
}

expected<func_decl_t> check_func_decl(environment_t& env, parser::func_decl_t const& decl)
{
    context_t ctx;
    auto pi_type = check_pi_type(env, ctx, decl.properties, decl.signature.args, decl.signature.ret_type.get());
    if (not pi_type)
        return std::move(pi_type.error());
    auto result = make_legal_func_decl(decl.properties, *pi_type, decl.name, std::get<expr_t::pi_t>(pi_type->value));
    if (auto ok = env.try_emplace(expr_t::global_t{decl.name}, result); not ok)
        return error_t::from_error(std::move(ok.error()));
    return result;
}

expected<func_def_t> check_func_def(environment_t& env, parser::func_def_t const& f)
{
    context_t ctx;
    auto abs = type_assign_abs(env, ctx, f.value, f.properties, f.name);
    if (not abs)
        return std::move(abs.error());
    auto result =
        make_legal_func_def(
            f.properties,
            abs->properties.sort.get(),
            f.name,
            std::move(std::get<expr_t::abs_t>(abs->value)));
    if (auto ok = env.try_emplace(expr_t::global_t{f.name}, result); not ok)
        return error_t::from_error(std::move(ok.error()));
    return result;
}

expected<expr_t> check_type(environment_t const& env, context_t const& ctx, parser::expr_t const& type)
{
    // TODO try move to a "traditional" 2-step approach: type-assign first and then compare to the expected type;
    // numerical constants might get in the way, if so could maybe pass a "tie-breaker"?
    auto as_type = check_expr(env, ctx, type, derivation_rules::make_typename());
    if (as_type)
        return as_type;
    auto as_kind = check_expr(env, ctx, type, kind_t{});
    if (as_kind)
        return as_kind;
    return error_t::from_error(
        dep0::error_t(
            "expression cannot be assigned to neither types nor kinds",
            type.properties,
            as_type.error() == as_kind.error() // please don't print stupid duplicate error messages...
                ? std::vector<dep0::error_t>{std::move(as_type.error())}
                : std::vector<dep0::error_t>{std::move(as_type.error()), std::move(as_kind.error())}));
}

expected<body_t> check_body(environment_t const& env, proof_state_t state, parser::body_t const& x)
{
    if (auto stmts = fmap_or_error(x.stmts, [&] (parser::stmt_t const& s) { return check_stmt(env, state, s); }))
        return make_legal_body(std::move(*stmts));
    else
        return std::move(stmts.error());
}

expected<stmt_t> check_stmt(environment_t const& env, proof_state_t& state, parser::stmt_t const& s)
{
    return match(
        s.value,
        [&] (parser::expr_t::app_t const& x) -> expected<stmt_t>
        {
            if (auto app = type_assign_app(env, state.context, x, s.properties))
                return make_legal_stmt(std::move(std::get<expr_t::app_t>(app->value)));
            else
                return std::move(app.error());
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            auto cond = check_expr(env, state.context, x.cond, derivation_rules::make_bool());
            if (not cond)
                return std::move(cond.error());
            auto true_branch = [&]
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_true());
                return check_body(env, std::move(new_state), x.true_branch);
            }();
            if (not true_branch)
                return std::move(true_branch.error());
            if (x.false_branch)
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_false());
                if (auto false_branch = check_body(env, std::move(new_state), *x.false_branch))
                    return make_legal_stmt(
                        stmt_t::if_else_t{
                            std::move(*cond),
                            std::move(*true_branch),
                            std::move(*false_branch)
                        });
                else
                    return std::move(false_branch.error());
            }
            // we are dealing with an if-else without an explicit else branch;
            // but if the true branch returns from all its sub-branches,
            // then it means we are now in the implied else branch;
            // we can then rewrite `cond = false` inside the current proof state
            if (returns_from_all_branches(*true_branch))
                state.rewrite(*cond, derivation_rules::make_false());
            return make_legal_stmt(
                stmt_t::if_else_t{
                    std::move(*cond),
                    std::move(*true_branch),
                    std::nullopt
                });
        },
        [&] (parser::stmt_t::return_t const& x) -> expected<stmt_t>
        {
            if (not x.expr)
            {
                if (is_beta_delta_equivalent(env, state.context, state.goal, derivation_rules::make_unit()))
                    return make_legal_stmt(stmt_t::return_t{});
                else
                {
                    std::ostringstream err;
                    pretty_print(err << "expecting expression of type `", state.goal) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), s.properties), env, state.context, state.goal);
                }
            }
            else if (auto expr = check_expr(env, state.context, *x.expr, state.goal))
                return make_legal_stmt(stmt_t::return_t{std::move(*expr)});
            else
                return std::move(expr.error());
        });
}

expected<expr_t>
check_numeric_expr(
    environment_t const& env,
    context_t const& ctx,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    expr_t const& expected_type)
{
    auto const error = [&] (std::string msg) -> expected<expr_t>
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc), env, ctx, expected_type);
    };
    auto const check_int = [&] (
        std::string_view const type_name,
        boost::multiprecision::cpp_int const& min_value,
        boost::multiprecision::cpp_int const& max_value
    ) -> expected<expr_t>
    {
        if (x.value < min_value or max_value < x.value)
        {
            std::ostringstream err;
            err << "numeric constant does not fit inside `" << type_name << '`';
            return error(err.str());
        }
        return make_legal_expr(expected_type, expr_t::numeric_constant_t{x.value});
    };
    return match(
        expected_type.value,
        [&] (expr_t::i8_t  const&) { return check_int("i8_t",  cpp_int_min_signed<8>(),  cpp_int_max_signed<8>()); },
        [&] (expr_t::i16_t const&) { return check_int("i16_t", cpp_int_min_signed<16>(), cpp_int_max_signed<16>()); },
        [&] (expr_t::i32_t const&) { return check_int("i32_t", cpp_int_min_signed<32>(), cpp_int_max_signed<32>()); },
        [&] (expr_t::i64_t const&) { return check_int("i64_t", cpp_int_min_signed<64>(), cpp_int_max_signed<64>()); },
        [&] (expr_t::u8_t  const&) { return check_int("u8_t",  0, cpp_int_max_unsigned<8>()); },
        [&] (expr_t::u16_t const&) { return check_int("u16_t", 0, cpp_int_max_unsigned<16>()); },
        [&] (expr_t::u32_t const&) { return check_int("u32_t", 0, cpp_int_max_unsigned<32>()); },
        [&] (expr_t::u64_t const&) { return check_int("u64_t", 0, cpp_int_max_unsigned<64>()); },
        [&] (expr_t::var_t const& var) -> expected<expr_t>
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << "type mismatch between numeric constant and `", var) << '`';
            return error(err.str());
        },
        [&] (expr_t::global_t const& global) -> expected<expr_t>
        {
            auto const def = env[global];
            assert(def and "unknown type variable despite typecheck succeeded for the expected type");
            return match(
                *def,
                [&] (type_def_t const& t) -> expected<expr_t>
                {
                    return match(
                        t.value,
                        [&] (type_def_t::integer_t const& integer) -> expected<expr_t>
                        {
                            auto const& [name, sign, width, max_abs_value] = integer;
                            if (sign == ast::sign_t::signed_v)
                                return max_abs_value
                                    ? check_int(name, -*max_abs_value, *max_abs_value)
                                    : check_int(name, cpp_int_min_signed(width), cpp_int_max_signed(width));
                            else
                                return max_abs_value
                                    ? check_int(name, 0ul, *max_abs_value)
                                    : check_int(name, 0ul, cpp_int_max_unsigned(width));
                        });
                },
                [&] (func_decl_t const& func_decl) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << func_decl.name << '`';
                    return error(err.str());
                },
                [&] (func_def_t const& func_def) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << func_def.name << '`';
                    return error(err.str());
                });
        },
        [&] (auto const& x)
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << "type mismatch between numeric constant and `", x) << '`';
            return error(err.str());
        });
}

expected<expr_t>
check_expr(
    environment_t const& env,
    context_t const& ctx,
    parser::expr_t const& x,
    sort_t const& expected_type)
{
    auto const loc = x.properties;
    auto const type_error = [&] (sort_t const& actual_type, dep0::error_t reason)
    {
        std::ostringstream err;
        pretty_print(err << "type mismatch between expression of type `", actual_type) << '`';
        pretty_print(err << " and expected type `", expected_type) << '`';
        return error_t::from_error(
            dep0::error_t(err.str(), loc, std::vector{std::move(reason)}),
            env, ctx, expected_type);
    };
    return match(
        x.value,
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            // numeric constant cannot be type-assigned without an expected type
            return match(
                expected_type,
                [&] (expr_t expected_type) -> expected<expr_t>
                {
                    beta_delta_normalize(env, ctx, expected_type);
                    return check_numeric_expr(env, ctx, x, loc, expected_type);
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between numeric constant and `", kind_t{}) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc), env, ctx, expected_type);
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            // an arithmetic expression like `1+2` cannot be type-assigned without an expected type
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    auto lhs = check_expr(env, ctx, x.lhs.get(), expected_type);
                    if (not lhs) return std::move(lhs.error());
                    auto rhs = check_expr(env, ctx, x.rhs.get(), expected_type);
                    if (not rhs) return std::move(rhs.error());
                    return make_legal_expr(
                        expected_type,
                        expr_t::arith_expr_t{
                            expr_t::arith_expr_t::plus_t{
                                std::move(*lhs),
                                std::move(*rhs)}});
                });
        },
        [&] (parser::expr_t::init_list_t const& init_list) -> expected<expr_t>
        {
            // initializer lists cannot be type-assigned without an expected type (which for now can only be an array)
            return match(
                expected_type,
                [&] (expr_t expected_type) -> expected<expr_t>
                {
                    beta_delta_normalize(env, ctx, expected_type);
                    auto const app = get_if_app_of_array(expected_type);
                    if (not app)
                    {
                        std::ostringstream err;
                        pretty_print(err << "type mismatch between initializer list and `", expected_type) << '`';
                        return error_t::from_error(dep0::error_t(err.str(), loc), env, ctx, expected_type);
                    }
                    auto const n = std::get_if<expr_t::numeric_constant_t>(&app->args.at(1ul).value);
                    if (not n)
                    {
                        std::ostringstream err;
                        pretty_print(err << "type mismatch between initializer list and `", expected_type) << '`';
                        return error_t::from_error(dep0::error_t(err.str(), loc), env, ctx, expected_type);
                    }
                    if (n->value != init_list.values.size())
                    {
                        std::ostringstream err;
                        err << "initializer list has " << init_list.values.size() << " elements but was expecting ";
                        pretty_print<properties_t>(err, *n);
                        return error_t::from_error(dep0::error_t(err.str(), loc), env, ctx, expected_type);
                    }
                    auto values =
                        fmap_or_error(
                            init_list.values,
                            [&] (parser::expr_t const& v)
                            {
                                return check_expr(env, ctx, v, app->args.at(0ul));
                            });
                    if (not values)
                        return std::move(values.error());
                    return make_legal_expr(expected_type, expr_t::init_list_t{std::move(*values)});
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between initializer list and `", kind_t{}) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc), env, ctx, expected_type);
                });
        },
        [&] (auto const&) -> expected<expr_t>
        {
            // for all other cases, we can type-assign and check that the assigned type is what we expect
            auto result = type_assign(env, ctx, x);
            if (result)
                if (auto eq = is_beta_delta_equivalent(env, ctx, result->properties.sort.get(), expected_type); not eq)
                    return type_error(result->properties.sort.get(), std::move(eq.error()));
            return result;
        });
}

expected<expr_t> check_pi_type(
    environment_t const& env,
    context_t& ctx,
    source_loc_t const& loc,
    std::vector<parser::func_arg_t> const& parser_args,
    parser::expr_t const& parser_ret_type)
{
    auto args = fmap_or_error(
        parser_args,
        [&, next_arg_index=0] (parser::func_arg_t const& arg) mutable -> expected<func_arg_t>
        {
            auto const arg_index = next_arg_index++;
            auto const arg_loc = arg.properties;
            auto var = arg.var ? std::optional{expr_t::var_t{arg.var->name}} : std::nullopt;
            auto type = check_type(env, ctx, arg.type);
            if (not type)
            {
                std::ostringstream err;
                if (var)
                    pretty_print<properties_t>(err << "cannot typecheck function argument `", *var) << '`';
                else
                    err << "cannot typecheck function argument at index " << arg_index;
                return error_t::from_error(dep0::error_t(err.str(), loc, {std::move(type.error())}));
            }
            if (var)
                if (auto ok = ctx.try_emplace(*var, arg_loc, make_legal_expr(*type, *var)); not ok)
                    return error_t::from_error(std::move(ok.error()));
            return make_legal_func_arg(std::move(*type), std::move(var));
        });
    if (not args)
        return std::move(args.error());
    auto const ret_type = check_type(env, ctx, parser_ret_type);
    if (not ret_type)
        return error_t::from_error(
            dep0::error_t("cannot typecheck function return type", loc, {std::move(ret_type.error())}));
    return make_legal_expr(ret_type->properties.sort.get(), expr_t::pi_t{std::move(*args), *ret_type});
}

} // namespace dep0::typecheck
