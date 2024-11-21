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

template <ast::Properties P, Predicate<ast::expr_t<P>> F_array, Predicate<ast::expr_t<P>> F_index>
boost::test_tools::predicate_result is_subscript_of(ast::expr_t<P> const& expr, F_array&& f_array, F_index&& f_index)
{
    auto const subscript = std::get_if<typename ast::expr_t<P>::subscript_t>(&expr.value);
    if (not subscript)
        return failure("type is not subscript_t but ", pretty_name(expr.value));
    if (auto const result = std::forward<F_array>(f_array)(subscript->array.get()); not result)
        return failure("array predicate failed: ", result.message());
    if (auto const result = std::forward<F_index>(f_index)(subscript->index.get()); not result)
        return failure("index predicate failed: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_array, Predicate<ast::expr_t<P>> F_index>
constexpr auto subscript_of(F_array&& f_array, F_index&& f_index)
{
    return [f_array=std::forward<F_array>(f_array), f_index=std::forward<F_index>(f_index)] (ast::expr_t<P> const& x)
    {
        return is_subscript_of(x, f_array, f_index);
    };
}

} // namespace dep0::testing
