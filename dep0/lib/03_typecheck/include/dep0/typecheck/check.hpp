#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/concepts.hpp"
#include "dep0/ast/tree.hpp"

#include "dep0/error.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace dep0::typecheck {

struct empty_t{};

struct derivation
{
    // TODO: here we should store the computed type for the function/expression (maybe statement?)
    // also the derivation of type declarations somehow

    // currently however we only support `int`, so a value of type `derivation` represents a valid integer expression
};

struct derivation_properties
{
    using module_properties_type = empty_t;
    using func_def_properties_type = empty_t; // `func_def` carries its return type which contains its own derivation
    using type_properties_type = derivation;
    using body_properties_type = empty_t;
    using stmt_properties_type = std::optional<derivation>; // why? does it really make sense? worth adding a comment
    using expr_properties_type = derivation;
};
static_assert(ast::Properties<derivation_properties>);

using module_t = ast::module_t<derivation_properties>;
using func_def_t = ast::func_def_t<derivation_properties>;
using type_t = ast::type_t<derivation_properties>;
using body_t = ast::body_t<derivation_properties>;
using stmt_t = ast::stmt_t<derivation_properties>;
using expr_t = ast::expr_t<derivation_properties>;

using tree = ast::tree<ast::module_t, derivation_properties>;

template <ast::Properties P> expected<module_t> check(ast::module_t<P> const&);
template <ast::Properties P> expected<func_def_t> check(ast::func_def_t<P> const&);
template <ast::Properties P> expected<type_t> check(ast::type_t<P> const&);
template <ast::Properties P> expected<body_t> check(ast::body_t<P> const&, std::optional<type_t> const&);
template <ast::Properties P> expected<stmt_t> check(ast::stmt_t<P> const&, std::optional<type_t> const&);
template <ast::Properties P> expected<expr_t> check(ast::expr_t<P> const&, type_t const&);

template <ast::Properties P> expected<tree> check(ast::tree<ast::module_t, P> const& x)
{
    if (auto result = check(x.root))
        return tree{x.source, std::move(*result)};
    else
        return result.error();
}

template <ast::Properties P> expected<module_t> check(ast::module_t<P> const& x)
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

template <ast::Properties P> expected<func_def_t> check(ast::func_def_t<P> const& f)
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

template <ast::Properties P> expected<type_t> check(ast::type_t<P> const& t)
{
    struct visitor
    {
        expected<type_t> operator()(typename ast::type_t<P>::int_t const&) const
        {
            // `int` and other primitives are always derivable
            return type_t{derivation{}, typename type_t::int_t{}};
        }
    };
    return std::visit(visitor{}, t.value);
}

// a body may not have an expected type, eg `void f()` - though it should perhaps be some unit type
template <ast::Properties P> expected<body_t> check(ast::body_t<P> const& x, std::optional<type_t> const& expected_type)
{
    std::vector<stmt_t> stmts;
    stmts.reserve(x.stmts.size());
    for (auto const& s: x.stmts)
    {
        auto stmt = [&]
        {
            if (std::holds_alternative<typename ast::stmt_t<P>::return_t>(s.value))
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
template <ast::Properties P> expected<stmt_t> check(ast::stmt_t<P> const& s, std::optional<type_t> const& expected_type)
{
    struct visitor
    {
        std::optional<type_t> const& expected_type;
        expected<stmt_t> operator()(typename ast::stmt_t<P>::return_t const& x)
        {
            if (x.expr)
            {
                if (not expected_type)
                    return error_t{"Unexpected return statement"};
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
                    return error_t{"Expecting expression of type 'int'"};
                return stmt_t{std::nullopt, stmt_t::return_t{std::nullopt}};
            }
        }
    };
    return std::visit(visitor{expected_type}, s.value);
}

template <ast::Properties P> expected<expr_t> check(ast::expr_t<P> const& x, type_t const& expected_type)
{
    struct visitor
    {
        expected<expr_t> operator()(typename ast::expr_t<P>::numeric_constant_t const& x, type_t::int_t const&) const
        {
            // TODO should check that the string represents a valid integer, for now we say that only `0` is valid
            if (x.number == "0")
                return expr_t{derivation{}, typename expr_t::numeric_constant_t{x.number}};
            else
                return error_t{"Invalid integer number"};
        }
    };
    return std::visit(visitor{}, x.value, expected_type.value);
}

} // namespace dep0::typecheck
