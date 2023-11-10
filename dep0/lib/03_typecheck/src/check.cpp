#include "dep0/typecheck/check.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/ast/substitute.hpp"

#include "private/derivation_rules.hpp"
#include "private/returns_from_all_branches.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <cassert>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

#include <iostream>

namespace dep0::typecheck {

// forward declarations
static expected<expr_t> type_assign_func_call(context_t const&, parser::expr_t::app_t const&);
static expected<type_def_t> check_type_def(context_t&, parser::type_def_t const&);
static expected<func_def_t> check_func_def(context_t&, parser::func_def_t const&);
static expected<type_t> check_type(context_t const&, parser::type_t const&);
static expected<body_t> check_body(context_t const&, parser::body_t const&, sort_t const&);
static expected<stmt_t> check_stmt(context_t const&, parser::stmt_t const&, sort_t const&);
static expected<expr_t> check_expr(context_t const&, parser::expr_t const&, sort_t const&);
static expected<std::pair<type_t, expr_t::abs_t>>
    check_abs(context_t const&, parser::expr_t::abs_t const&, std::optional<source_text> const&);

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
expected<expr_t> type_assign_func_call(context_t const& ctx, parser::expr_t::app_t const& f)
{
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t(std::move(msg)), ctx);
    };
    auto func = [&] () -> expected<expr_t>
    {
        auto const var = std::get_if<parser::expr_t::var_t>(&f.func.get().value);
        assert(var and "only invocation by function name is currently supported"); // TODO support any `func` expr
        auto const v = ctx[var->name];
        if (not v)
            return error("function prototype not found");
        return match(
            *v,
            [&] (type_def_t const&) -> expected<expr_t>
            {
                return error("cannot invoke a typedef");
            },
            [&] (type_t::var_t const&) -> expected<expr_t>
            {
                return error("cannot invoke a type variable");
            },
            [&] (expr_t const& expr) -> expected<expr_t>
            {
                return make_legal_expr(expr.properties.sort, expr_t::var_t{var->name});
            });
    }();
    if (not func)
        return func.error();
    auto func_type = [&] () -> expected<type_t::arr_t>
    {
        if (auto const type = std::get_if<type_t>(&func->properties.sort))
            if (auto const arr = std::get_if<type_t::arr_t>(&type->value))
                return *arr;
        std::ostringstream err;
        pretty_print(err << "cannot invoke expression of type `", func->properties.sort) << '`';
        return error(err.str());
    }();
    if (not func_type)
        return func_type.error();
    if (func_type->args.size() != f.args.size())
    {
        std::ostringstream err;
        err << "passed " << f.args.size() << " arguments but was expecting " << func_type->args.size();
        return error(err.str());
    }
    std::vector<expr_t> args;
    for (auto const i: std::views::iota(0ul, func_type->args.size()))
    {
        auto arg = match(
            func_type->args[i].value,
            [&] (func_arg_t::type_arg_t& type_arg) -> expected<expr_t>
            {
                auto type = check_expr(ctx, f.args[i], ast::typename_t{});
                if (not type)
                    return std::move(type.error());
                if (type_arg.var)
                {
                    pretty_print(std::cout << "before normalization" << std::endl, *type) << std::endl;
                    beta_delta_normalize(ctx.delta_reduction_context(), *type);
                    pretty_print(std::cout << "after normalization" << std::endl, *type) << std::endl;
                    substitute(
                        func_type->args.begin() + i + 1,
                        func_type->args.end(),
                        func_type->ret_type.get(),
                        *type_arg.var,
                        std::get<type_t>(type->value));
                }
                return std::move(*type);
            },
            [&] (func_arg_t::term_arg_t const& term_arg) -> expected<expr_t>
            {
                return check_expr(ctx, f.args[i], term_arg.type);
            });
        if (arg)
            args.push_back(std::move(*arg));
        else
            return std::move(arg.error());
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
            auto const [it, inserted] = ctx.try_emplace(ast::indexed_var_t{x.name}, result);
            if (not inserted)
            {
                std::ostringstream err;
                pretty_print(err << "cannot redefine `" << x.name << "` as typedef, previously `", it->second) << '`';
                return error_t::from_error(dep0::error_t{err.str(), type_def.properties}, ctx);
            }
            return result;
        });
}

