/*
 * Copyright Raffaele Rossi 2023 - 2024.
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

inline constexpr auto is_true_t =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::true_t>(x.value))
        return true;
    else
        return failure("expression is not true_t but ", pretty_name(x.value));
};

template <ast::Properties P, Predicate<ast::expr_t<P>> F_expr>
boost::test_tools::predicate_result is_true_t_of(ast::expr_t<P> const& x, F_expr&& f_expr)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&x.value);
    if (not app)
        return failure("not a true_t because not an application but ", pretty_name(x.value));
    if (not std::holds_alternative<typename ast::expr_t<P>::true_t>(app->func.get().value))
        return failure("not a true_t because not an application of true_t but ", pretty_name(app->func.get().value));
    if (app->args.size() != 1ul)
        return failure("not a true_t because application does not have exactly 1 argument but ", app->args.size());
    if (auto const result = std::forward<F_expr>(f_expr)(app->args[0ul]); not result)
        return failure("inside argument of true_t: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_expr>
constexpr auto true_t_of(F_expr&& f_expr)
{
    return [f_expr=std::forward<F_expr>(f_expr)] (ast::expr_t<P> const& x)
    {
        return is_true_t_of(x, f_expr);
    };
}

} // namespace dep0::testing
