#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/returns_from_all_branches.hpp"

#include <sstream>

namespace dep0::typecheck {

static tt::derivation_t bool_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_bool()); }
static tt::derivation_t int_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_int()); }
static tt::derivation_t unit_derivation() { return tt::derivation_t(tt::derivation_t::form_t::primitive_unit()); }

expected<module_t> check(tt::context_t ctx, parser::module_t const& x)
{
    std::vector<func_def_t> fs;
    fs.reserve(x.func_defs.size());
    for (auto const& f: x.func_defs)
        if (auto res = check(ctx, f))
        {
            if (auto extended = ctx.extend(tt::term_t::var_t(f.name), tt::type_of(res->type.properties.derivation));
                not extended)
            {
                return extended.error();
            }
            fs.push_back(std::move(*res));
        }
        else
            return std::move(res.error());
    return module_t{legal_module_t{}, std::move(fs)};
}

expected<func_def_t> check(tt::context_t ctx, parser::func_def_t const& f)
{
    auto ret_type = check(ctx, f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    if (auto extended = ctx.extend(tt::term_t::var_t(f.name), tt::type_of(ret_type->properties.derivation));
        not extended)
    {
        return extended.error();
    }
    auto body = check(ctx, f.body, *ret_type);
    if (not body)
        return std::move(body.error());
// TODO: add this check
//  if (type_of(ret_type->properties.conclusion) != type_of(body->properties.conclusion))
//      return error_t{"The return type `{}` does not match the body type `{}`"};

    // so far so good, but we now need to make sure that all branches contain a return statement,
    // with the only exception of functions returning `unit_t` because the return statement is optional;
    if (not std::holds_alternative<type_t::unit_t>(ret_type->value) and not returns_from_all_branches(*body))
    {
        std::ostringstream err;
        err << "In function `" << f.name << "` missing return statement";
        return error_t{err.str(), f.properties};
    }
    return func_def_t{legal_func_def_t{}, std::move(*ret_type), f.name, std::move(*body)};
}

expected<type_t> check(tt::context_t ctx, parser::type_t const& t)
{
    struct visitor
    {
        tt::context_t& ctx;
        source_loc_t const& location;

        expected<type_t> operator()(parser::type_t::bool_t const&) const
        {
            return type_t{bool_derivation(), type_t::bool_t{}};
        }
        expected<type_t> operator()(parser::type_t::int_t const&) const
        {
            return type_t{int_derivation(), type_t::int_t{}};
        }
        expected<type_t> operator()(parser::type_t::unit_t const&) const
        {
            return type_t{unit_derivation(), type_t::unit_t{}};
        }
    };
    return std::visit(visitor{ctx, t.properties}, t.value);
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
        source_loc_t const& location;
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
                return error_t{err.str(), location};
            }
            else if (auto expr = check(ctx, *x.expr, return_type))
                return stmt_t{legal_stmt_t{}, stmt_t::return_t{std::move(*expr)}};
            else
                return std::move(expr.error());
        }
    };
    return std::visit(visitor{ctx, return_type, s.properties}, s.value);
}

struct check_expr_visitor
{
    tt::context_t const& context;
    legal_type_t const& expected_type;
    source_loc_t const& location;

    template <typename T>
    expected<expr_t> operator()(parser::expr_t::fun_call_t const& x, T const&) const
    {
        auto fun_type = context[tt::term_t::var_t(x.name)];
        if (not fun_type)
        {
            std::ostringstream err;
            err << "Unknown function name `" << x.name << '`';
            return error_t{err.str(),  location};
        }
        else if (*fun_type != tt::type_of(expected_type.derivation))
        {
            std::ostringstream err;
            tt::pretty_print(err << "Type mismatch between function return type `", *fun_type);
            tt::pretty_print(err << "` and expected type `", tt::type_of(expected_type.derivation)) << '`';
            return error_t{err.str(), location};
        }
        else
            return expr_t{expected_type.derivation, expr_t::fun_call_t{x.name}};
    }

    template <typename T>
    expected<expr_t> operator()(parser::expr_t::boolean_constant_t const& x, T const&) const
    {
        std::ostringstream err;
        tt::pretty_print(
            err << "Type mismatch between boolean constant and `",
            tt::type_of(expected_type.derivation)
        ) << '`';
        return error_t{err.str(), location};
    }

    expected<expr_t> operator()(parser::expr_t::boolean_constant_t const& x, type_t::bool_t const&) const
    {
        return expr_t{expected_type.derivation, expr_t::boolean_constant_t{x.value}};
    }

    template <typename T>
    expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x, T const&) const
    {
        std::ostringstream err;
        tt::pretty_print(
            err << "Type mismatch between numeric constant and `",
            tt::type_of(expected_type.derivation)
        ) << '`';
        return error_t{err.str(), location};
    }

    expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x, type_t::int_t const&) const
    {
        // TODO should check that the string represents a valid integer, for now we say that only `0` is valid
        // the test per se is easy, I really need to decide on what primitive types I want, possible choices are:
        // - `[unsigned] (short|int|long|long long)` with platform defined width, like C
        // - `[iu](8|16|32|64|128)_t` with fixed width
        // - both
        // - both but `short, int, etc` have fixed width and `word_t, quadword_t` for platform stuff
        if (x.number != "0")
            return error_t{"Invalid integer number", location};
        return expr_t{expected_type.derivation, expr_t::numeric_constant_t{x.number}};
    }
};
expected<expr_t> check(tt::context_t ctx, parser::expr_t const& x, type_t const& expected_type)
{
    return std::visit(check_expr_visitor{ctx, expected_type.properties, x.properties}, x.value, expected_type.value);
}

} // namespace dep0::typecheck