expected<func_def_t> check_func_def(context_t& ctx, parser::func_def_t const& f)
{
    if (auto abs = check_abs(ctx, f.value, f.name))
    {
        auto const [it, inserted] = ctx.try_emplace(ast::indexed_var_t{f.name}, make_legal_expr(abs->first, abs->second));
        if (not inserted)
        {
            std::ostringstream err;
            pretty_print(err << "cannot redefine `" << f.name << "` as function, previously `", it->second) << '`';
            return error_t::from_error(dep0::error_t(err.str()), ctx);
        }
        ctx.add_definition(ast::indexed_var_t{f.name}, abs->second);
        return make_legal_func_def(f.name, std::move(abs->second));
    }
    else
    {
        if (not abs.error().location)
            abs.error().location = f.properties;
        return std::move(abs.error());
    }
}

expected<type_t> check_type(context_t const& ctx, parser::type_t const& t)
{
    auto const loc = t.properties;
    auto const error = [&] (std::string msg)
    {
        return error_t::from_error(dep0::error_t{std::move(msg), t.properties}, ctx);
    };
    return match(
        t.value,
        [&] (parser::type_t::bool_t const&) -> expected<type_t> { return make_legal_type(type_t::bool_t{}); },
        [&] (parser::type_t::unit_t const&) -> expected<type_t> { return make_legal_type(type_t::unit_t{}); },
        [&] (parser::type_t::i8_t const&) -> expected<type_t> { return make_legal_type(type_t::i8_t{}); },
        [&] (parser::type_t::i16_t const&) -> expected<type_t> { return make_legal_type(type_t::i16_t{}); },
        [&] (parser::type_t::i32_t const&) -> expected<type_t> { return make_legal_type(type_t::i32_t{}); },
        [&] (parser::type_t::i64_t const&) -> expected<type_t> { return make_legal_type(type_t::i64_t{}); },
        [&] (parser::type_t::u8_t const&) -> expected<type_t> { return make_legal_type(type_t::u8_t{}); },
        [&] (parser::type_t::u16_t const&) -> expected<type_t> { return make_legal_type(type_t::u16_t{}); },
        [&] (parser::type_t::u32_t const&) -> expected<type_t> { return make_legal_type(type_t::u32_t{}); },
        [&] (parser::type_t::u64_t const&) -> expected<type_t> { return make_legal_type(type_t::u64_t{}); },
        [&] (parser::type_t::var_t const& var) -> expected<type_t>
        {
            auto const val = ctx[var.name];
            if (not val)
            {
                std::ostringstream err;
                pretty_print(err << "unknown type `", var.name) << '`';
                return error(err.str());
            }
            return match(
                *val,
                [&] (type_def_t const&) -> expected<type_t> { return make_legal_type(type_t::var_t{var.name}); },
                [] (type_t::var_t const& v) -> expected<type_t> { return make_legal_type(v); },
                [&] (expr_t const& expr) -> expected<type_t>
                {
                    return match(
                        expr.properties.sort,
                        [&] (type_t const& type) -> expected<type_t>
                        {
                            std::ostringstream err;
                            pretty_print(err << "expression does not yield a type but a value of type `", type) << '`';
                            return error(err.str());
                        },
                        [&] (ast::typename_t) -> expected<type_t>
                        {
                            return make_legal_type(type_t::var_t{var.name});
                        });
                });
        },
        [&] (parser::type_t::arr_t const& x) -> expected<type_t>
        {
            auto arr_ctx = ctx.extend();
            auto args =
                fmap_or_error(
                    x.args,
                    [&] (parser::func_arg_t const& arg)
                    {
                        return match(
                            arg.value,
                            [&] (parser::func_arg_t::type_arg_t const& type_arg) -> expected<func_arg_t>
                            {
                                auto type_var =
                                    type_arg.var
                                    ? std::optional{type_t::var_t{type_arg.var->name}}
                                    : std::nullopt;
                                if (type_var)
                                {
                                    auto const [it, inserted] = arr_ctx.try_emplace(type_var->name, *type_var);
                                    if (not inserted)
                                    {
                                        std::ostringstream err;
                                        pretty_print(err << "cannot redefine `", type_var->name) << "` as typename";
                                        pretty_print(err << ", previously `", it->second) << '`';
                                        return error_t::from_error(dep0::error_t(err.str()), arr_ctx);
                                    }
                                }
                                return make_legal_func_arg(func_arg_t::type_arg_t{type_var});
                            },
                            [&] (parser::func_arg_t::term_arg_t const& term_arg) -> expected<func_arg_t>
                            {
                                if (auto t = check_type(arr_ctx, term_arg.type))
                                    return make_legal_func_arg(
                                        func_arg_t::term_arg_t{
                                            std::move(*t),
                                            term_arg.var
                                                ? std::optional{expr_t::var_t{term_arg.var->name}}
                                                : std::nullopt});
                                else
                                    return std::move(t.error());
                            });
                    });
            if (not args)
                return std::move(args.error());
            return match(
                x.ret_type.get(),
                [&] (ast::typename_t) -> expected<type_t>
                {
                    return make_legal_type(type_t::arr_t{std::move(*args), sort_t{ast::typename_t{}}});
                },
                [&] (parser::type_t const& t) -> expected<type_t>
                {
                    if (auto ret_type = check_type(arr_ctx, t))
                        return make_legal_type(type_t::arr_t{std::move(*args), sort_t{std::move(*ret_type)}});
                    else
                        return std::move(ret_type.error());
                });
        });
}

