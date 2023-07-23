#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/match.hpp"

#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

struct func_proto_t // this should really be in the AST
{
    std::vector<func_def_t::arg_t> args;
};

// TODO: improve... maybe define `typecheck::context_t` (with extension operations) with typedefs and `tt::context_t`
using typedefs_t = std::map<source_text, type_def_t>;
using fnprotos_t = std::map<source_text, func_proto_t>; // yeah both of these need to be scoped...

// forward declarations
static expected<type_def_t> check(tt::context_t, parser::type_def_t const&);
static expected<func_def_t> check(tt::context_t, fnprotos_t&, typedefs_t const&, parser::func_def_t const&);
static expected<func_def_t::arg_t> check(tt::context_t, typedefs_t const&, parser::func_def_t::arg_t const&);
static expected<type_t> check(tt::context_t, typedefs_t const&, parser::type_t const&);
static expected<body_t> check(tt::context_t, fnprotos_t const&, typedefs_t const&, parser::body_t const&, type_t const& return_type);
static expected<stmt_t> check(tt::context_t, fnprotos_t const&, typedefs_t const&, parser::stmt_t const&, type_t const& return_type);
static expected<expr_t> check(tt::context_t, fnprotos_t const&, typedefs_t const&, parser::expr_t const&, type_t const& expected_type);

static tt::type_t make_arrow_type(std::vector<func_def_t::arg_t> const& args, type_t const& ret)
{
    auto const first = args.begin();
    auto const last = args.end();
    return first == last
        ? tt::type_of(ret.properties.derivation)
        : tt::type_t::arr(
            tt::type_of(first->type.properties.derivation),
            std::accumulate( // right-associative accumulate, from last to 2nd function argument
                args.rbegin(), std::make_reverse_iterator(std::next(first)),
                tt::type_of(ret.properties.derivation),
                [] (tt::type_t img, func_def_t::arg_t const& arg)
                {
                    return tt::type_t::arr(tt::type_of(arg.type.properties.derivation), std::move(img));
                }));
}

expected<module_t> check(parser::module_t const& x)
{
    tt::context_t ctx;
    typedefs_t typedefs;
    fnprotos_t protos;
    std::vector<type_def_t> type_defs;
    std::vector<func_def_t> func_defs;
    type_defs.reserve(x.type_defs.size());
    func_defs.reserve(x.func_defs.size());
    for (auto const& t: x.type_defs)
        if (auto res = check(ctx, t))
        {
            auto const name = match(t.value, [] (auto const& x) { return x.name; });
            if (auto ok = ctx.add(tt::type_t::var_t(name));
                not ok)
            {
                ok.error().location = t.properties;
                return error_t::from_error(std::move(ok.error()), ctx);
            }
            typedefs.emplace(name, *res);
            type_defs.push_back(std::move(*res));
        }
        else
            return std::move(res.error());
    for (auto const& f: x.func_defs)
        if (auto res = check(ctx, protos, typedefs, f))
        {
            if (auto ok = ctx.add(tt::term_t::var_t(f.name), make_arrow_type(res->args, res->type));
                not ok)
            {
                ok.error().location = f.properties;
                return error_t::from_error(std::move(ok.error()), ctx);
            }
            func_defs.push_back(std::move(*res));
        }
        else
            return std::move(res.error());
    return module_t{legal_module_t{}, std::move(type_defs), std::move(func_defs)};
}

// implementations
expected<type_def_t> check(tt::context_t ctx, parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            return type_def_t{legal_type_def_t{}, type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value}};
        });
}

