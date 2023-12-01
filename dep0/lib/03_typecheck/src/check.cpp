#include "dep0/typecheck/check.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/ast/substitute.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/derivation_rules.hpp"
#include "private/proof_state.hpp"
#include "private/returns_from_all_branches.hpp"
#include "private/rewrite.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <cassert>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

// forward declarations
static expected<expr_t> type_assign_app(context_t const&, parser::expr_t::app_t const&, source_loc_t const&);
static expected<type_def_t> check_type_def(context_t&, parser::type_def_t const&);
static expected<func_def_t> check_func_def(context_t&, parser::func_def_t const&);
static expected<body_t> check_body(proof_state_t&, parser::body_t const&);
static expected<stmt_t> check_stmt(proof_state_t&, parser::stmt_t const&);
static expected<expr_t> check_expr(context_t const&, parser::expr_t const&, sort_t const& expected_type);
static expected<expr_t> check_abs(
    context_t const&,
    parser::expr_t::abs_t const&,
    source_loc_t const&,
    std::optional<source_text> const&);
static expected<expr_t> check_numeric_expr(
    context_t const&,
    parser::expr_t::numeric_constant_t const&,
    source_loc_t const&,
    expr_t const& expected_type);
static expected<expr_t> check_pi_type(context_t&, std::vector<parser::func_arg_t> const&, parser::expr_t const& ret_ty);

expected<module_t> check(parser::module_t const& x) noexcept
{
    context_t ctx;
    // TODO maybe one day typedefs can depend on function definitions? if so we will need 2 passes
    auto type_defs = fmap_or_error(x.type_defs, [&] (parser::type_def_t const& t) { return check_type_def(ctx, t); });
    if (not type_defs)
        return std::move(type_defs.error());
    auto func_defs = fmap_or_error(x.func_defs, [&] (parser::func_def_t const& f) { return check_func_def(ctx, f); });
    if (not func_defs)
        return std::move(func_defs.error());
    return make_legal_module(std::move(*type_defs), std::move(*func_defs));
}

// implementations
expected<expr_t> type_assign_app(context_t const& ctx, parser::expr_t::app_t const& app, source_loc_t const& loc)
{
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc), ctx);
    };
    auto func = [&] () -> expected<expr_t>
    {
        auto const var = std::get_if<parser::expr_t::var_t>(&app.func.get().value);
        if (not var)
            // TODO support any `func` expr
            return error("only invocation by function name is currently supported");
        auto const v = ctx[expr_t::var_t{var->name}];
        if (not v)
            return error("function prototype not found");
        return match(
            *v,
            [&] (type_def_t const&) -> expected<expr_t>
            {
                return error("cannot invoke a typedef");
            },
            [&] (expr_t const& expr) -> expected<expr_t>
            {
                return make_legal_expr(expr.properties.sort.get(), expr_t::var_t{var->name});
            });
    }();
    if (not func)
        return std::move(func.error());
    auto func_type = [&] () -> expected<expr_t::pi_t>
    {
        if (auto const type = std::get_if<expr_t>(&func->properties.sort.get()))
            if (auto const pi = std::get_if<expr_t::pi_t>(&type->value))
                return *pi;
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

expected<type_def_t> check_type_def(context_t& ctx, parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [&] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            auto const result = make_legal_type_def(type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value});
            auto const [it, inserted] = ctx.try_emplace(expr_t::var_t{x.name}, result);
            if (not inserted)
            {
                std::ostringstream err;
                pretty_print(err << "cannot redefine `" << x.name << "` as typedef, previously `", it->second) << '`';
                return error_t::from_error(dep0::error_t(err.str(), type_def.properties), ctx);
            }
            return result;
        });
}

expected<func_def_t> check_func_def(context_t& ctx, parser::func_def_t const& f)
{
    auto abs = check_abs(ctx, f.value, f.properties, f.name);
    if (not abs)
        return std::move(abs.error());
    auto const [it, inserted] = ctx.try_emplace(expr_t::var_t{f.name}, *abs);
    if (not inserted)
    {
        std::ostringstream err;
        pretty_print(err << "cannot redefine `" << f.name << "` as function, previously `", it->second) << '`';
        return error_t::from_error(dep0::error_t(err.str()), ctx);
    }
    return make_legal_func_def(abs->properties.sort.get(), f.name, std::move(std::get<expr_t::abs_t>(abs->value)));
}

