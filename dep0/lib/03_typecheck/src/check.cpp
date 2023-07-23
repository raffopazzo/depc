#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include "dep0/digit_separator.hpp"
#include "dep0/match.hpp"

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
expr_t make_legal_expr(type_t expected_type, Args&&... args)
{
    return expr_t{derivation_rules::make_derivation<expr_t>(), std::move(expected_type), std::forward<Args>(args)...};
}

struct func_proto_t // this should really be in the AST
{
    type_t ret_type;
    std::vector<func_def_t::arg_t> args;
};

// TODO: improve... maybe define `typecheck::context_t` (with extension operations) with typedefs and `tt::context_t`
using typedefs_t = std::map<source_text, type_def_t>;
using fnprotos_t = std::map<source_text, func_proto_t>; // yeah both of these need to be scoped...
using argsmap_t = std::map<source_text, func_def_t::arg_t>; // also in context, local to function definition

// forward declarations
static expected<type_def_t> check(parser::type_def_t const&);
static expected<func_def_t> check(fnprotos_t&, typedefs_t const&, parser::func_def_t const&);
static expected<func_def_t::arg_t> check(typedefs_t const&, parser::func_def_t::arg_t const&);
static expected<type_t> check(typedefs_t const&, parser::type_t const&);
static expected<body_t> check(fnprotos_t const&, typedefs_t const&, argsmap_t const&, parser::body_t const&, type_t const& return_type);
static expected<stmt_t> check(fnprotos_t const&, typedefs_t const&, argsmap_t const&, parser::stmt_t const&, type_t const& return_type);
static expected<expr_t> check(fnprotos_t const&, typedefs_t const&, argsmap_t const&, parser::expr_t const&, type_t const& expected_type);

expected<module_t> check(parser::module_t const& x)
{
    typedefs_t typedefs;
    fnprotos_t protos;
    std::vector<type_def_t> type_defs;
    std::vector<func_def_t> func_defs;
    type_defs.reserve(x.type_defs.size());
    func_defs.reserve(x.func_defs.size());

    for (auto const& t: x.type_defs)
        if (auto res = check(t))
        {
            auto const name = match(t.value, [] (auto const& x) { return x.name; });
            auto const [it, inserted] = typedefs.try_emplace(name, *res);
            if (not inserted)
            {
                std::ostringstream err;
                err << "typedef `" << name << "` already defined in current context"; // TODO also say where
                return error_t::from_error(dep0::error_t{err.str(), t.properties}, /*ctx=*/{});
            }
            type_defs.push_back(std::move(*res)); // TODO introduce `fmap_or_error()`
        }
        else
            return std::move(res.error());
    for (auto const& f: x.func_defs)
        if (auto res = check(protos, typedefs, f))
            func_defs.push_back(std::move(*res));
        else
            return std::move(res.error());
    return make_legal_module(std::move(type_defs), std::move(func_defs));
}

// implementations
expected<type_def_t> check(parser::type_def_t const& type_def)
{
    return match(
        type_def.value,
        [] (parser::type_def_t::integer_t const& x) -> expected<type_def_t>
        {
            return make_legal_type_def(type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value});
        });
}

expected<func_def_t> check(fnprotos_t& protos, typedefs_t const& typedefs, parser::func_def_t const& f)
{
    auto ret_type = check(typedefs, f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    std::vector<func_def_t::arg_t> args;
    argsmap_t args_map; // TODO move in scoped context
    for (auto const& arg: f.args)
        if (auto type = check(typedefs, arg.type))
        {
            auto const [it, inserted] = args_map.try_emplace(arg.name, std::move(*type), arg.name);
            if (not inserted)
            {
                std::ostringstream err;
                err << "found duplicate argument name `" << arg.name << '`'; // TODO say where, and previous type too?
                return error_t::from_error(dep0::error_t{err.str(), f.properties}, /*ctx=*/{});
            }
            args.push_back(it->second); // TODO `fmap_or_error()`
        }
        else
            return std::move(type.error());
    auto const [it, inserted] = protos.try_emplace(f.name, *ret_type, args); // TODO we could bail out quicker for duplicates...
    if (not inserted)
    {
        std::ostringstream err;
        err << "function `" << f.name << "` already defined"; // TODO should also say where
        return error_t::from_error(dep0::error_t{err.str(), f.properties}, /*ctx=*/{});
    }
    auto body = check(protos, typedefs, args_map, f.body, *ret_type);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
    {
        std::ostringstream err;
        err << "in function `" << f.name << "` missing return statement";
        return error_t::from_error(dep0::error_t{err.str(), f.properties}, /*ctx=*/{});
    }
    return make_legal_func_def(std::move(*ret_type), f.name, std::move(args), std::move(*body));
}

expected<type_t> check(typedefs_t const& typedefs, parser::type_t const& t)
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
            if (typedefs.contains(name.name))
                return make_legal_type(type_t::name_t{name.name});
            else
            {
                std::ostringstream err;
                err << "unknown type `" << name.name << '`';
                return error_t::from_error(dep0::error_t{err.str(), t.properties}, /*ctx=*/{});
            }
        });
}

