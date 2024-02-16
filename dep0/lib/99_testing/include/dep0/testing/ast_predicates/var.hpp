#pragma once

#include "dep0/testing/ast_predicates/details/check_name.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_var(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const var = std::get_if<typename ast::expr_t<P>::var_t>(&expr.value);
    if (not var)
        return failure("expression is not var_t but ", pretty_name(expr.value));
    return details::check_name<P>(*var, name);
}

template <ast::Properties P>
boost::test_tools::predicate_result is_global(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const g = std::get_if<typename ast::expr_t<P>::global_t>(&expr.value);
    if (not g)
        return failure("expression is not global_t but ", pretty_name(expr.value));
    if (g->name.view() != name)
    {
        auto failed = boost::test_tools::predicate_result(false);
        ast::pretty_print<P>(failed.message().stream(), *g) << " != " << name;
        return failed;
    }
    return true;
}

inline auto var(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        auto result = is_global(expr, name); // TODO remove
        if (not result)
            result = is_var(expr, name);
        return result;
    };
}

inline auto global(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_global(expr, name);
    };
}

} // namespace dep0::testing