expected<body_t> check_body(context_t const& ctx, parser::body_t const& x, sort_t const& sort)
{
    if (auto stmts = fmap_or_error(x.stmts, [&] (parser::stmt_t const& s) { return check_stmt(ctx, s, sort); }))
        return make_legal_body(std::move(*stmts));
    else
        return std::move(stmts.error());
}

expected<stmt_t> check_stmt(context_t const& ctx, parser::stmt_t const& s, sort_t const& sort)
{
    return match(
        s.value,
        [&] (parser::expr_t::app_t const& x) -> expected<stmt_t>
        {
            if (auto f = type_assign_func_call(ctx, x))
                return make_legal_stmt(std::move(std::get<expr_t::app_t>(f->value)));
            else
            {
                if (not f.error().location)
                    f.error().location = s.properties;
                return std::move(f.error());
            }
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            // TODO can we do lazy bind here? something like `expected::bind([] {a}, [] {b}, [] {c}, [] (a,b,c) {x})`
            auto cond = check_expr(ctx, x.cond, make_legal_type(type_t::bool_t{}));
            if (not cond)
                return std::move(cond.error());
            auto true_branch = check_body(ctx, x.true_branch, sort);
            if (not true_branch)
                return std::move(true_branch.error());
            std::optional<body_t> false_branch;
            if (x.false_branch)
            {
                if (auto f = check_body(ctx, *x.false_branch, sort))
                    false_branch.emplace(std::move(*f));
                else
                    return std::move(f.error());
            }
            return make_legal_stmt(
                stmt_t::if_else_t{
                    std::move(*cond),
                    std::move(*true_branch),
                    std::move(false_branch)});
        },
        [&] (parser::stmt_t::return_t const& x) -> expected<stmt_t>
        {
            if (not x.expr)
            {
                if (auto const type = std::get_if<type_t>(&sort))
                    if (std::holds_alternative<type_t::unit_t>(type->value))
                        return make_legal_stmt(stmt_t::return_t{});
                std::ostringstream err;
                pretty_print(err << "expecting expression of sort `", sort) << '`';
                return error_t::from_error(dep0::error_t{err.str(), s.properties}, ctx, sort);
            }
            else if (auto expr = check_expr(ctx, *x.expr, sort))
                return make_legal_stmt(stmt_t::return_t{std::move(*expr)});
            else
                return std::move(expr.error());
        });
}

