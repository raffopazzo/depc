#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

static expected<func_def_t> check(tt::context_t, parser::func_def_t const&);
static expected<type_t> check(tt::context_t, parser::type_t const&);
static expected<body_t> check(tt::context_t, parser::body_t const&, type_t const& return_type);
static expected<stmt_t> check(tt::context_t, parser::stmt_t const&, type_t const& return_type);
static expected<expr_t> check(tt::context_t, parser::expr_t const&, type_t const& expected_type);

expected<module_t> check(parser::module_t const& x)
{
    tt::context_t ctx;
    std::vector<func_def_t> func_defs;
    func_defs.reserve(x.func_defs.size());
    for (auto const& f: x.func_defs)
        if (auto res = check(ctx, f))
        {
            if (auto ok = ctx.add(tt::term_t::var_t(f.name), tt::type_of(res->type.properties.derivation));
                not ok)
            {
                return error_t::from_error(ok.error(), ctx);
            }
            func_defs.push_back(std::move(*res));
        }
        else
            return std::move(res.error());
    return module_t{legal_module_t{}, std::move(func_defs)};
}

expected<func_def_t> check(tt::context_t ctx, parser::func_def_t const& f)
{
    ctx = ctx.extend();
    auto ret_type = check(ctx, f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    if (auto ok = ctx.add(tt::term_t::var_t(f.name), tt::type_of(ret_type->properties.derivation));
        not ok)
    {
        return error_t::from_error(ok.error(), ctx, tt::type_of(ret_type->properties.derivation));
    }
    auto body = check(ctx, f.body, *ret_type);
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

expected<type_t> check(tt::context_t ctx, parser::type_t const& t)
{
    struct visitor
    {
        type_t operator()(parser::type_t::bool_t const&) const
        {
            return type_t{tt::derivation_t::bool_t(), type_t::bool_t{}};
        }
        type_t operator()(parser::type_t::unit_t const&) const
        {
            return type_t{tt::derivation_t::unit_t(), type_t::unit_t{}};
        }
        type_t operator()(parser::type_t::i8_t const&) const
        {
            return type_t{tt::derivation_t::i8_t(), type_t::i8_t{}};
        }
        type_t operator()(parser::type_t::i16_t const&) const
        {
            return type_t{tt::derivation_t::i16_t(), type_t::i16_t{}};
        }
        type_t operator()(parser::type_t::i32_t const&) const
        {
            return type_t{tt::derivation_t::i32_t(), type_t::i32_t{}};
        }
        type_t operator()(parser::type_t::i64_t const&) const
        {
            return type_t{tt::derivation_t::i64_t(), type_t::i64_t{}};
        }
        type_t operator()(parser::type_t::u8_t const&) const
        {
            return type_t{tt::derivation_t::u8_t(), type_t::u8_t{}};
        }
        type_t operator()(parser::type_t::u16_t const&) const
        {
            return type_t{tt::derivation_t::u16_t(), type_t::u16_t{}};
        }
        type_t operator()(parser::type_t::u32_t const&) const
        {
            return type_t{tt::derivation_t::u32_t(), type_t::u32_t{}};
        }
        type_t operator()(parser::type_t::u64_t const&) const
        {
            return type_t{tt::derivation_t::u64_t(), type_t::u64_t{}};
        }
    };
    return expected<type_t>{std::visit(visitor{}, t.value)};
}

expected<body_t> check(tt::context_t ctx, parser::body_t const& x, type_t const& return_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        if (auto stmt = check(ctx, s, return_type))
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return body_t{legal_body_t{}, std::move(stmts)};
}

expected<stmt_t> check(tt::context_t ctx, parser::stmt_t const& s, type_t const& return_type)
{
    struct visitor
    {
        tt::context_t const& ctx;
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
            auto cond = check(ctx, x.cond, type_t{tt::derivation_t::bool_t(), type_t::bool_t{}});
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
            else if (auto expr = check(ctx, *x.expr, return_type))
                return stmt_t{legal_stmt_t{}, stmt_t::return_t{std::move(*expr)}};
            else
                return std::move(expr.error());
        }
    };
    return std::visit(visitor{ctx, return_type, s.properties}, s.value);
}

static expected<expr_t> check_numeric_expr(
    tt::context_t const& ctx,
    parser::expr_t::numeric_constant_t const& x,
    source_loc_t const& loc,
    type_t const& expected_type)
{
    struct visitor
    {
        tt::context_t const& ctx;
        parser::expr_t::numeric_constant_t const& x;
        source_loc_t const& loc;
        type_t const& expected_type;
        expected<expr_t> operator()(type_t::bool_t const&) const
        {
            return error_t::from_error(dep0::error_t{"Type mismatch between numeric constant and `bool`", loc}, ctx);
        }
        expected<expr_t> operator()(type_t::unit_t const&) const
        {
            return error_t::from_error(dep0::error_t{"Type mismatch between numeric constant and `unit_t`", loc}, ctx);
        }

        expected<expr_t> operator()(type_t::i8_t const&) const { return check_integer("i8_t", "+-", "127"); }
        expected<expr_t> operator()(type_t::i16_t const&) const { return check_integer("i16_t", "+-", "32767"); }
        expected<expr_t> operator()(type_t::i32_t const&) const { return check_integer("i32_t", "+-", "2147483647"); }
        expected<expr_t> operator()(type_t::i64_t const&) const { return check_integer("i64_t", "+-", "9223372036854775807"); }
        expected<expr_t> operator()(type_t::u8_t const&) const { return check_integer("u8_t", "+", "255"); }
        expected<expr_t> operator()(type_t::u16_t const&) const { return check_integer("u16_t", "+", "65535"); }
        expected<expr_t> operator()(type_t::u32_t const&) const { return check_integer("u32_t", "+", "4294967295"); }
        expected<expr_t> operator()(type_t::u64_t const&) const { return check_integer("u64_t", "+", "18446744073709551615"); }

    private:
        expected<expr_t> check_integer(
            std::string_view const type_name,
            std::string_view const sign_chars,
            std::string_view const max_abs_value) const
        {
            std::string_view number = x.number.view();
            skip_zero_or_one(number, sign_chars);
            skip_any(number, "0");
            if (bigger_than(number, max_abs_value))
            {
                std::ostringstream err;
                err << "Numeric constant does not fit inside `" << type_name << '`';
                return error_t::from_error(dep0::error_t{err.str(), loc}, ctx);
            }
            return expr_t{expected_type, expr_t::numeric_constant_t{x.number}};
        }

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
    return std::visit(visitor{ctx, x, loc, expected_type}, expected_type.value);
}

expected<expr_t> check(tt::context_t ctx, parser::expr_t const& x, type_t const& expected_type)
{
    struct visitor
    {
        tt::context_t const& ctx;
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
            return check_numeric_expr(ctx, x, loc, expected_type);
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
        visitor{ctx, x.properties, expected_type, tt::type_of(expected_type.properties.derivation)},
        x.value);
}

} // namespace dep0::typecheck

