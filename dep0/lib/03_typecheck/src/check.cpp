#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

static tt::derivation_t bool_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_bool()); }
static tt::derivation_t unit_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_unit()); }
static tt::derivation_t i8_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_i8()); }
static tt::derivation_t i16_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_i16()); }
static tt::derivation_t i32_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_i32()); }
static tt::derivation_t i64_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_i64()); }
static tt::derivation_t u8_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_u8()); }
static tt::derivation_t u16_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_u16()); }
static tt::derivation_t u32_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_u32()); }
static tt::derivation_t u64_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_u64()); }

static expected<expr_t> type_assign(tt::context_t ctx, parser::expr_t const& x)
{
    struct visitor
    {
        tt::context_t const& ctx;
        source_loc_t const& loc;

        expected<expr_t> operator()(parser::expr_t::fun_call_t const& x) const
        {
            if (auto d = tt::type_assign(ctx, tt::term_t::var(x.name)))
                return expr_t{std::move(*d), expr_t::fun_call_t{x.name}};
            else
            {
                d.error().location = loc;
                return error_t::from_error(std::move(d.error()), ctx);
            }
        }

        expected<expr_t> operator()(parser::expr_t::boolean_constant_t const& x) const
        {
            return expr_t{bool_derivation(), expr_t::boolean_constant_t{x.value}};
        }

        expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x) const
        {
            // use `check_numeric_expr()` instead
            return error_t::from_error(dep0::error_t{"Cannot infer type of numeric constant", loc}, ctx);
        }
    };
    return std::visit(visitor{ctx, x.properties}, x.value);
}

expected<module_t> check(tt::context_t ctx, parser::module_t const& x)
{
    ctx = ctx.extend();
    std::vector<func_def_t> fs;
    fs.reserve(x.func_defs.size());
    for (auto const& f: x.func_defs)
        if (auto res = check(ctx, f))
        {
            if (auto ok = ctx.add(tt::term_t::var_t(f.name), tt::type_of(res->type.properties.derivation));
                not ok)
            {
                return error_t::from_error(ok.error(), ctx);
            }
            fs.push_back(std::move(*res));
        }
        else
            return std::move(res.error());
    return module_t{legal_module_t{}, std::move(fs)};
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
        type_t operator()(parser::type_t::bool_t const&) const { return type_t{bool_derivation(), type_t::bool_t{}}; }
        type_t operator()(parser::type_t::unit_t const&) const { return type_t{unit_derivation(), type_t::unit_t{}}; }
        type_t operator()(parser::type_t::i8_t const&) const { return type_t{i8_derivation(), type_t::i8_t{}}; }
        type_t operator()(parser::type_t::i16_t const&) const { return type_t{i16_derivation(), type_t::i16_t{}}; }
        type_t operator()(parser::type_t::i32_t const&) const { return type_t{i32_derivation(), type_t::i32_t{}}; }
        type_t operator()(parser::type_t::i64_t const&) const { return type_t{i64_derivation(), type_t::i64_t{}}; }
        type_t operator()(parser::type_t::u8_t const&) const { return type_t{u8_derivation(), type_t::u8_t{}}; }
        type_t operator()(parser::type_t::u16_t const&) const { return type_t{u16_derivation(), type_t::u16_t{}}; }
        type_t operator()(parser::type_t::u32_t const&) const { return type_t{u32_derivation(), type_t::u32_t{}}; }
        type_t operator()(parser::type_t::u64_t const&) const { return type_t{u64_derivation(), type_t::u64_t{}}; }
    };
    return expected<type_t>(std::visit(visitor{}, t.value));
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
            if (auto expr = type_assign(ctx, x.expr))
                return stmt_t{legal_stmt_t{}, stmt_t::fun_call_t{std::move(*expr)}};
            else
                // leaving `expr.error().tgt` empty because for a function call statement there is no target type
                return std::move(expr.error());
        }
        expected<stmt_t> operator()(parser::stmt_t::if_else_t const& x)
        {
            auto cond = check(ctx, x.cond, type_t{bool_derivation(), type_t::bool_t{}});
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
        expected<expr_t> operator()(type_t::i8_t const& ty) const
        {
            std::string_view number = x.number.view();
            skip_plus_or_minus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "127"))
                return invalid_integer_number("i8_t");
            return expr_t{i8_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::i16_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus_or_minus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "32767"))
                return invalid_integer_number("i16_t");
            return expr_t{i16_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::i32_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus_or_minus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "2147483647"))
                return invalid_integer_number("i32_t");
            return expr_t{i32_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::i64_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus_or_minus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "9223372036854775807"))
                return invalid_integer_number("i64_t");
            return expr_t{i64_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::u8_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "255"))
                return invalid_integer_number("u8_t");
            return expr_t{u8_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::u16_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "65535"))
                return invalid_integer_number("u16_t");
            return expr_t{u16_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::u32_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "4294967295"))
                return invalid_integer_number("u32_t");
            return expr_t{u32_derivation(), expr_t::numeric_constant_t{x.number}};
        }
        expected<expr_t> operator()(type_t::u64_t const&) const
        {
            std::string_view number = x.number.view();
            skip_plus(number);
            skip_leading_zeros(number);
            if (bigger_than(number, "18446744073709551615"))
                return invalid_integer_number("u64_t");
            return expr_t{u64_derivation(), expr_t::numeric_constant_t{x.number}};
        }

    private:
        static void skip_plus(std::string_view& s)
        {
            if (s.starts_with('+'))
                s.remove_prefix(1);
        }
        static void skip_plus_or_minus(std::string_view& s)
        {
            if (s.starts_with('+') or s.starts_with('-'))
                s.remove_prefix(1);
        }
        static void skip_leading_zeros(std::string_view& s)
        {
            while (not s.empty() and s[0] == '0')
                s.remove_prefix(1);
        }
        // assuming `a` and `b` represent positive integer numbers without leading 0s, returns whether `a > b`
        static bool bigger_than(std::string_view a, std::string_view b)
        {
            if (a.size() > b.size())
                return true;
            if (a.size() == b.size())
                for (auto const i: std::views::iota(0ul, a.size()))
                    if (a[i] != b[i]) return a[i] > b[i];
            return false;
        }
        expected<expr_t> invalid_integer_number(std::string_view const type) const
        {
            std::ostringstream err;
            err << "Numeric constant does not fit inside `" << type << '`';
            return error_t::from_error(dep0::error_t{err.str(), loc}, ctx);
        }
    };
    return std::visit(visitor{ctx, x, loc, expected_type}, expected_type.value);
}

expected<expr_t> check(tt::context_t ctx, parser::expr_t const& x, type_t const& expected_type)
{
    if (auto const* const p = std::get_if<parser::expr_t::numeric_constant_t>(&x.value))
        return check_numeric_expr(ctx, *p, x.properties, expected_type);

    auto expr = type_assign(ctx, x);
    if (not expr)
    {
        expr.error().tgt = tt::type_of(expected_type.properties.derivation);
        return std::move(expr.error());
    }
    auto const& expected_ty = tt::type_of(expected_type.properties.derivation);
    auto const& actual_ty = tt::type_of(expr->properties.derivation);
    if (expected_ty != actual_ty)
    {
        std::ostringstream err;
        tt::pretty_print(err << "Expression of type `", actual_ty) << '`';
        tt::pretty_print(err << " does not typecheck with expected type `", expected_ty) << '`';
        return error_t::from_error(dep0::error_t{err.str(), x.properties}, ctx, expected_ty);
    }
    return expr;
}

} // namespace dep0::typecheck
