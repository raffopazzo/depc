#include "dep0/typecheck/check.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/check.hpp"
#include "private/cpp_int_limits.hpp"
#include "private/c_types.hpp"
#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/returns_from_all_branches.hpp"
#include "private/type_assign.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"
#include "dep0/typecheck/list_initialization.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <boost/hana.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

expected<module_t> check(env_t const& base_env, parser::module_t const& x) noexcept
{
    auto env = base_env.extend();
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
                    [&] (parser::axiom_t const& x) -> expected<entry_t> { return check_axiom(env, x); },
                    [&] (parser::extern_decl_t const& x) -> expected<entry_t> { return check_extern_decl(env, x); },
                    [&] (parser::func_decl_t const& x) -> expected<entry_t>
                    {
                        auto const& result = check_func_decl(env, x);
                        if (result)
                            decls.push_back(expr_t::global_t{std::nullopt, x.name});
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
        return error_t("module is not valid", std::nullopt, std::move(reasons));
    }
    return make_legal_module(std::move(*entries));
}

// implementation of private functions

expected<type_def_t> check_type_def(env_t& env, parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [&] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            auto const result =
                make_legal_type_def(
                    type_def.properties,
                    type_def_t::integer_t{x.name, x.sign, x.width});
            if (auto ok = env.try_emplace(expr_t::global_t{std::nullopt, x.name}, result); not ok)
                return std::move(ok.error());
            return result;
        });
}

expected<axiom_t> check_axiom(env_t& env, parser::axiom_t const& axiom)
{
    assert(axiom.signature.is_mutable == ast::is_mutable_t::no and "invalid axiom from parser");
    ctx_t ctx;
    auto pi_type =
        check_pi_type(
            env, ctx, axiom.properties,
            axiom.signature.is_mutable,
            axiom.signature.args,
            axiom.signature.ret_type.get());
    if (not pi_type)
        return std::move(pi_type.error());
    auto result = make_legal_axiom(axiom.properties, *pi_type, axiom.name, std::get<expr_t::pi_t>(pi_type->value));
    if (auto ok = env.try_emplace(expr_t::global_t{std::nullopt, axiom.name}, result); not ok)
        return std::move(ok.error());
    return result;
}

expected<extern_decl_t> check_extern_decl(env_t& env, parser::extern_decl_t const& decl)
{
    ctx_t ctx;
    if (auto ok = is_c_func_type(decl.signature, decl.properties); not ok)
        return std::move(ok.error());
    auto pi_type =
        check_pi_type(
            env, ctx, decl.properties,
            decl.signature.is_mutable,
            decl.signature.args,
            decl.signature.ret_type.get());
    if (not pi_type)
        return std::move(pi_type.error());
    auto result = make_legal_extern_decl(decl.properties, *pi_type, decl.name, std::get<expr_t::pi_t>(pi_type->value));
    if (auto ok = env.try_emplace(expr_t::global_t{std::nullopt, decl.name}, result); not ok)
        return std::move(ok.error());
    return result;
}

expected<func_decl_t> check_func_decl(env_t& env, parser::func_decl_t const& decl)
{
    ctx_t ctx;
    auto pi_type =
        check_pi_type(
            env, ctx, decl.properties,
            decl.signature.is_mutable,
            decl.signature.args,
            decl.signature.ret_type.get());
    if (not pi_type)
        return std::move(pi_type.error());
    auto result = make_legal_func_decl(decl.properties, *pi_type, decl.name, std::get<expr_t::pi_t>(pi_type->value));
    if (auto ok = env.try_emplace(expr_t::global_t{std::nullopt, decl.name}, result); not ok)
        return std::move(ok.error());
    return result;
}

