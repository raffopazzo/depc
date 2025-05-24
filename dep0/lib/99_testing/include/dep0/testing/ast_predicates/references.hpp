/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result is_deref(ast::expr_t<P> const& expr, F&& f)
{
    auto const deref = std::get_if<typename ast::expr_t<P>::deref_t>(&expr.value);
    if (not deref)
        return failure("expression is not deref_t but ", pretty_name(expr.value));
    if (auto const result = std::forward<F>(f)(deref->ref.get()); not result)
        return failure("inside dereferenced expression: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F>
constexpr auto deref(F&& f)
{
    return [f=std::forward<F>(f)] (ast::expr_t<P> const& x)
    {
        return is_deref(x, f);
    };
}

inline constexpr auto is_ref =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::ref_t>(x.value))
        return true;
    else
        return failure("expression is not ref_t but ", pretty_name(x.value));
};

template <ast::Properties P, Predicate<ast::expr_t<P>> F_type, Predicate<ast::expr_t<P>> F_scope>
constexpr auto ref_of(F_type&& f_type, F_scope&& f_scope)
{
    return [f_type=std::forward<F_type>(f_type), f_scope=std::forward<F_scope>(f_scope)] (ast::expr_t<P> const& x)
    {
        return is_app_of(x, is_ref, f_type, f_scope);
    };
}

inline constexpr auto is_scope =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::scope_t>(x.value))
        return true;
    else
        return failure("expression is not scope_t but ", pretty_name(x.value));
};

template <ast::Properties P>
boost::test_tools::predicate_result is_scopeof(ast::expr_t<P> const& expr, std::string_view const var)
{
    auto const s = std::get_if<typename ast::expr_t<P>::scopeof_t>(&expr.value);
    if (not s)
        return failure("expression is not scopeof_t but ", pretty_name(expr.value));
    if (s->var != var)
        return failure("scopeof(", s->var, ") != scopeof(", var, ')');
    return true;
}

inline auto scopeof(std::string_view const var)
{
    return [var=std::string(var)] <ast::Properties P> (ast::expr_t<P> const& x)
    {
        return is_scopeof(x, var);
    };
}

} // namespace dep0::testing
