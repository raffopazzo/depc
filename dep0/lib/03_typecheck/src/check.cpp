#include "dep0/typecheck/check.hpp"

namespace dep0::typecheck {

expected<tree> check(parser::parse_tree const& x)
{
    if (auto result = check(x.root))
        return tree{x.source, std::move(*result)};
    else
        return result.error();
}

expected<module_t> check(parser::module_t const& x)
{
    std::vector<func_def_t> fs;
    fs.reserve(x.func_defs.size());
    for (auto const& f: x.func_defs)
        if (auto res = check(f))
            fs.push_back(std::move(*res));
        else
            return std::move(res.error());
    return module_t{empty_t{}, std::move(fs)};
}

expected<func_def_t> check(parser::func_def_t const& f)
{
    auto ret_type = check(f.type);
    if (not ret_type)
        return std::move(ret_type.error());
    auto body = check(f.body, *ret_type);
    if (not body)
        return std::move(body.error());
// TODO: add this check
//  if (type_of(ret_type->properties.conclusion) != type_of(body->properties.conclusion))
//      return error_t{"The return type `{}` does not match the body type `{}`"};
    return func_def_t{empty_t{}, std::move(*ret_type), f.name, std::move(*body)};
}

expected<type_t> check(parser::type_t const& t)
{
    struct visitor
    {
        expected<type_t> operator()(parser::type_t::int_t const&) const
        {
            // `int` and other primitives are always derivable
            return type_t{derivation{}, typename type_t::int_t{}};
        }
    };
    return std::visit(visitor{}, t.value);
}

// a body may not have an expected type, eg `void f()` - though it should perhaps be some unit type
expected<body_t> check(parser::body_t const& x, std::optional<type_t> const& expected_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        auto stmt = [&]
        {
            if (std::holds_alternative<parser::stmt_t::return_t>(s.value))
                return check(s, expected_type);
            else
                return check(s, std::nullopt);
        }();
        if (stmt)
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return body_t{empty_t{}, std::move(stmts)};
}

// statement also may not have a type, eg an assigment `a = 5;`
expected<stmt_t> check(parser::stmt_t const& s, std::optional<type_t> const& expected_type)
{
    struct visitor
    {
        std::optional<type_t> const& expected_type;
        source_loc_t const& location;
        expected<stmt_t> operator()(parser::stmt_t::return_t const& x)
        {
            if (x.expr)
            {
                if (not expected_type)
                    return error_t{"Unexpected return statement", location};
                if (auto expr = check(*x.expr, *expected_type))
                {
                    auto der = expr->properties;
                    return stmt_t{std::move(der), stmt_t::return_t{std::move(*expr)}};
                }
                else
                    return std::move(expr.error());
            }
            else
            {
                if (expected_type)
                    // TODO replace `int` with real type name
                    return error_t{"Expecting expression of type 'int'", location};
                return stmt_t{std::nullopt, stmt_t::return_t{std::nullopt}};
            }
        }
    };
    return std::visit(visitor{expected_type, s.properties}, s.value);
}

expected<expr_t> check(parser::expr_t const& x, type_t const& expected_type)
{
    struct visitor
    {
        source_loc_t const& location;
        expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x, type_t::int_t const&) const
        {
            // TODO should check that the string represents a valid integer, for now we say that only `0` is valid
            if (x.number == "0")
                return expr_t{derivation{}, typename expr_t::numeric_constant_t{x.number}};
            else
                return error_t{"Invalid integer number", location};
        }
    };
    return std::visit(visitor{x.properties}, x.value, expected_type.value);
}

} // namespace dep0::typecheck
