#include "dep0/typecheck/check.hpp"

namespace dep0::typecheck {

expected<module_t> check(parser::module_t const& x)
{
    std::vector<func_def_t> fs;
    fs.reserve(x.func_defs.size());
    for (auto const& f: x.func_defs)
        if (auto res = check(f))
            fs.push_back(std::move(*res));
        else
            return std::move(res.error());
    return module_t{legal_module_t{}, std::move(fs)};
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
    return func_def_t{legal_func_def_t{}, std::move(*ret_type), f.name, std::move(*body)};
}

expected<type_t> check(parser::type_t const& t)
{
    struct visitor
    {
        expected<type_t> operator()(parser::type_t::int_t const&) const
        {
            // `int` and other primitives are always derivable
            return type_t{legal_type_t{}, typename type_t::int_t{}};
        }
    };
    return std::visit(visitor{}, t.value);
}

expected<body_t> check(parser::body_t const& x, type_t const& return_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        if (auto stmt = check(s, return_type))
            stmts.push_back(std::move(*stmt));
        else
            return std::move(stmt.error());
    }
    return body_t{legal_body_t{}, std::move(stmts)};
}

expected<stmt_t> check(parser::stmt_t const& s, type_t const& return_type)
{
    struct visitor
    {
        type_t const& return_type;
        source_loc_t const& location;
        expected<stmt_t> operator()(parser::stmt_t::return_t const& x)
        {
            if (x.expr)
            {
                if (auto expr = check(*x.expr, return_type))
                    return stmt_t{legal_stmt_t{}, stmt_t::return_t{std::move(*expr)}};
                else
                    return std::move(expr.error());
            }
            else
                // TODO it `return_type` is "void" this would be fine
                // TODO replace `int` with real type name
                return error_t{"Expecting expression of type 'int'", location};
        }
    };
    return std::visit(visitor{return_type, s.properties}, s.value);
}

expected<expr_t> check(parser::expr_t const& x, type_t const& expected_type)
{
    struct visitor
    {
        source_loc_t const& location;
        expected<expr_t> operator()(parser::expr_t::numeric_constant_t const& x, type_t::int_t const&) const
        {
            // TODO should check that the string represents a valid integer, for now we say that only `0` is valid
            if (x.number != "0")
                return error_t{"Invalid integer number", location};
            return expr_t{legal_expr_t{}, typename expr_t::numeric_constant_t{x.number}};
        }
    };
    return std::visit(visitor{x.properties}, x.value, expected_type.value);
}

} // namespace dep0::typecheck
