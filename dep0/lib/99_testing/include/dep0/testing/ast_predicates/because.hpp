#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::expr_t<P>> F_value, Predicate<ast::expr_t<P>> F_reason>
boost::test_tools::predicate_result is_because_of(ast::expr_t<P> const& expr, F_value&& f_value, F_reason&& f_reason)
{
    auto const because = std::get_if<typename ast::expr_t<P>::because_t>(&expr.value);
    if (not because)
        return failure("type is not because_t but ", pretty_name(expr.value));
    if (auto const result = std::forward<F_value>(f_value)(because->value.get()); not result)
        return failure("inside value: ", result.message());
    if (auto const result = std::forward<F_reason>(f_reason)(because->reason.get()); not result)
        return failure("inside reason: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_value, Predicate<ast::expr_t<P>> F_reason>
constexpr auto because_of(F_value&& f_value, F_reason&& f_reason)
{
    return [f_value=std::forward<F_value>(f_value), f_reason=std::forward<F_reason>(f_reason)] (ast::expr_t<P> const& x)
    {
        return is_because_of(x, f_value, f_reason);
    };
}

} // namespace dep0::testing

