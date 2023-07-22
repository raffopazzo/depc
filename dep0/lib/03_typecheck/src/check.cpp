#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include "dep0/digit_separator.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

// TODO: improve... maybe define `typecheck::context_t` (with extension operations) with typedefs and `tt::context_t`
using typedefs_t = std::map<source_text, type_def_t>;

// forward declarations
static expected<type_def_t> check(tt::context_t, parser::type_def_t const&);
static expected<func_def_t> check(tt::context_t, typedefs_t const&, parser::func_def_t const&);
static expected<type_t> check(tt::context_t, typedefs_t const&, parser::type_t const&);
static expected<body_t> check(tt::context_t, typedefs_t const&, parser::body_t const&, type_t const& return_type);
static expected<stmt_t> check(tt::context_t, typedefs_t const&, parser::stmt_t const&, type_t const& return_type);
static expected<expr_t> check(tt::context_t, typedefs_t const&, parser::expr_t const&, type_t const& expected_type);

expected<module_t> check(parser::module_t const& x)
{
    tt::context_t ctx;
    typedefs_t typedefs;
    std::vector<type_def_t> type_defs;
    std::vector<func_def_t> func_defs;
    type_defs.reserve(x.type_defs.size());
    func_defs.reserve(x.func_defs.size());
    for (auto const& t: x.type_defs)
        if (auto res = check(ctx, t))
        {
            auto const name = std::visit([] (auto const& x) { return x.name; }, t.value);
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
        if (auto res = check(ctx, typedefs, f))
        {
            if (auto ok = ctx.add(tt::term_t::var_t(f.name), tt::type_of(res->type.properties.derivation));
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
    struct visitor
    {
        expected<type_def_t> operator()(parser::type_def_t::integer_t const& x) const
        {
            return type_def_t{legal_type_def_t{}, type_def_t::integer_t{x.name, x.sign, x.width, x.max_abs_value}};
        }
    };
    return std::visit(visitor{}, type_def.value);
}

expected<func_def_t> check(tt::context_t ctx, typedefs_t const& typedefs, parser::func_def_t const& f)
{
    ctx = ctx.extend();
    auto ret_type = check(ctx, typedefs, f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    if (auto ok = ctx.add(tt::term_t::var_t(f.name), tt::type_of(ret_type->properties.derivation));
        not ok)
    {
        return error_t::from_error(ok.error(), ctx, tt::type_of(ret_type->properties.derivation));
    }
    auto body = check(ctx, typedefs, f.body, *ret_type);
    if (not body)
        return std::move(body.error());
    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
    {
        std::ostringstream err;
        err << "In function `" << f.name << "` missing return statement";
        return error_t::from_error(dep0::error_t{err.str(), f.properties}, ctx, tt::type_of(ret_type->properties.derivation));
    }
    return func_def_t{legal_func_def_t{}, std::move(*ret_type), f.name, std::move(*body)};
}

expected<type_t> check(tt::context_t ctx, typedefs_t const& typedefs, parser::type_t const& t)
{
    struct visitor
    {
        tt::context_t const& ctx;
        typedefs_t const& typedefs;
        source_loc_t const& loc;
        expected<type_t> operator()(parser::type_t::bool_t const&) const
        {
            return type_t{tt::derivation_t::bool_t(), type_t::bool_t{}};
        }
        expected<type_t> operator()(parser::type_t::unit_t const&) const
        {
            return type_t{tt::derivation_t::unit_t(), type_t::unit_t{}};
        }
        expected<type_t> operator()(parser::type_t::i8_t const&) const
        {
            return type_t{tt::derivation_t::i8_t(), type_t::i8_t{}};
        }
        expected<type_t> operator()(parser::type_t::i16_t const&) const
        {
            return type_t{tt::derivation_t::i16_t(), type_t::i16_t{}};
        }
        expected<type_t> operator()(parser::type_t::i32_t const&) const
        {
            return type_t{tt::derivation_t::i32_t(), type_t::i32_t{}};
        }
        expected<type_t> operator()(parser::type_t::i64_t const&) const
        {
            return type_t{tt::derivation_t::i64_t(), type_t::i64_t{}};
        }
        expected<type_t> operator()(parser::type_t::u8_t const&) const
        {
            return type_t{tt::derivation_t::u8_t(), type_t::u8_t{}};
        }
        expected<type_t> operator()(parser::type_t::u16_t const&) const
        {
            return type_t{tt::derivation_t::u16_t(), type_t::u16_t{}};
        }
        expected<type_t> operator()(parser::type_t::u32_t const&) const
        {
            return type_t{tt::derivation_t::u32_t(), type_t::u32_t{}};
        }
        expected<type_t> operator()(parser::type_t::u64_t const&) const
        {
            return type_t{tt::derivation_t::u64_t(), type_t::u64_t{}};
        }
        expected<type_t> operator()(parser::type_t::name_t const& name) const
        {
            if (auto d = tt::type_assign(ctx, tt::type_t::var_t(name.name)))
                return type_t{std::move(*d), type_t::name_t{name.name}};
            else
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx);
            }
        }
    };
    return std::visit(visitor{ctx, typedefs, t.properties}, t.value);
}

expected<body_t> check(tt::context_t ctx, typedefs_t const& typedefs, parser::body_t const& x, type_t const& return_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        if (auto stmt = check(ctx, typedefs, s, return_type))
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return body_t{legal_body_t{}, std::move(stmts)};
}

expected<stmt_t> check(tt::context_t ctx, typedefs_t const& typedefs, parser::stmt_t const& s, type_t const& return_type)
{
    struct visitor
    {
        tt::context_t const& ctx;
        typedefs_t const& typedefs;
        type_t const& return_type;
        source_loc_t const& loc;
        expected<stmt_t> operator()(parser::stmt_t::fun_call_t const& x)
        {
            if (auto d = tt::type_assign(ctx, tt::term_t::var(x.name)))
                return stmt_t{legal_stmt_t{}, stmt_t::fun_call_t{x.name}};
            else
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx);
            }
        }
        expected<stmt_t> operator()(parser::stmt_t::if_else_t const& x)
        {
            auto cond = check(ctx, typedefs, x.cond, type_t{tt::derivation_t::bool_t(), type_t::bool_t{}});
            if (not cond)
                return std::move(cond.error());
            auto true_branch = check(ctx, typedefs, x.true_branch, return_type);
            if (not true_branch)
                return std::move(true_branch.error());
            std::optional<body_t> false_branch;
            if (x.false_branch)
            {
                if (auto f = check(ctx, typedefs, *x.false_branch, return_type))
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
        }
        expected<stmt_t> operator()(parser::stmt_t::return_t const& x)
        {
            if (not x.expr)
            {
                if (std::holds_alternative<type_t::unit_t>(return_type.value))
                    return expected<stmt_t>{std::in_place, legal_stmt_t{}, stmt_t::return_t{}};
                std::ostringstream err;
                tt::pretty_print(
                    err << "Expecting expression of type `",
                    tt::type_of(return_type.properties.derivation)
                ) << '`';
                return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, tt::type_of(return_type.properties.derivation));
            }
            else if (auto expr = check(ctx, typedefs, *x.expr, return_type))
                return stmt_t{legal_stmt_t{}, stmt_t::return_t{std::move(*expr)}};
            else
                return std::move(expr.error());
        }
    };
    return std::visit(visitor{ctx, typedefs, return_type, s.properties}, s.value);
}

