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

template <ast::Properties P, Predicate<ast::expr_t<P>>... ValuePredicates>
boost::test_tools::predicate_result is_init_list_of(ast::expr_t<P> const& expr, ValuePredicates&&... f_values)
{
    auto const init_list = std::get_if<typename ast::expr_t<P>::init_list_t>(&expr.value);
    if (not init_list)
        return failure("expr is not init_list_t but ", pretty_name(expr.value));
    auto constexpr N = sizeof...(ValuePredicates);
    if (init_list->values.size() != N)
        return failure("wrong number of initializer values: ", N, " != ", init_list->values.size());
    auto result = boost::test_tools::predicate_result(true);
    auto it = init_list->values.begin();
    int next = 0;
    ([&]
    {
        auto const i = next++;
        if (result)
            if (auto const tmp = f_values(*it++); not tmp)
                result = failure("initializer value ", i, ": ", tmp.message());
    }(), ...);
    return result;
}

template <ast::Properties P, Predicate<ast::expr_t<P>>... ValuePredicates>
constexpr auto init_list_of(ValuePredicates&&... f_values)
{
    return [...f_values=std::forward<ValuePredicates>(f_values)] (ast::expr_t<P> const& x)
    {
        return is_init_list_of(x, f_values...);
    };
}

} // namespace dep0::testing