expected<func_def_t> check_func_def(env_t& env, parser::func_def_t const& f)
{
    ctx_t ctx;
    usage_t usage;
    auto abs = type_assign_abs(env, ctx, f.value, f.properties, f.name, usage, ast::qty_t::one);
    if (not abs)
        return std::move(abs.error());
    auto result =
        make_legal_func_def(
            f.properties,
            abs->properties.sort.get(),
            f.name,
            std::move(std::get<expr_t::abs_t>(abs->value)));
    if (auto ok = env.try_emplace(expr_t::global_t{std::nullopt, f.name}, result); not ok)
        return std::move(ok.error());
    return result;
}

expected<expr_t> check_type(env_t const& env, ctx_t const& ctx, parser::expr_t const& type)
{
    // TODO try move to a "traditional" 2-step approach: type-assign first and then compare to the expected type;
    // numerical constants might get in the way, if so could maybe pass a "tie-breaker"?
    usage_t usage; // when checking types an empty usage_t works just as fine, because the multiplier is zero anyway
    auto const immutable = ast::is_mutable_t::no;
    auto as_type = check_expr(env, ctx, type, derivation_rules::make_typename(), immutable, usage, ast::qty_t::zero);
    if (as_type)
        return as_type;
    auto as_kind = check_expr(env, ctx, type, kind_t{}, immutable, usage, ast::qty_t::zero);
    if (as_kind)
        return as_kind;
    return error_t(
        "expression is neither a type nor a kind",
        type.properties,
        as_type.error() == as_kind.error() // please don't print stupid duplicate error messages...
            ? std::vector<dep0::error_t>{std::move(as_type.error())}
            : std::vector<dep0::error_t>{std::move(as_type.error()), std::move(as_kind.error())});
}

expected<body_t>
check_body(
    env_t const& env,
    proof_state_t state,
    parser::body_t const& x,
    ast::is_mutable_t const is_mutable,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto stmts =
        fmap_or_error(
            x.stmts,
            [&] (parser::stmt_t const& s)
            {
                return check_stmt(env, state, s, is_mutable, usage, usage_multiplier);
            });
    if (stmts)
        return make_legal_body(std::move(*stmts));
    else
        return std::move(stmts.error());
}