expected<body_t> check(
    fnprotos_t const& protos,
    typedefs_t const& typedefs,
    argsmap_t const& args_map,
    parser::body_t const& x,
    type_t const& return_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        if (auto stmt = check(protos, typedefs, args_map, s, return_type))
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return make_legal_body(std::move(stmts));
}

expected<stmt_t> check(
    fnprotos_t const& protos,
    typedefs_t const& typedefs,
    argsmap_t const& args_map,
    parser::stmt_t const& s,
    type_t const& return_type)
{
    auto const loc = s.properties;
    return match(
        s.value,
        [&] (parser::stmt_t::fun_call_t const& x) -> expected<stmt_t>
        {
            if (protos.contains(x.name))
                return make_legal_stmt(stmt_t::fun_call_t{x.name});
            else
            {
                std::ostringstream err;
                err << "unknown function `" << x.name << '`';
                return error_t::from_error(dep0::error_t{err.str(), loc}/*, ctx, tgt */);
            }
        },
        [&] (parser::stmt_t::if_else_t const& x) -> expected<stmt_t>
        {
            auto cond = check(protos, typedefs, args_map, x.cond, make_legal_type(type_t::bool_t{}));
            if (not cond)
                return std::move(cond.error());
            auto true_branch = check(protos, typedefs, args_map, x.true_branch, return_type);
            if (not true_branch)
                return std::move(true_branch.error());
            std::optional<body_t> false_branch;
            if (x.false_branch)
            {
                if (auto f = check(protos, typedefs, args_map, *x.false_branch, return_type))
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
                err << "expecting expression of type `" << "TODO" << '`'; // TODO print expcted type
                return error_t::from_error(dep0::error_t{err.str(), loc}/*, ctx, tgt */);
            }
            else if (auto expr = check(protos, typedefs, args_map, *x.expr, return_type))
                return make_legal_stmt(stmt_t::return_t{std::move(*expr)});
            else
                return std::move(expr.error());
        });
}

static expected<expr_t> check_numeric_expr(
    typedefs_t const& typedefs,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    type_t const& expected_type)
{
    auto const error = [&] (std::string msg) -> expected<expr_t>
    {
//      auto const tgt = tt::type_of(expected_type.properties.derivation);
        return error_t::from_error(dep0::error_t{std::move(msg), loc}/*, ctx, tgt */);
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
        return make_legal_expr(expected_type, expr_t::numeric_constant_t{x.number});
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

expected<expr_t> check(
    fnprotos_t const& protos,
    typedefs_t const& typedefs,
    argsmap_t const& args_map,
    parser::expr_t const& x,
    type_t const& expected_type)
{
    auto const loc = x.properties;
//  auto const tgt = tt::type_of(expected_type.properties.derivation);
    auto const type_error = [&] (type_t const& actual_ty)
    {
        std::ostringstream err;
        err << "expression of type `" << "TODO" << '`'; // TODO print types
        err << " does not typecheck with expected type `" << "TODO" << '`';
//      tt::pretty_print(err << "expression of type `", actual_ty) << '`';
//      tt::pretty_print(err << " does not typecheck with expected type `", tgt) << '`';
        return error_t::from_error(dep0::error_t{err.str(), loc}/*, ctx, tgt */);
    };
    return match(
        x.value,
        [&] (parser::expr_t::fun_call_t const& x) -> expected<expr_t>
        {
            auto const error = [&] (std::vector<dep0::error_t> reasons)
            {
                std::ostringstream err;
                err << "invocation of function `" << x.name << "` does not typecheck";
                return error_t::from_error(dep0::error_t{err.str(), loc, std::move(reasons)}/*, ctx, tgt */);
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
                if (auto expr = check(protos, typedefs, args_map, x.args[i], proto_it->second.args[i].type))
                    args.push_back(std::move(*expr));
                else
                    reasons.push_back(std::move(expr.error()));
            if (not reasons.empty())
                return error(std::move(reasons));
            if (proto_it->second.ret_type != expected_type)
                return type_error(proto_it->second.ret_type);
            return make_legal_expr(expected_type, expr_t::fun_call_t{x.name, std::move(args)});
        },
        [&] (parser::expr_t::boolean_constant_t const& x) -> expected<expr_t>
        {
            if (not std::holds_alternative<type_t::bool_t>(expected_type.value))
                return type_error(make_legal_type(type_t::bool_t{}));
            return make_legal_expr(expected_type, expr_t::boolean_constant_t{x.value});
        },
        [&] (parser::expr_t::numeric_constant_t const& x) -> expected<expr_t>
        {
            return check_numeric_expr(typedefs, x, loc, expected_type);
        },
        [&] (parser::expr_t::var_t const& x) -> expected<expr_t>
        {
            auto const it = args_map.find(x.name);
            if (it == args_map.end())
            {
                std::ostringstream err;
                err << "unknown variable `" << x.name << "` in current context";
                return error_t::from_error(dep0::error_t{err.str(), loc}/*, ctx, tgt */);
            }
            if (it->second.type != expected_type)
                return type_error(it->second.type);
            return make_legal_expr(expected_type, expr_t::var_t{x.name});
        });
}

} // namespace dep0::typecheck