expected<body_t> check_body(proof_state_t& state, parser::body_t const& x)
{
    if (auto stmts = fmap_or_error(x.stmts, [&] (parser::stmt_t const& s) { return check_stmt(state, s); }))
        return make_legal_body(std::move(*stmts));
    else
        return std::move(stmts.error());
}

expected<stmt_t> check_stmt(proof_state_t& state, parser::stmt_t const& s)
{
    return match(
        s.value,
        [&] (parser::expr_t::app_t const& x) -> expected<stmt_t>
        {
            if (auto app = type_assign_app(state.context, x, s.properties))
                return make_legal_stmt(std::move(std::get<expr_t::app_t>(app->value)));
            else
                return std::move(app.error());
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            auto cond = check_expr(state.context, x.cond, derivation_rules::make_bool());
            if (not cond)
                return std::move(cond.error());
            auto true_branch = [&]
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_true());
                return check_body(new_state, x.true_branch);
            }();
            if (not true_branch)
                return std::move(true_branch.error());
            if (x.false_branch)
            {
                auto new_state = proof_state_t(state.context.extend(), state.goal);
                new_state.rewrite(*cond, derivation_rules::make_false());
                auto false_branch = check_body(new_state, *x.false_branch);
                if (not false_branch)
                    return std::move(false_branch.error());
                return make_legal_stmt(
                    stmt_t::if_else_t{
                        std::move(*cond),
                        std::move(*true_branch),
                        std::move(*false_branch)
                    });
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
                bool const ok =
                    match(
                        state.goal,
                        [&] (expr_t const& return_type)
                        {
                            // TODO beta equivalence too?
                            return is_alpha_equivalent(return_type, derivation_rules::make_unit()).has_value();
                        },
                        [] (kind_t) { return false; });
                if (ok)
                    return make_legal_stmt(stmt_t::return_t{});
                else
                {
                    std::ostringstream err;
                    pretty_print(err << "expecting expression of type `", state.goal) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), s.properties), state.context, state.goal);
                }
            }
            else if (auto expr = check_expr(state.context, *x.expr, state.goal))
                return make_legal_stmt(stmt_t::return_t{std::move(*expr)});
            else
                return std::move(expr.error());
        });
}