static expected<expr_t> check_numeric_expr(
    context_t const& ctx,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    type_t const& expected_type)
{
    auto const error = [&] (std::string msg) -> expected<expr_t>
    {
        return error_t::from_error(dep0::error_t{std::move(msg), loc}, ctx, expected_type);
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
                if (s.starts_with(c))
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
        [&] (type_t::bool_t const&) { return error("type mismatch between numeric constant and `bool`"); },
        [&] (type_t::unit_t const&) { return error("type mismatch between numeric constant and `unit_t`"); },
        [&] (type_t::i8_t const&) { return check_integer("i8_t", "+-", "127"); },
        [&] (type_t::i16_t const&) { return check_integer("i16_t", "+-", "32767"); },
        [&] (type_t::i32_t const&) { return check_integer("i32_t", "+-", "2147483647"); },
        [&] (type_t::i64_t const&) { return check_integer("i64_t", "+-", "9223372036854775807"); },
        [&] (type_t::u8_t const&) { return check_integer("u8_t", "+", "255"); },
        [&] (type_t::u16_t const&) { return check_integer("u16_t", "+", "65535"); },
        [&] (type_t::u32_t const&) { return check_integer("u32_t", "+", "4294967295"); },
        [&] (type_t::u64_t const&) { return check_integer("u64_t", "+", "18446744073709551615"); },
        [&] (type_t::var_t const& var) -> expected<expr_t>
        {
            auto const val = ctx[var.name];
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
                                    err << "uknown width for signed integer `static_cast<int>(integer.width)="
                                        << static_cast<int>(integer.width) << "`. ";
                                    err << "This is a compiler bug, please report it!";
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
                                    err << "uknown width for unsigned integer `static_cast<int>(integer.width)="
                                        << static_cast<int>(integer.width) << '`';
                                    err << ". This is a compiler bug, please report it!";
                                    return error(err.str());
                                }
                                return check_integer(integer.name, "+", max_abs);
                            }
                        });
                },
                [&] (type_t::var_t const& t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print<properties_t>(err << "type mismatch between numeric constant and `", t) << '`';
                    return error(err.str());
                },
                [&] (expr_t const&) -> expected<expr_t>
                {
                    assert(false and "type variable yielded an expression");
                    std::ostringstream err;
                    err << "type variable yielded an expression";
                    err << ". This is a compiler bug, please report it!";
                    return error(err.str());
                });
        },
        [&] (type_t::arr_t const& x)
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << "type mismatch between numeric constant and `", x) << '`';
            return error(err.str());
        });
}

