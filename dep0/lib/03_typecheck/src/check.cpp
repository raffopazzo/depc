#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

struct derivation_rules // TODO move to derivation.cpp and derivation_impl.hpp? (in private library)
{
    template <typename T>
    static derivation_t<T> make_derivation()
    {
        return derivation_t<T>{};
    }
};

template <typename... Args>
module_t make_legal_module(Args&&... args)
{
    return module_t{derivation_rules::make_derivation<module_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
type_def_t make_legal_type_def(Args&&... args)
{
    return type_def_t{derivation_rules::make_derivation<type_def_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
func_def_t make_legal_func_def(Args&&... args)
{
    return func_def_t{derivation_rules::make_derivation<func_def_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
func_call_t make_legal_func_call(type_t ret_type, Args&&... args)
{
    return func_call_t{derivation_rules::make_derivation<func_call_t>(), std::move(ret_type), std::forward<Args>(args)...};
}

template <typename... Args>
type_t make_legal_type(Args&&... args)
{
    return type_t{derivation_rules::make_derivation<type_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
body_t make_legal_body(Args&&... args)
{
    return body_t{derivation_rules::make_derivation<body_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
stmt_t make_legal_stmt(Args&&... args)
{
    return stmt_t{derivation_rules::make_derivation<stmt_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
expr_t make_legal_expr(sort_t sort, Args&&... args)
{
    return expr_t{derivation_rules::make_derivation<expr_t>(), std::move(sort), std::forward<Args>(args)...};
}

// forward declarations
static dep0::expected<func_call_t> type_assign_func_call(context_t const&, parser::func_call_t const&);
static expected<type_def_t> check(context_t&, parser::type_def_t const&);
static expected<func_def_t> check(context_t&, parser::func_def_t const&);
static expected<type_t> check(context_t const&, parser::type_t const&);
static expected<body_t> check(context_t const&, parser::body_t const&, type_t const& return_type);
static expected<stmt_t> check(context_t const&, parser::stmt_t const&, type_t const& return_type);
static expected<expr_t> check(context_t const&, parser::expr_t const&, type_t const& expected_type);
static expected<type_t> check(context_t const&, parser::expr_t const&, ast::typename_t const&);

static std::ostream& print_location(std::ostream& os, source_loc_t const& loc)
{
    return os << loc.line << ':' << loc.col; // TODO add filename once available;
}

expected<module_t> check(parser::module_t const& x)
{
    context_t ctx;
    // TODO maybe one day typedefs can depend on function definitions? if so we will need 2 passes
    auto type_defs = fmap_or_error(x.type_defs, [&] (parser::type_def_t const& t) { return check(ctx, t); });
    if (not type_defs)
        return std::move(type_defs.error());
    auto func_defs = fmap_or_error(x.func_defs, [&] (parser::func_def_t const& f) { return check(ctx, f); });
    if (not func_defs)
        return std::move(func_defs.error());
    return make_legal_module(std::move(*type_defs), std::move(*func_defs));
}

// implementations

dep0::expected<func_call_t> type_assign_func_call(context_t const& ctx, parser::func_call_t const& f)
{
    auto const error = [&] (std::vector<dep0::error_t> reasons)
    {
        std::ostringstream err;
        err << "invocation of function `" << f.name << "` does not typecheck";
        return dep0::error_t{err.str(), f.properties, std::move(reasons)};
    };
    auto const proto = ctx.find_proto(f.name);
    if (not proto)
        return error({dep0::error_t{"function prototype not found"}});
    if (proto->value.args.size() != f.args.size())
    {
        std::ostringstream err;
        err << "passed " << f.args.size() << " arguments but was expecting " << proto->value.args.size();
        return error({dep0::error_t{err.str()}});
    }
    auto proto_ = proto->value;
    std::vector<expr_t> args;
    std::vector<dep0::error_t> reasons;
    auto const subst = [] (type_t const& x, source_text const& name, type_t const& y)
    {
        if (auto const p = std::get_if<type_t::name_t>(&x.value))
            if (p->name == name)
                return y;
        return x;
    };
    for (auto const i: std::views::iota(0ul, f.args.size()))
        match(
            proto_.args[i].sort,
            [&] (type_t const& t)
            {
                if (auto expr = check(ctx, f.args[i], t))
                    args.push_back(std::move(*expr));
                else
                    reasons.push_back(std::move(expr.error()));
            },
            [&] (ast::typename_t const& t)
            {
                if (auto type = check(ctx, f.args[i], t))
                {
                    args.push_back(make_legal_expr(t, *type));
                    proto_.ret_type = subst(proto_.ret_type, proto_.args[i].name, *type);
                    for (auto const j: std::views::iota(i, f.args.size()))
                        match(
                            proto_.args[j].sort,
                            [&] (type_t const& t)
                            {
                                proto_.args[j].sort = subst(t, proto_.args[i].name, *type);
                            },
                            [&] (ast::typename_t const&) { /* nop */});
                }
                else
                    reasons.push_back(std::move(type.error()));
            });
    if (not reasons.empty())
        return error(std::move(reasons));
    return make_legal_func_call(proto_.ret_type, f.name, std::move(args));
}

expected<type_def_t> check(context_t& ctx, parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [&] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            auto const [it, inserted] =
                ctx.try_emplace_typedef(
                    x.name,
                    type_def.properties,
                    make_legal_type_def(type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value}));
            if (not inserted)
            {
                std::ostringstream err;
                print_location(err << "typedef `" << x.name << "` already defined at ", it->second.loc);
                return error_t::from_error(dep0::error_t{err.str(), type_def.properties}, ctx);
            }
            return it->second.value;
        });
}

template <typename BeforeBodyHook>
expected<expr_t::abs_t> check(context_t const& ctx, parser::expr_t::abs_t const& f, BeforeBodyHook&& hook)
{
    auto f_ctx = ctx.extend();
    auto args = fmap_or_error(
        f.args,
        [&] (parser::expr_t::abs_t::arg_t const& x) -> expected<expr_t::abs_t::arg_t>
        {
            auto ok =
                match(
                    x.sort,
                    [&] (parser::type_t const& parsed_ty)
                        -> expected<std::pair<context_t::args_iterator, bool>>
                    {
                        auto ty = check(f_ctx, parsed_ty);
                        if (not ty)
                            return std::move(ty.error());
                        return f_ctx.try_emplace_arg(x.name, x.loc, std::move(*ty), x.name, x.loc);
                    },
                    [&] (ast::typename_t)
                        -> expected<std::pair<context_t::args_iterator, bool>>
                    {
                        return f_ctx.try_emplace_arg(x.name, x.loc, ast::typename_t{}, x.name, x.loc);
                    });
            if (not ok)
                return std::move(ok.error());
            auto const [it, inserted] = *ok;
            if (not inserted)
            {
                std::ostringstream err;
                print_location(err << "function argument `" << x.name << "` already defined at ", it->second.loc);
                pretty_print(err << " as `", it->second.value.sort) << '`';
                return error_t::from_error(dep0::error_t{err.str(), x.loc}, f_ctx);
            }
            return it->second.value;
        });
    if (not args)
        return std::move(args.error());
    auto ret_type = check(f_ctx, f.ret_type);
    if (not ret_type)
        return std::move(ret_type.error());
    if (auto ok = hook(*ret_type, *args); not ok)
        return std::move(ok.error());
    auto body = check(f_ctx, f.body, *ret_type);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
        return error_t::from_error(dep0::error_t{"missing return statement"}, f_ctx);
    return expr_t::abs_t{std::move(*args), std::move(*ret_type), std::move(*body)};
}

expected<func_def_t> check(context_t& ctx, parser::func_def_t const& f)
{
    // top level function definitions can be recursive;
    // to make recursion work we need to add the function prototype to the context
    // before we check the body, so pass in a `BeforeBodyHook`
    auto const try_add_proto =
        [&] (type_t const& ret_type, std::vector<expr_t::abs_t::arg_t> const& args)
            -> expected<std::true_type>
        {
            auto const [it, inserted] = ctx.try_emplace_proto(f.name, f.properties, ret_type, args);
            if (not inserted)
            {
                std::ostringstream err;
                print_location(err << "function `" << f.name << "` already defined at ", it->second.loc);
                pretty_print(err << " with type `", it->second.value) << '`';
                return error_t::from_error(dep0::error_t{err.str(), f.properties}, ctx);
            }
            return std::true_type{};
        };
    if (auto abs = check(ctx, f.value, try_add_proto))
        return make_legal_func_def(f.name, std::move(*abs));
    else
    {
        if (not abs.error().location)
            abs.error().location = f.properties;
        return std::move(abs.error());
    }
}

expected<type_t> check(context_t const& ctx, parser::type_t const& t)
{
    auto const loc = t.properties;
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
        [&] (parser::type_t::name_t const& name) -> expected<type_t>
        {
            if (ctx.find_typedef(name.name)) // TODO `typename` args should shadow typedefs
                return make_legal_type(type_t::name_t{name.name});
            if (auto arg = ctx.find_arg(name.name))
                if (std::holds_alternative<ast::typename_t>(arg->value.sort))
                    return make_legal_type(type_t::name_t{name.name});
            std::ostringstream err;
            err << "unknown type `" << name.name << '`';
            return error_t::from_error(dep0::error_t{err.str(), t.properties}, ctx);
        });
}

expected<body_t> check(context_t const& ctx, parser::body_t const& x, type_t const& return_type)
{
    if (auto stmts = fmap_or_error(x.stmts, [&] (parser::stmt_t const& s) { return check(ctx, s, return_type); }))
        return make_legal_body(std::move(*stmts));
    else
        return std::move(stmts.error());
}

expected<stmt_t> check(context_t const& ctx, parser::stmt_t const& s, type_t const& return_type)
{
    return match(
        s.value,
        [&] (parser::func_call_t const& x) -> expected<stmt_t>
        {
            if (auto f = type_assign_func_call(ctx, x))
                return make_legal_stmt(std::move(*f));
            else
                return error_t::from_error(std::move(f.error()), ctx);
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            // TODO can we do lazy bind here? something like `expected::bind([] {a}, [] {b}, [] {c}, [] (a,b,c) {x})`
            auto cond = check(ctx, x.cond, make_legal_type(type_t::bool_t{}));
            if (not cond)
                return std::move(cond.error());
            auto true_branch = check(ctx, x.true_branch, return_type);
            if (not true_branch)
                return std::move(true_branch.error());
            std::optional<body_t> false_branch;
            if (x.false_branch)
            {
                if (auto f = check(ctx, *x.false_branch, return_type))
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
                if (std::holds_alternative<type_t::unit_t>(return_type.value))
                    return make_legal_stmt(stmt_t::return_t{});
                std::ostringstream err;
                pretty_print(err << "expecting expression of type `", return_type) << '`';
                return error_t::from_error(dep0::error_t{err.str(), s.properties}, ctx, return_type);
            }
            else if (auto expr = check(ctx, *x.expr, return_type))
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
        [&] (type_t::name_t const& name) -> expected<expr_t>
        {
            auto t = ctx.find_typedef(name.name);
            if (not t)
            {
                std::ostringstream err;
                err << "could not find typedef for name `" << name.name << "` even though the typeref typechecked. ";
                err << "This is a compiler bug, please report it!";
                return error(err.str());
            }
            return match(
                t->value.value,
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

expected<expr_t> check(context_t const& ctx, parser::expr_t const& x, type_t const& expected_type)
{
    auto const loc = x.properties;
    auto const type_error = [&] (sort_t const& actual_ty)
    {
        std::ostringstream err;
        pretty_print(err << "expression of type `", actual_ty) << '`';
        pretty_print(err << " does not typecheck with expected type `", expected_type) << '`';
        return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, expected_type);
    };
    return match(
        x.value,
        [&] (parser::func_call_t const& x) -> expected<expr_t>
        {
            if (auto f = type_assign_func_call(ctx, x))
            {
                if (f->properties.ret_type == expected_type)
                    return make_legal_expr(expected_type, std::move(*f));
                else
                    return type_error(f->properties.ret_type);
            }
            else
                return error_t::from_error(std::move(f.error()), ctx, expected_type);
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
                    auto lhs = check(ctx, x.lhs.get(), expected_type);
                    if (not lhs) return std::move(lhs.error());
                    auto rhs = check(ctx, x.rhs.get(), expected_type);
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
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            if (not std::holds_alternative<type_t::bool_t>(expected_type.value))
                return type_error(make_legal_type(type_t::bool_t{}));
            return make_legal_expr(expected_type, expr_t::boolean_constant_t{x.value});
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return check_numeric_expr(ctx, x, loc, expected_type);
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto arg = ctx.find_arg(x.name);
            if (not arg)
            {
                std::ostringstream err;
                err << "unknown variable `" << x.name << "` in current context";
                return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, expected_type);
            }
            if (not match(
                arg->value.sort,
                [&] (type_t const& t) { return t == expected_type; },
                [] (auto const&) { return false; }))
            {
                return type_error(arg->value.sort);
            }
            return make_legal_expr(expected_type, expr_t::var_t{x.name});
        },
        [&] (parser::expr_t::abs_t const& f) -> expected<expr_t>
        {
            // abstraction expressions are anonymous, so recursion is not possible because we do not have
            // a name to register the prototype in the current context; so we pass a no-op lambda;
            auto abs = check(ctx, f, [] (auto&&...) -> expected<std::true_type> { return std::true_type{}; });
            if (not abs)
            {
                if (not abs.error().location)
                    abs.error().location = loc;
                return std::move(abs.error());
            }
            // TODO check expected_type
            return error_t::from_error(
                dep0::error_t{"Abstraction expressions not yet supported", loc},
                ctx,
                expected_type);
        },
        [&] (parser::type_t const& x) -> expected<expr_t>
        {
            std::ostringstream err;
            pretty_print(
                err << "the expression `" << x.properties.txt << "` yields a type not a value of type `",
                expected_type) << '`';
            return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, expected_type);
        });
}

expected<type_t> check(context_t const& ctx, parser::expr_t const& x, ast::typename_t const& expected_type)
{
    auto const error = [&] (std::string err)
    {
        return error_t::from_error(dep0::error_t{std::move(err), x.properties}, ctx, expected_type);
    };
    return match(
        x.value,
        [&] (parser::func_call_t const& x) -> expected<type_t>
        {
            // in lambda-2 a function cannot return a type, so we could bail out immediately;
            // but we want to give nice error messages, so just perform typechecking as usual;
            // we'll need to do this in lambda-omega anyway, so might as well do it now
            if (auto f = type_assign_func_call(ctx, x))
            {
                std::ostringstream err;
                pretty_print(
                    err << "type mismatch between invocation of function `" << f->name << "` with return type `",
                    f->properties.ret_type
                ) << "` and `typename`";
                return error(err.str());
            }
            else
                return error_t::from_error(std::move(f.error()), ctx, expected_type);
        },
        [&] (parser::expr_t::arith_expr_t const& x) -> expected<type_t>
        {
            return error("type mismatch between arithmetic expression and `typename`");
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<type_t>
        {
            return error("type mismatch between boolean constant and `typename`");
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<type_t>
        {
            return error("type mismatch between numeric constant and `typename`");
        },
        [&] (parser::expr_t::var_t const& x) -> expected<type_t>
        {
            return error("not implemented");
        },
        [&] (parser::expr_t::abs_t const& x) -> expected<type_t>
        {
            return error("type mismatch between abstraction expression and `typename`");
        },
        [&] (parser::type_t const& x) -> expected<type_t>
        {
            return check(ctx, x);
        });
}

} // namespace dep0::typecheck