static expected<expr_t> check_numeric_expr(
    context_t const& ctx,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    expr_t const& expected_type)
{
    auto const error = [&] (std::string msg) -> expected<expr_t>
    {
        return error_t::from_error(dep0::error_t(std::move(msg), loc), ctx, expected_type);
    };
    auto const check_integer = [&] (
        std::string_view const type_name,
        std::string_view const sign_chars,
        std::string_view const max_abs_value
    ) -> expected<expr_t>
    {
        auto const skip_any = [] (std::string_view& s, std::string_view const chars)
        {
            for (auto const c: chars)
                if (s.starts_with(c)) // TODO should this be `while` instead of `if`?
                    s.remove_prefix(1);
        };
        // assuming `a` and `b` represent positive integer numbers without leading 0s, returns whether `a > b`
        auto const bigger_than = [] (std::string_view const a, std::string_view const b)
        {
            if (a.size() > b.size())
                return true;
            if (a.size() == b.size())
                for (auto const i: std::views::iota(0ul, a.size()))
                    if (a[i] != b[i]) return a[i] > b[i];
            return false;
        };

        std::optional<std::string> without_separator; // keeps alive the string pointed to by `number`
        std::string_view number;
        if (contains_digit_separator(x.number))
            number = without_separator.emplace(remove_digit_separator(x.number));
        else
            number = x.number;
        if (x.sign and sign_chars.find(*x.sign) == std::string_view::npos)
            return error("invalid sign for numeric constant");
        skip_any(number, "0");
        if (bigger_than(number, max_abs_value))
        {
            std::ostringstream err;
            err << "numeric constant does not fit inside `" << type_name << '`';
            return error(err.str());
        }
        return make_legal_expr(expected_type, expr_t::numeric_constant_t{x.sign, x.number});
    };
    return match(
        expected_type.value,
        [&] (expr_t::i8_t const&) { return check_integer("i8_t", "+-", "127"); },
        [&] (expr_t::i16_t const&) { return check_integer("i16_t", "+-", "32767"); },
        [&] (expr_t::i32_t const&) { return check_integer("i32_t", "+-", "2147483647"); },
        [&] (expr_t::i64_t const&) { return check_integer("i64_t", "+-", "9223372036854775807"); },
        [&] (expr_t::u8_t const&) { return check_integer("u8_t", "+", "255"); },
        [&] (expr_t::u16_t const&) { return check_integer("u16_t", "+", "65535"); },
        [&] (expr_t::u32_t const&) { return check_integer("u32_t", "+", "4294967295"); },
        [&] (expr_t::u64_t const&) { return check_integer("u64_t", "+", "18446744073709551615"); },
        [&] (expr_t::var_t const& var) -> expected<expr_t>
        {
            auto const val = ctx[var];
            assert(val and "unknown type variable despite typecheck succeeded for the expected type");
            return match(
                *val,
                [&] (type_def_t const& t) -> expected<expr_t>
                {
                    return match(
                        t.value,
                        [&] (type_def_t::integer_t const& integer) -> expected<expr_t>
                        {
                            if (integer.sign == ast::sign_t::signed_v)
                            {
                                std::string_view const max_abs =
                                    integer.max_abs_value ? integer.max_abs_value->view() :
                                    integer.width == ast::width_t::_8 ? "127" :
                                    integer.width == ast::width_t::_16 ? "32767" :
                                    integer.width == ast::width_t::_32 ? "2147483647" :
                                    integer.width == ast::width_t::_64 ? "9223372036854775807" :
                                    "";
                                if (max_abs.empty())
                                {
                                    std::ostringstream err;
                                    err << "unknown width for signed integer `static_cast<int>(integer.width)="
                                        << static_cast<int>(integer.width) << '`';
                                    err << ". This is a compiler bug, please report it!";
                                    return error(err.str());
                                }
                                return check_integer(integer.name, "+-", max_abs);
                            }
                            else
                            {
                                std::string_view const max_abs =
                                    integer.max_abs_value ? integer.max_abs_value->view() :
                                    integer.width == ast::width_t::_8 ? "255" :
                                    integer.width == ast::width_t::_16 ? "65535" :
                                    integer.width == ast::width_t::_32 ? "4294967295" :
                                    integer.width == ast::width_t::_64 ? "18446744073709551615" :
                                    "";
                                if (max_abs.empty())
                                {
                                    std::ostringstream err;
                                    err << "unknown width for unsigned integer `static_cast<int>(integer.width)="
                                        << static_cast<int>(integer.width) << '`';
                                    err << ". This is a compiler bug, please report it!";
                                    return error(err.str());
                                }
                                return check_integer(integer.name, "+", max_abs);
                            }
                        });
                },
                [&] (expr_t const& expr) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between numeric constant and `", expr) << '`';
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

expected<expr_t> check_expr(context_t const& ctx, parser::expr_t const& x, sort_t const& expected_type)
{
    auto const loc = x.properties;
    auto const type_error = [&] (sort_t const& actual_type, dep0::error_t reason)
    {
        std::ostringstream err;
        pretty_print(err << "type mismatch between expression of type `", actual_type) << '`';
        pretty_print(err << " and expected type `", expected_type) << '`';
        return error_t::from_error(dep0::error_t(err.str(), loc, std::vector{std::move(reason)}), ctx, expected_type);
    };
    return match(
        x.value,
        [&] (parser::expr_t::typename_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_typename();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::bool_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_bool();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::unit_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_unit();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::i8_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_i8();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::i16_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_i16();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::i32_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_i32();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::i64_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_i64();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::u8_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_u8();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::u16_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_u16();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::u32_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_u32();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::u64_t) -> expected<expr_t>
        {
            auto const result = derivation_rules::make_u64();
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            auto result = make_legal_expr(derivation_rules::make_bool(), expr_t::boolean_constant_t{x.value});
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return match(
                expected_type,
                [&] (expr_t expected_type) -> expected<expr_t>
                {
                    beta_delta_normalize(ctx.delta_reduction_context(), expected_type);
                    return check_numeric_expr(ctx, x, loc, expected_type);
                },
                [&] (kind_t const&) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between numeric constant and `", kind_t{}) << '`';
                    return error_t::from_error(dep0::error_t(err.str(), loc), ctx, expected_type);
                });
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    // TODO: would be nice to check both lhs and rhs and if both failed gather both reasons
                    // but currently we can only nest `dep0::error_t` not `typecheck::error_t` and we would
                    // end up losing context and target information of the failed operand
                    auto lhs = check_expr(ctx, x.lhs.get(), expected_type);
                    if (not lhs) return std::move(lhs.error());
                    auto rhs = check_expr(ctx, x.rhs.get(), expected_type);
                    if (not rhs) return std::move(rhs.error());
                    return make_legal_expr(
                        expected_type,
                        expr_t::arith_expr_t{
                            // TODO require proof that it doesn't overflow; user can always appeal to `believe_me()`
                            expr_t::arith_expr_t::plus_t{
                                std::move(*lhs),
                                std::move(*rhs)}});
                });
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto const val = ctx[expr_t::var_t{x.name}];
            if (not val)
            {
                std::ostringstream err;
                pretty_print<parser::properties_t>(err << "unknown variable `", x) << '`';
                return error_t::from_error(dep0::error_t(err.str(), loc), ctx, expected_type);
            }
            auto const result = 
                make_legal_expr(
                    match(
                        *val,
                        [&] (type_def_t const&) -> sort_t { return derivation_rules::make_typename(); },
                        [&] (expr_t const& expr) -> sort_t { return expr.properties.sort.get(); }),
                    expr_t::var_t{x.name});
            if (auto eq = is_beta_delta_equivalent(ctx, result.properties.sort.get(), expected_type))
                return result;
            else
                return type_error(result.properties.sort.get(), std::move(eq.error()));
        },
        [&] (parser::expr_t::app_t const& x) -> expected<expr_t>
        {
            auto result = type_assign_app(ctx, x, loc);
            if (result)
                if (auto eq = is_beta_delta_equivalent(ctx, result->properties.sort.get(), expected_type); not eq)
                    return type_error(result->properties.sort.get(), std::move(eq.error()));
            return result;
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            auto abs = check_abs(ctx, f, loc, std::nullopt);
            if (not abs)
                return std::move(abs.error());
            // TODO check expected_type
            return error_t::from_error(
                dep0::error_t{"Abstraction expressions not yet supported", loc},
                ctx,
                expected_type);
        },
        [&] (parser::expr_t::pi_t const& pi) -> expected<expr_t>
        {
            auto pi_ctx = ctx.extend();
            auto result = check_pi_type(pi_ctx, pi.args, pi.ret_type.get());
            if (result)
                if (auto eq = is_beta_delta_equivalent(pi_ctx, result->properties.sort.get(), expected_type); not eq)
                    return type_error(result->properties.sort.get(), std::move(eq.error()));
            return result;
        });
}

expected<expr_t> check_pi_type(
    context_t& ctx,
    std::vector<parser::func_arg_t> const& parser_args,
    parser::expr_t const& parser_ret_type)
{
    int next_arg_index = 0;
    auto args = fmap_or_error(
        parser_args,
        [&] (parser::func_arg_t const& arg) -> expected<func_arg_t>
        {
            auto const arg_index = next_arg_index++;
            auto const arg_loc = arg.properties;
            auto var = arg.var ? std::optional{expr_t::var_t{arg.var->name}} : std::nullopt;
            auto const cannot_redefine = [&] (auto const& prev) -> expected<func_arg_t>
            {
                assert(var);
                std::ostringstream err;
                pretty_print<properties_t>(err << "cannot redefine `", *var) << '`';
                pretty_print(err << " as function argument, previously `", prev) << '`';
                return error_t::from_error(dep0::error_t(err.str(), arg_loc), ctx);
            };
            // It can be argued that here we are "guessing" whether the argument is a type or a kind.
            // Type-assignment in lambda-c is decidable and unique up to alpha/beta-equivalence.
            // Therefore there should not be any need for "guessing". This is coming from the fact that
            // currently numerical constants cannot be type-assigned without providing a "tie-breaker".
            // If we get rid of that (perhaps by type-assigning consatnts to i32_t and then convert them),
            // we could restructure this entire type-checking around a more "traditional" 2-step approach
            // of type-assigning first and then comparing against the expected type.
            auto type = check_expr(ctx, arg.type, derivation_rules::make_typename());
            if (type)
            {
                if (var)
                {
                    auto const [it, inserted] = ctx.try_emplace(*var, make_legal_expr(*type, *var));
                    if (not inserted)
                        return cannot_redefine(it->second);
                }
                return make_legal_func_arg(std::move(*type), std::move(var));
            }
            auto kind = check_expr(ctx, arg.type, kind_t{});
            if (kind)
            {
                if (var)
                {
                    auto const [it, inserted] = ctx.try_emplace(*var, make_legal_expr(*kind, *var));
                    if (not inserted)
                        return cannot_redefine(it->second);
                }
                return make_legal_func_arg(std::move(*kind), std::move(var));
            }
            std::ostringstream err;
            if (var)
                pretty_print<properties_t>(err << "cannot typecheck function argument `", *var) << '`';
            else
                err << "cannot typecheck function argument at index " << arg_index;
            return error_t::from_error(
                dep0::error_t(
                    err.str(),
                    arg_loc,
                    {
                        dep0::error_t("expression is not a type", {std::move(type.error())}),
                        dep0::error_t("expression is not a kind", {std::move(kind.error())})
                    }),
                ctx);
        });
    if (not args)
        return std::move(args.error());
    auto const ret_type = [&] () -> expected<expr_t>
    {
        auto type = check_expr(ctx, parser_ret_type, derivation_rules::make_typename());
        if (type)
            return std::move(type);
        auto kind = check_expr(ctx, parser_ret_type, kind_t{});
        if (kind)
            return std::move(kind);
        std::ostringstream err;
        err << "cannot typecheck function return type";
        return error_t::from_error(
            dep0::error_t(err.str(), parser_ret_type.properties, {std::move(type.error()), std::move(kind.error())}),
            ctx);
    }();
    if (not ret_type)
        return std::move(ret_type.error());
    return make_legal_expr(ret_type->properties.sort.get(), expr_t::pi_t{std::move(*args), *ret_type});
}

expected<expr_t> check_abs(
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
        auto const [it, inserted] = f_ctx.try_emplace(func_name, make_legal_expr(*func_type, func_name));
        if (not inserted)
        {
            std::ostringstream err;
            pretty_print(err << "cannot redefine `" << *name << "` as function, previously `", it->second) << '`';
            return error_t::from_error(dep0::error_t(err.str(), location), f_ctx);
        }
    }
    auto const& pi_type = std::get<expr_t::pi_t>(func_type->value);
    auto const& ret_type = pi_type.ret_type.get();
    auto state = proof_state_t(f_ctx, ret_type);
    auto body = check_body(state, f.body);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not returns_from_all_branches(*body))
    {
        if (not is_beta_delta_equivalent(f_ctx, ret_type, derivation_rules::make_unit()))
        {
            std::ostringstream err;
            if (name)
                err << "in function `" << *name << "` ";
            err << "missing return statement";
            return error_t::from_error(dep0::error_t(err.str(), location), f_ctx);
        }
    }
    return make_legal_expr(*func_type, expr_t::abs_t{pi_type.args, ret_type, std::move(*body)});
}

} // namespace dep0::typecheck