static expected<expr_t> check_numeric_expr(
    tt::context_t const& ctx,
    typedefs_t const& typedefs,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    type_t const& expected_type)
{
    struct visitor_base
    {
        tt::context_t const& ctx;
        parser::expr_t::numeric_constant_t const& x;
        source_loc_t const& loc;
        type_t const& expected_type;

        expected<expr_t> error(std::string msg) const
        {
            auto const tgt = tt::type_of(expected_type.properties.derivation);
            return error_t::from_error(dep0::error_t{std::move(msg), loc}, ctx, tgt);
        }

        expected<expr_t> check_integer(
            std::string_view const type_name,
            std::string_view const sign_chars,
            std::string_view const max_abs_value) const
        {
            std::optional<std::string> without_separator; // keeps alive the string pointed to by `number`
            std::string_view number;
            if (contains_digit_separator(x.number.view()))
                number = without_separator.emplace(remove_digit_separator(x.number.view()));
            else
                number = x.number.view();
            skip_zero_or_one(number, sign_chars);
            skip_any(number, "0");
            if (number.starts_with('-'))
                // if we were checking for signed integer we would skip the `-`, so we must be checking for unsigned
                // TODO should we allow `-0` though? not sure...
                return error("Invalid negative constant for unsigned integer");
            if (bigger_than(number, max_abs_value))
            {
                std::ostringstream err;
                err << "Numeric constant does not fit inside `" << type_name << '`';
                return error(err.str());
            }
            return expr_t{expected_type, expr_t::numeric_constant_t{x.number}};
        }

    private:
        static void skip_any(std::string_view& s, std::string_view const chars)
        {
            for (auto const c: chars)
                if (s.starts_with(c))
                    s.remove_prefix(1);
        }
        static void skip_zero_or_one(std::string_view& s, std::string_view const chars)
        {
            for (auto const c: chars)
                if (s.starts_with(c))
                {
                    s.remove_prefix(1);
                    return;
                }
        }
        // assuming `a` and `b` represent positive integer numbers without leading 0s, returns whether `a > b`
        static bool bigger_than(std::string_view const a, std::string_view const b)
        {
            if (a.size() > b.size())
                return true;
            if (a.size() == b.size())
                for (auto const i: std::views::iota(0ul, a.size()))
                    if (a[i] != b[i]) return a[i] > b[i];
            return false;
        }
    };
    struct visitor : visitor_base
    {
        typedefs_t const& typedefs;

        expected<expr_t> operator()(type_t::bool_t const&) const
        {
            return error("Type mismatch between numeric constant and `bool`");
        }
        expected<expr_t> operator()(type_t::unit_t const&) const
        {
            return error("Type mismatch between numeric constant and `unit_t`");
        }

        expected<expr_t> operator()(type_t::i8_t const&) const { return check_integer("i8_t", "+-", "127"); }
        expected<expr_t> operator()(type_t::i16_t const&) const { return check_integer("i16_t", "+-", "32767"); }
        expected<expr_t> operator()(type_t::i32_t const&) const { return check_integer("i32_t", "+-", "2147483647"); }
        expected<expr_t> operator()(type_t::i64_t const&) const { return check_integer("i64_t", "+-", "9223372036854775807"); }
        expected<expr_t> operator()(type_t::u8_t const&) const { return check_integer("u8_t", "+", "255"); }
        expected<expr_t> operator()(type_t::u16_t const&) const { return check_integer("u16_t", "+", "65535"); }
        expected<expr_t> operator()(type_t::u32_t const&) const { return check_integer("u32_t", "+", "4294967295"); }
        expected<expr_t> operator()(type_t::u64_t const&) const { return check_integer("u64_t", "+", "18446744073709551615"); }
        expected<expr_t> operator()(type_t::name_t const& name) const
        {
            auto const it = typedefs.find(name.name);
            if (it == typedefs.end())
            {
                std::ostringstream err;
                err << "Could not find typedef for name `" << name.name << "` even though the typeref typechecked. ";
                err << "This is a compiler bug, please report it!";
                return error(err.str());
            }
            struct visitor : visitor_base
            {
                expected<expr_t> operator()(type_def_t::integer_t const& integer) const
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
                            err << "Uknown width for signed integer `static_cast<int>(integer.width)="
                                << static_cast<int>(integer.width) << "`. ";
                            err << "This is a compiler bug, please report it!";
                            return error(err.str());
                        }
                        return check_integer(integer.name.view(), "+-", max_abs);
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
                            err << "Uknown width for unsigned integer `static_cast<int>(integer.width)="
                                << static_cast<int>(integer.width) << "`. ";
                            err << "This is a compiler bug, please report it!";
                            return error(err.str());
                        }
                        return check_integer(integer.name.view(), "+", max_abs);
                    }
                }
            };
            return std::visit(visitor{ctx, x, loc, expected_type}, it->second.value);
        }
    };
    return std::visit(visitor{ctx, x, loc, expected_type, typedefs}, expected_type.value);
}