expected<func_def_t> check(tt::context_t ctx, fnprotos_t& protos, typedefs_t const& typedefs, parser::func_def_t const& f)
{
    ctx = ctx.extend();
    auto ret_type = check(ctx, typedefs, f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    std::vector<func_def_t::arg_t> args;
    for (auto const& arg: f.args)
        if (auto type = check(ctx, typedefs, arg.type))
        {
            if (auto ok = ctx.add(tt::term_t::var_t(arg.name), tt::type_of(type->properties.derivation)))
                args.push_back(func_def_t::arg_t{std::move(*type), arg.name});
            else
            {
                ok.error().location = f.properties;
                return error_t::from_error(ok.error(), ctx);
            }
        }
        else
            return std::move(type.error());
    if (auto ok = ctx.add(tt::term_t::var_t(f.name), make_arrow_type(args, *ret_type)); not ok)
    {
        ok.error().location = f.properties;
        return error_t::from_error(ok.error(), ctx);
    }
    protos[f.name].args = args;
    auto body = check(ctx, protos, typedefs, f.body, *ret_type);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
    {
        std::ostringstream err;
        err << "in function `" << f.name << "` missing return statement";
        return error_t::from_error(dep0::error_t{err.str(), f.properties}, ctx, tt::type_of(ret_type->properties.derivation));
    }
    return func_def_t{legal_func_def_t{}, std::move(*ret_type), f.name, std::move(args), std::move(*body)};
}

expected<type_t> check(tt::context_t ctx, typedefs_t const& typedefs, parser::type_t const& t)
{
    auto const loc = t.properties;
    return match(
        t.value,
        [&] (parser::type_t::bool_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::bool_t(), type_t::bool_t{}};
        },
        [&] (parser::type_t::unit_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::unit_t(), type_t::unit_t{}};
        },
        [&] (parser::type_t::i8_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::i8_t(), type_t::i8_t{}};
        },
        [&] (parser::type_t::i16_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::i16_t(), type_t::i16_t{}};
        },
        [&] (parser::type_t::i32_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::i32_t(), type_t::i32_t{}};
        },
        [&] (parser::type_t::i64_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::i64_t(), type_t::i64_t{}};
        },
        [&] (parser::type_t::u8_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::u8_t(), type_t::u8_t{}};
        },
        [&] (parser::type_t::u16_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::u16_t(), type_t::u16_t{}};
        },
        [&] (parser::type_t::u32_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::u32_t(), type_t::u32_t{}};
        },
        [&] (parser::type_t::u64_t const&) -> expected<type_t>
        {
            return type_t{tt::derivation_t::u64_t(), type_t::u64_t{}};
        },
        [&] (parser::type_t::name_t const& name) -> expected<type_t>
        {
            if (auto d = tt::type_assign(ctx, tt::type_t::var_t(name.name)))
                return type_t{std::move(*d), type_t::name_t{name.name}};
            else
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx);
            }
        });
}

expected<body_t> check(tt::context_t ctx, fnprotos_t const& protos, typedefs_t const& typedefs, parser::body_t const& x, type_t const& return_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        if (auto stmt = check(ctx, protos, typedefs, s, return_type))
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return body_t{legal_body_t{}, std::move(stmts)};
}