expected<expr_t> check_expr(context_t const& ctx, parser::expr_t const& x, sort_t const& sort)
{
    auto const loc = x.properties;
    auto const type_error = [&] (sort_t const& actual_ty)
    {
        std::ostringstream err;
        pretty_print(err << "type mismatch between expression of type `", actual_ty) << '`';
        pretty_print(err << " and expected type `", sort) << '`';
        return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, sort);
    };
    return match(
        x.value,
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<expr_t>
        {
            return match(
                x.value,
                [&] (parser::expr_t::arith_expr_t::plus_t const& x) -> expected<expr_t>
                {
                    // TODO: would be nice to check both lhs and rhs and if both failed gather both reasons
                    // but currently we can only nest `dep0::error_t` not `typecheck::error_t` and we would
                    // end up losing context and target information of the failed operand
                    auto lhs = check_expr(ctx, x.lhs.get(), sort);
                    if (not lhs) return std::move(lhs.error());
                    auto rhs = check_expr(ctx, x.rhs.get(), sort);
                    if (not rhs) return std::move(rhs.error());
                    return make_legal_expr(
                        sort,
                        expr_t::arith_expr_t{
                            // TODO require proof that it doesn't overflow; user can always appeal to `believe_me()`
                            expr_t::arith_expr_t::plus_t{
                                std::move(*lhs),
                                std::move(*rhs)}});
                });
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            return match(
                sort,
                [&] (ast::typename_t) -> expected<expr_t>
                {
                    return type_error(make_legal_type(type_t::bool_t{}));
                },
                [&] (type_t const& expected_type) -> expected<expr_t>
                {
                    if (not std::holds_alternative<type_t::bool_t>(expected_type.value))
                        return type_error(make_legal_type(type_t::bool_t{}));
                    return make_legal_expr(expected_type, expr_t::boolean_constant_t{x.value});
                });
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return match(
                sort,
                [&] (ast::typename_t) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "type mismatch between numeric constant and `", sort) << '`';
                    return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, sort);
                },
                [&] (type_t const& expected_type) -> expected<expr_t>
                {
                    return check_numeric_expr(ctx, x, loc, expected_type);
                });
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto const val = ctx[x.name];
            if (not val)
            {
                std::ostringstream err;
                pretty_print(err << "unknown variable `", x.name) << '`';
                return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, sort);
            }
            return match(
                sort,
                [&] (ast::typename_t) -> expected<expr_t>
                {
                    return match(
                        *val,
                        [&] (type_def_t const&) -> expected<expr_t>
                        {
                            return make_legal_expr(sort, make_legal_type(type_t::var_t{x.name}));
                        },
                        [&] (type_t::var_t const&) -> expected<expr_t>
                        {
                            return make_legal_expr(sort, make_legal_type(type_t::var_t{x.name}));
                        },
                        [&] (expr_t const& expr) -> expected<expr_t>
                        {
                            return match(
                                expr.properties.sort,
                                [&] (ast::typename_t) -> expected<expr_t>
                                {
                                    return expr;
                                },
                                [&] (type_t const& t) -> expected<expr_t>
                                {
                                    return type_error(t);
                                });
                        });
                },
                [&] (type_t const& expected_type) -> expected<expr_t>
                {
                    return match(
                        *val,
                        [&] (type_def_t const&) -> expected<expr_t>
                        {
                            return type_error(ast::typename_t{});
                        },
                        [&] (type_t::var_t const&) -> expected<expr_t>
                        {
                            return type_error(ast::typename_t{});
                        },
                        [&] (expr_t const& expr) -> expected<expr_t>
                        {
                            return match(
                                expr.properties.sort,
                                [&] (ast::typename_t) -> expected<expr_t>
                                {
                                    return type_error(ast::typename_t{});
                                },
                                [&] (type_t const& t) -> expected<expr_t>
                                {
                                    if (auto equivalent = is_alpha_equivalent(t, expected_type))
                                        return make_legal_expr(expected_type, expr_t::var_t{x.name});
                                    else
                                    {
                                        auto err = type_error(t);
                                        err.reasons.push_back(std::move(equivalent.error()));
                                        return err;
                                    }
                                });
                        });
                });
        },
        [&] (parser::expr_t::app_t const& x) -> expected<expr_t>
        {
            auto f = type_assign_func_call(ctx, x);
            if (not f)
            {
                if (not f.error().location)
                    f.error().location = loc;
                if (not f.error().tgt)
                    f.error().tgt = sort;
                return std::move(f.error());
            }
            auto const legal =
                match(
                    sort,
                    [&] (ast::typename_t) -> expected<std::true_type>
                    {
                        return match(
                            f->properties.sort,
                            [] (ast::typename_t) -> expected<std::true_type> { return {}; },
                            [&] (type_t const&) -> expected<std::true_type> { return type_error(f->properties.sort); });
                    },
                    [&] (type_t const& expected_type) -> expected<std::true_type>
                    {
                        return match(
                            f->properties.sort,
                            [&] (ast::typename_t) -> expected<std::true_type> { return type_error(f->properties.sort); },
                            [&] (type_t const& t) -> expected<std::true_type>
                            {
                                // TODO could also be beta-equivalent (i.e. alpha-beta-equivalent); only if sort==kind?
                                if (auto eq = is_alpha_equivalent(t, expected_type))
                                    return {};
                                else
                                    return error_t::from_error(std::move(eq.error()), ctx, sort);
                            });
                    });
            if (not legal)
                return legal.error();
            return f;
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            auto abs = check_abs(ctx, f, std::nullopt);
            if (not abs)
            {
                if (not abs.error().location)
                    abs.error().location = loc;
                return std::move(abs.error());
            }
            // TODO check expected_type
            return error_t::from_error(dep0::error_t{"Abstraction expressions not yet supported", loc}, ctx, sort);
        },
        [&] (parser::type_t const& x) -> expected<expr_t>
        {
            return match(
                sort,
                [&] (ast::typename_t) -> expected<expr_t>
                {
                    if (auto t = check_type(ctx, x))
                        return make_legal_expr(sort, std::move(*t));
                    else
                        return t.error();
                },
                [&] (type_t const& expected_type) -> expected<expr_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "expression yields a type not a value of type `", expected_type) << '`';
                    return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, expected_type);
                });
        });
}

