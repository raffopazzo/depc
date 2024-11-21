/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/ast_predicates/details/check_name.hpp"

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

inline auto constexpr is_unnamed_var =
[] <ast::Properties P> (ast::expr_t<P> const& expr)
-> boost::test_tools::predicate_result
{
    auto const var = std::get_if<typename ast::expr_t<P>::var_t>(&expr.value);
    if (not var)
        return failure("expression is not var_t but ", pretty_name(expr.value));
    if (var->name != "auto")
        return failure("variable has name `", var->name, "` but should be unnamed");
    return true;
};

template <ast::Properties P>
boost::test_tools::predicate_result is_var(ast::expr_t<P> const& expr, std::string_view const name)
{
    auto const var = std::get_if<typename ast::expr_t<P>::var_t>(&expr.value);
    if (not var)
        return failure("expression is not var_t but ", pretty_name(expr.value));
    return details::check_name<P>(*var, name);
}

inline auto var(std::string const& name)
{
    return [name] <ast::Properties P> (ast::expr_t<P> const& expr)
    {
        return is_var(expr, name);
    };
}

} // namespace dep0::testing