expected<stmt_t> check(tt::context_t ctx, fnprotos_t const& protos, typedefs_t const& typedefs, parser::stmt_t const& s, type_t const& return_type)
{
    auto const loc = s.properties;
    return match(
        s.value,
        [&] (parser::stmt_t::fun_call_t const& x) -> expected<stmt_t>
        {
            if (auto d = tt::type_assign(ctx, tt::term_t::var(x.name)))
                return stmt_t{legal_stmt_t{}, stmt_t::fun_call_t{x.name}};
            else
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx);
            }
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            auto cond = check(ctx, protos, typedefs, x.cond, type_t{tt::derivation_t::bool_t(), type_t::bool_t{}});
            if (not cond)
                return std::move(cond.error());
            auto true_branch = check(ctx, protos, typedefs, x.true_branch, return_type);
            if (not true_branch)
                return std::move(true_branch.error());
            std::optional<body_t> false_branch;
            if (x.false_branch)
            {
                if (auto f = check(ctx, protos, typedefs, *x.false_branch, return_type))
                    false_branch.emplace(std::move(*f));
                else
                    return std::move(f.error());
            }
            return stmt_t{
                legal_stmt_t{},
                stmt_t::if_else_t{
                    std::move(*cond),
                    std::move(*true_branch),
                    std::move(false_branch)}};
        },
        [&] (parser::stmt_t::return_t const& x) -> expected<stmt_t>
        {
            if (not x.expr)
            {
                if (std::holds_alternative<type_t::unit_t>(return_type.value))
                    return expected<stmt_t>{std::in_place, legal_stmt_t{}, stmt_t::return_t{}};
                std::ostringstream err;
                tt::pretty_print(
                    err << "expecting expression of type `",
                    tt::type_of(return_type.properties.derivation)
                ) << '`';
                return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, tt::type_of(return_type.properties.derivation));
            }
            else if (auto expr = check(ctx, protos, typedefs, *x.expr, return_type))
                return stmt_t{legal_stmt_t{}, stmt_t::return_t{std::move(*expr)}};
            else
                return std::move(expr.error());
        });
}

static expected<expr_t> check_numeric_expr(
    tt::context_t const& ctx,
    typedefs_t const& typedefs,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    type_t const& expected_type)
{
    auto const error = [&] (std::string msg) -> expected<expr_t>
    {
        auto const tgt = tt::type_of(expected_type.properties.derivation);
        return error_t::from_error(dep0::error_t{std::move(msg), loc}, ctx, tgt);
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
        auto const skip_zero_or_one = [] (std::string_view& s, std::string_view const chars)
        {
            for (auto const c: chars)
                if (s.starts_with(c))
                {
                    s.remove_prefix(1);
                    return;
                }
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
        skip_zero_or_one(number, sign_chars);
        skip_any(number, "0");
        if (number.starts_with('-'))
            // if we were checking for signed integer we would skip the `-`, so we must be checking for unsigned
            // TODO should we allow `-0` though? not sure...
            return error("invalid negative constant for unsigned integer");
        if (bigger_than(number, max_abs_value))
        {
            std::ostringstream err;
            err << "numeric constant does not fit inside `" << type_name << '`';
            return error(err.str());
        }
        return expr_t{expected_type, expr_t::numeric_constant_t{x.number}};
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
        [&] (type_t::name_t const& name) -> expected<expr_t>
        {
            auto const it = typedefs.find(name.name);
            if (it == typedefs.end())
            {
                std::ostringstream err;
                err << "could not find typedef for name `" << name.name << "` even though the typeref typechecked. ";
                err << "This is a compiler bug, please report it!";
                return error(err.str());
            }
            return match(
                it->second.value,
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
                                << static_cast<int>(integer.width) << "`. ";
                            err << "This is a compiler bug, please report it!";
                            return error(err.str());
                        }
                        return check_integer(integer.name, "+", max_abs);
                    }
                });
        });
}

// TODO is `next_constant_id` a valid approach? it probably works for type checking but it won't work for term finding
static tt::term_t term_from_expr(tt::context_t&, std::size_t& next_constant_id, expr_t::fun_call_t const&);
static tt::term_t term_from_expr(tt::context_t& ctx, std::size_t& next_constant_id, expr_t const& expr)
{
    return match(
        expr.value,
        [&] (expr_t::boolean_constant_t const& x)
        {
            auto d = tt::type_of(expr.properties.type.properties.derivation);
            do
            {
                auto c = tt::term_t::const_t(next_constant_id++, x.value);
                if (ctx.add(c, d))
                    return tt::term_t(c);
            }
            while (true);
        },
        [&] (expr_t::numeric_constant_t const& x)
        {
            auto d = tt::type_of(expr.properties.type.properties.derivation);
            do
            {
                auto c = tt::term_t::const_t(next_constant_id++, x.number);
                if (ctx.add(c, d))
                    return tt::term_t(c);
            }
            while (true);
        },
        [&] (expr_t::fun_call_t const& x)
        {
            return term_from_expr(ctx, next_constant_id, x);
        },
        [&] (expr_t::var_t const& x)
        {
            return tt::term_t::var(x.name);
        });
}