expected<stmt_t>
check_stmt(
    env_t const& env,
    proof_state_t& state,
    parser::stmt_t const& s,
    ast::is_mutable_t const is_mutable,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto const loc = s.properties;
    return match(
        s.value,
        [&] (parser::expr_t::app_t const& x) -> expected<stmt_t>
        {
            if (auto app = type_assign_app(env, state.context, x, loc, is_mutable, usage, usage_multiplier))
                return make_legal_stmt(std::move(std::get<expr_t::app_t>(app->value)));
            else
                return std::move(app.error());
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            auto cond =
                check_expr(
                    env, state.context, x.cond, derivation_rules::make_bool(), is_mutable, usage, usage_multiplier);
            if (not cond)
                return std::move(cond.error());
            // Variable usage in each branch must be checked separately, starting afresh from the outer scope.
            // But after each branch, usages have to be combined and accounted towards the outer scope.
            auto true_branch_usage = usage.extend();
            auto false_branch_usage = usage.extend();
            auto const combine_usages = [&]
            {
                auto const& combined =
                    usage_t::merge(
                        true_branch_usage,
                        false_branch_usage,
                        [] (expr_t::var_t const&, ast::qty_t const a, ast::qty_t const b)
                        {
                            return std::max(a, b);
                        });
                return usage.try_add(state.context, combined);
            };
            auto true_branch = [&]
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_true());
                // we must add `true_t(cond)` to the new context only after we have rewritten `cond=true`,
                // otherwise the new context will contain `true_t(true)`, which is not helpful to verify array access
                new_state.context.add_unnamed(derivation_rules::make_true_t(*cond));
                return check_body(env, std::move(new_state), x.true_branch, is_mutable, true_branch_usage, usage_multiplier);
            }();
            if (not true_branch)
                return std::move(true_branch.error());
            auto const add_true_not_cond = [&cond] (ctx_t& dest)
            {
                dest.add_unnamed(
                    derivation_rules::make_true_t(
                        derivation_rules::make_boolean_expr(
                            expr_t::boolean_expr_t::not_t{*cond})));
            };
            if (x.false_branch)
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_false());
                add_true_not_cond(new_state.context);
                auto false_branch =
                    check_body(env, std::move(new_state), *x.false_branch, is_mutable, false_branch_usage, usage_multiplier);
                if (false_branch)
                {
                    if (auto ok = combine_usages(); not ok)
                        return std::move(ok.error());
                    return make_legal_stmt(
                        stmt_t::if_else_t{
                            std::move(*cond),
                            std::move(*true_branch),
                            std::move(*false_branch)
                        });
                }
                else
                    return std::move(false_branch.error());
            }
            // we are dealing with an if-else without an explicit else branch;
            // but if the true branch returns from all its sub-branches,
            // then it means we are now in the implied else branch;
            // we can then rewrite `cond = false` inside the current proof state
            if (returns_from_all_branches(*true_branch))
            {
                state.rewrite(*cond, derivation_rules::make_false());
                add_true_not_cond(state.context);
            }
            if (auto ok = combine_usages(); not ok)
                return std::move(ok.error());
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
                    return error_t(err.str(), loc);
                }
            }
            else if (auto expr = check_expr(env, state.context, *x.expr, state.goal, is_mutable, usage, usage_multiplier))
                return make_legal_stmt(stmt_t::return_t{std::move(*expr)});
            else
                return std::move(expr.error());
        },
        [&] (parser::stmt_t::impossible_t const& x) -> expected<stmt_t>
        {
            auto const check_impossible_stmt = [&] (ctx_t const& ctx, std::optional<expr_t> reason) -> expected<stmt_t>
            {
                auto const false_type = sort_t{derivation_rules::make_true_t(derivation_rules::make_false())};
                for (auto const& v: ctx.vars())
                    if (is_beta_delta_equivalent(env, ctx, ctx[v]->value.type, false_type))
                        return make_legal_stmt(stmt_t::impossible_t{std::move(reason)});
                return error_t("proof of false not found", loc);
            };
            if (x.reason)
            {
                // reasons consume no resources and can contain mutable operations (its type cannot; but the value can)
                auto reason = type_assign(env, state.context, *x.reason, ast::is_mutable_t::yes, usage, ast::qty_t::zero);
                if (not reason)
                    return std::move(reason.error());
                auto ctx2 = state.context.extend();
                if (auto const reason_type = std::get_if<expr_t>(&reason->properties.sort.get()))
                    ctx2.add_unnamed(*reason_type);
                return check_impossible_stmt(ctx2, std::move(*reason));
            }
            else
                return check_impossible_stmt(state.context, std::nullopt);
        });
}

expected<expr_t>
check_numeric_expr(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    expr_t const& expected_type)
{
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
            return error_t(err.str(), loc);
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
            return error_t(err.str(), loc);
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
                        [&] (type_def_t::integer_t const& t) -> expected<expr_t>
                        {
                            return t.sign == ast::sign_t::signed_v
                                ? check_int(t.name, cpp_int_min_signed(t.width), cpp_int_max_signed(t.width))
                                : check_int(t.name, 0ul, cpp_int_max_unsigned(t.width));
                        });
                },
                [&] (axiom_t const& axiom) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << axiom.name << '`';
                    return error_t(err.str(), loc);
                },
                [&] (extern_decl_t const& extern_decl) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << extern_decl.name << '`';
                    return error_t(err.str(), loc);
                },
                [&] (func_decl_t const& func_decl) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << func_decl.name << '`';
                    return error_t(err.str(), loc);
                },
                [&] (func_def_t const& func_def) -> expected<expr_t>
                {
                    std::ostringstream err;
                    err << "type mismatch between numeric constant and `" << func_def.name << '`';
                    return error_t(err.str(), loc);
                });
        },
        [&] (auto const& x) -> expected<expr_t>
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << "type mismatch between numeric constant and `", x) << '`';
            return error_t(err.str(), loc);
        });
}