expected<std::pair<type_t, expr_t::abs_t>> check_abs(
    context_t const& ctx,
    parser::expr_t::abs_t const& f,
    std::optional<source_text> const& name)
{
    auto f_ctx = ctx.extend();
    auto args = fmap_or_error(
        f.args,
        [&] (parser::func_arg_t const& x) -> expected<func_arg_t>
        {
            auto const cannot_redefine =
                [&f_ctx] (ast::indexed_var_t const& name, auto const& prev) -> expected<func_arg_t>
                {
                    std::ostringstream err;
                    pretty_print(err << "cannot redefine `", name) << '`';
                    pretty_print(err << " as function argument, previously `", prev) << '`';
                    return error_t::from_error(dep0::error_t{err.str()}, f_ctx);
                };
            return match(
                x.value,
                [&] (parser::func_arg_t::type_arg_t const& arg) -> expected<func_arg_t>
                {
                    auto const type_var = arg.var ? std::optional{type_t::var_t{arg.var->name}} : std::nullopt;
                    if (type_var)
                    {
                        auto const [it, inserted] = f_ctx.try_emplace(type_var->name, *type_var);
                        if (not inserted)
                            return cannot_redefine(type_var->name, it->second);
                    }
                    return make_legal_func_arg(func_arg_t::type_arg_t{type_var});
                },
                [&] (parser::func_arg_t::term_arg_t const& arg) -> expected<func_arg_t>
                {
                    auto ty = check_type(f_ctx, arg.type);
                    if (not ty)
                        return std::move(ty.error());
                    auto const var = arg.var ? std::optional{expr_t::var_t{arg.var->name}} : std::nullopt;
                    if (var)
                    {
                        auto const [it, inserted] = f_ctx.try_emplace(var->name, make_legal_expr(*ty, *var));
                        if (not inserted)
                            return cannot_redefine(var->name, it->second);
                    }
                    return make_legal_func_arg(func_arg_t::term_arg_t{std::move(*ty), var});
                });
        });
    if (not args)
        return std::move(args.error());
    auto ret_sort =
        match(
            f.ret_type,
            [] (ast::typename_t) -> expected<sort_t> { return sort_t{ast::typename_t{}}; },
            [&] (parser::type_t const& ret_type) -> expected<sort_t>
            {
                if (auto t = check_type(f_ctx, ret_type))
                    return sort_t{std::move(*t)};
                else
                    return t.error();
            });
    if (not ret_sort)
        return std::move(ret_sort.error());
    auto func_type = make_legal_type(type_t::arr_t{*args, *ret_sort});
    // if a function has a name it can call itself recursively;
    // to typecheck recursive functions we need to add them to the current function context before checking the body
    if (name)
    {
        auto const func_name = ast::indexed_var_t{*name};
        auto const [it, inserted] = f_ctx.try_emplace(func_name, make_legal_expr(func_type, expr_t::var_t{func_name}));
        if (not inserted)
        {
            std::ostringstream err;
            pretty_print(err << "cannot redefine `" << *name << "` as function, previously `", it->second) << '`';
            return error_t::from_error(dep0::error_t{err.str()}, f_ctx);
        }
    }
    auto body = check_body(f_ctx, f.body, *ret_sort);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (auto const ret_type = std::get_if<type_t>(&*ret_sort))
        if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
        {
            std::ostringstream err;
            if (name)
                err << "in function `" << *name << "` ";
            err << "missing return statement";
            return error_t::from_error(dep0::error_t{err.str()}, f_ctx);
        }
    return std::pair{std::move(func_type), expr_t::abs_t{std::move(*args), std::move(*ret_sort), std::move(*body)}};
}

} // namespace dep0::typecheck