tt::term_t term_from_expr(tt::context_t& ctx, std::size_t& next_constant_id, expr_t::fun_call_t const& x)
{
    auto const it = x.args.begin();
    return it == x.args.end()
        ? tt::term_t::var(x.name)
        : std::accumulate(
            std::next(it), x.args.end(),
            tt::term_t::app(tt::term_t::var(x.name), term_from_expr(ctx, next_constant_id, *it)),
            [&] (tt::term_t left, expr_t const& x)
            {
                return tt::term_t::app(std::move(left), term_from_expr(ctx, next_constant_id, x));
            });
}

expected<expr_t> check(
    tt::context_t ctx,
    fnprotos_t const& protos,
    typedefs_t const& typedefs,
    parser::expr_t const& x,
    type_t const& expected_type)
{
    auto const loc = x.properties;
    auto const tgt = tt::type_of(expected_type.properties.derivation);
    auto const type_error = [&] (tt::type_t const& actual_ty)
    {
        std::ostringstream err;
        tt::pretty_print(err << "expression of type `", actual_ty) << '`';
        tt::pretty_print(err << " does not typecheck with expected type `", tgt) << '`';
        return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, tgt);
    };
    return match(
        x.value,
        [&] (parser::expr_t::fun_call_t const& x) -> expected<expr_t>
        {
            auto const error = [&] (std::vector<dep0::error_t> reasons)
            {
                std::ostringstream err;
                err << "invocation of function `" << x.name << "` does not typecheck";
                return error_t::from_error(dep0::error_t{err.str(), loc, std::move(reasons)}, ctx, tgt);
            };
            auto const proto_it = protos.find(x.name);
            if (proto_it == protos.end())
                return error({dep0::error_t{"function prototype not found"}});
            if (proto_it->second.args.size() != x.args.size())
            {
                std::ostringstream err;
                err << "passed " << x.args.size() << " arguments but was expecting " << proto_it->second.args.size();
                return error({dep0::error_t{err.str()}});
            }
            std::vector<expr_t> args;
            std::vector<dep0::error_t> reasons;
            for (auto const i: std::views::iota(0ul, x.args.size()))
                if (auto expr = check(ctx, protos, typedefs, x.args[i], proto_it->second.args[i].type))
                    args.push_back(std::move(*expr));
                else
                    reasons.push_back(std::move(expr.error()));
            if (not reasons.empty())
                return error(std::move(reasons));
            std::size_t next_constant_id = 0ul;
            auto call = expr_t::fun_call_t{x.name, std::move(args)};
            ctx = ctx.extend(); // temporarily extend a new context to hold the constant expressions
            auto term = term_from_expr(ctx, next_constant_id, call);
            auto d = tt::type_assign(ctx, term);
            if (not d)
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx, tgt);
            }
            if (tt::type_of(*d) != tgt)
                return type_error(tt::type_of(*d));
            return expr_t{legal_expr_t{expected_type}, std::move(call)};
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            if (tgt != tt::type_of(tt::derivation_t::bool_t()))
                return type_error(tt::type_of(tt::derivation_t::bool_t()));
            return expr_t{legal_expr_t{expected_type}, expr_t::boolean_constant_t{x.value}};
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return check_numeric_expr(ctx, typedefs, x, loc, expected_type);
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto d = tt::type_assign(ctx, tt::term_t::var(x.name));
            if (not d)
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx, tgt);
            }
            if (tt::type_of(*d) != tgt)
                return type_error(tt::type_of(*d));
            return expr_t{legal_expr_t{expected_type}, expr_t::var_t{x.name}};
        });
}

} // namespace dep0::typecheck