expected<expr_t>
check_expr(
    env_t const& env,
    ctx_t const& ctx,
    parser::expr_t const& x,
    sort_t const& expected_type,
    ast::is_mutable_t const is_mutable,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    auto const loc = x.properties;
    auto const type_error = [&] (sort_t const& actual_type, dep0::error_t reason)
    {
        std::ostringstream err;
        pretty_print(err << "type mismatch between expression of type `", actual_type) << '`';
        pretty_print(err << " and expected type `", expected_type) << '`';
        return dep0::error_t(err.str(), loc, std::vector{std::move(reason)});
    };
    // some expressions cannot be type-assigned without an expected type, so we have to add a special case;
    // for all other expressions we can type-assign and check that the assigned type is what we expect
    return match(
        x.value,
        [&] (parser::expr_t::auto_t const& x) -> expected<expr_t>
        {
            return match(
                expected_type,
                [&] (expr_t const& expected_type) -> expected<expr_t>
                {
                    if (auto p = search_proof(env, ctx, expected_type, is_mutable, usage, usage_multiplier))
                        return std::move(*p);
                    else
                    {
                        std::ostringstream err;
                        pretty_print(err << "could not find a value of type `", expected_type) << '`';
                        return error_t(err.str(), loc);
                    }
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between auto-expression and `", kind_t{}) << '`';
                    return error_t(err.str(), loc);
                });
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
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
                    return error_t(err.str(), loc);
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] <typename T> (T const& x) -> expected<expr_t>
                {
                    auto lhs = check_expr(env, ctx, x.lhs.get(), expected_type, is_mutable, usage, usage_multiplier);
                    if (not lhs) return std::move(lhs.error());
                    auto rhs = check_expr(env, ctx, x.rhs.get(), expected_type, is_mutable, usage, usage_multiplier);
                    if (not rhs) return std::move(rhs.error());
                    if constexpr (std::is_same_v<T, parser::expr_t::arith_expr_t::div_t>)
                    {
                        auto const zero = make_legal_expr(expected_type, expr_t::numeric_constant_t{0});
                        auto const proof_type =
                            derivation_rules::make_true_t(
                                derivation_rules::make_relation_expr(
                                    expr_t::relation_expr_t::neq_t{*rhs, zero}));
                        if (not search_proof(env, ctx, proof_type, ast::is_mutable_t::no, usage, ast::qty_t::zero))
                        {
                            std::ostringstream err;
                            pretty_print(err << "cannot verify that divisor `", *rhs) << "` is non-zero";
                            return dep0::error_t(err.str(), loc);
                        }
                    }
                    return make_legal_expr(
                        expected_type,
                        boost::hana::overload(
                            [&] (boost::hana::type<parser::expr_t::arith_expr_t::plus_t>) -> expr_t::arith_expr_t
                            {
                                return {expr_t::arith_expr_t::plus_t{std::move(*lhs), std::move(*rhs)}};
                            },
                            [&] (boost::hana::type<parser::expr_t::arith_expr_t::minus_t>) -> expr_t::arith_expr_t
                            {
                                return {expr_t::arith_expr_t::minus_t{std::move(*lhs), std::move(*rhs)}};
                            },
                            [&] (boost::hana::type<parser::expr_t::arith_expr_t::mult_t>) -> expr_t::arith_expr_t
                            {
                                return {expr_t::arith_expr_t::mult_t{std::move(*lhs), std::move(*rhs)}};
                            },
                            [&] (boost::hana::type<parser::expr_t::arith_expr_t::div_t>) -> expr_t::arith_expr_t
                            {
                                return {expr_t::arith_expr_t::div_t{std::move(*lhs), std::move(*rhs)}};
                            })(boost::hana::type_c<T>));
                });
        },
        [&] (parser::expr_t::init_list_t const& list) -> expected<expr_t>
        {
            return match(
                expected_type,
                [&] (expr_t expected_type) -> expected<expr_t>
                {
                    beta_delta_normalize(env, ctx, expected_type);
                    return match(
                        is_list_initializable(expected_type),
                        [&] (is_list_initializable_result::no_t) -> expected<expr_t>
                        {
                            std::ostringstream err;
                            pretty_print(err << "type mismatch between initializer list and `", expected_type) << '`';
                            return error_t(err.str(), loc);
                        },
                        [&] (is_list_initializable_result::unit_t) -> expected<expr_t>
                        {
                            if (not list.values.empty())
                            {
                                std::ostringstream err;
                                pretty_print(err << "initializer list for `", expected_type) << "` must be empty";
                                return error_t(err.str(), loc);
                            }
                            return make_legal_expr(expected_type, expr_t::init_list_t{});
                        },
                        [&] (is_list_initializable_result::true_t) -> expected<expr_t>
                        {
                            if (not list.values.empty())
                            {
                                std::ostringstream err;
                                pretty_print(err << "initializer list for `", expected_type) << "` must be empty";
                                return error_t(err.str(), loc);
                            }
                            return make_legal_expr(expected_type, expr_t::init_list_t{});
                        },
                        [&] (is_list_initializable_result::array_t const& array) -> expected<expr_t>
                        {
                            if (array.size.value != list.values.size())
                            {
                                std::ostringstream err;
                                pretty_print(err << "initializer list for `", expected_type) << '`';
                                pretty_print<properties_t>(
                                    err << " has " << list.values.size() << " elements but was expecting ",
                                    array.size);
                                return error_t(err.str(), loc);
                            }
                            auto values =
                                fmap_or_error(
                                    list.values,
                                    [&] (parser::expr_t const& v)
                                    {
                                        return check_expr(
                                            env, ctx, v, array.element_type, is_mutable, usage, usage_multiplier);
                                    });
                            if (not values)
                                return std::move(values.error());
                            return make_legal_expr(expected_type, expr_t::init_list_t{std::move(*values)});
                        });
                },
                [&] (kind_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between initializer list and `", kind_t{}) << '`';
                    return error_t(err.str(), loc);
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
            auto value = check_expr(env, ctx2, x.value.get(), expected_type, is_mutable, usage, usage_multiplier);
            if (not value)
                return std::move(value.error());
            return make_legal_expr(expected_type, expr_t::because_t{std::move(*value), std::move(*reason)});
        },
        [&] (auto const&) -> expected<expr_t>
        {
            auto result = type_assign(env, ctx, x, is_mutable, usage, usage_multiplier);
            if (result)
                if (auto eq = is_beta_delta_equivalent(env, ctx, result->properties.sort.get(), expected_type); not eq)
                    return type_error(result->properties.sort.get(), std::move(eq.error()));
            return result;
        });
}

expected<expr_t> check_pi_type(
    env_t const& env,
    ctx_t& ctx,
    source_loc_t const& loc,
    ast::is_mutable_t const is_mutable,
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
                return error_t(err.str(), loc, {std::move(type.error())});
            }
            if (auto ok = ctx.try_emplace(var, arg_loc, ctx_t::var_decl_t{arg.qty, *type}); not ok)
                return std::move(ok.error());
            return make_legal_func_arg(arg.qty, std::move(*type), std::move(var));
        });
    if (not args)
        return std::move(args.error());
    auto const ret_type = check_type(env, ctx, parser_ret_type);
    if (not ret_type)
        return dep0::error_t("cannot typecheck function return type", loc, {std::move(ret_type.error())});
    return make_legal_expr(
        ret_type->properties.sort.get(),
        expr_t::pi_t{is_mutable, std::move(*args), *ret_type});
}

} // namespace dep0::typecheck