expected<expr_t> check(
    tt::context_t ctx,
    typedefs_t const& typedefs,
    parser::expr_t const& x,
    type_t const& expected_type)
{
    struct visitor
    {
        tt::context_t const& ctx;
        typedefs_t const& typedefs;
        source_loc_t const& loc;
        type_t const& expected_type;
        tt::type_t const& tgt;
        expected<expr_t> operator()(parser::expr_t::fun_call_t const& x) const
        {
            auto d = tt::type_assign(ctx, tt::term_t::var(x.name));
            if (not d)
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx, tgt);
            }
            if (tt::type_of(*d) != tgt)
                return type_error(tt::type_of(*d));
            return expr_t{legal_expr_t{expected_type}, expr_t::fun_call_t{x.name}};
        }

        expected<expr_t> operator()(parser::expr_t::boolean_constant_t const& x) const
        {
            if (tgt != tt::type_of(tt::derivation_t::bool_t()))
                return type_error(tt::type_of(tt::derivation_t::bool_t()));
            return expr_t{legal_expr_t{expected_type}, expr_t::boolean_constant_t{x.value}};
        }

        expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x) const
        {
            return check_numeric_expr(ctx, typedefs, x, loc, expected_type);
        }

    private:
        error_t type_error(tt::type_t const& actual_ty) const
        {
            std::ostringstream err;
            tt::pretty_print(err << "Expression of type `", actual_ty) << '`';
            tt::pretty_print(err << " does not typecheck with expected type `", tgt) << '`';
            return error_t::from_error(dep0::error_t{err.str(), loc}, ctx, tgt);
        }
    };
    return std::visit(
        visitor{ctx, typedefs, x.properties, expected_type, tt::type_of(expected_type.properties.derivation)},
        x.value);
}

} // namespace dep0::typecheck

