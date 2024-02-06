#pragma once

#include "dep0/testing/check_all.hpp"
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
    if constexpr (N > 0ul)
        return check_all(init_list->values, std::forward<ValuePredicates>(f_values)...);
    else
        return true;
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
