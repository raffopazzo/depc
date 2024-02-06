#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

inline constexpr auto is_array =
[] <ast::Properties P> (ast::expr_t<P> const& x)
-> boost::test_tools::predicate_result
{
    if (std::holds_alternative<typename ast::expr_t<P>::array_t>(x.value))
        return true;
    else
        return failure("expression is not array_t but ", pretty_name(x.value));
};

template <ast::Properties P, Predicate<ast::expr_t<P>> F_type, Predicate<ast::expr_t<P>> F_size>
boost::test_tools::predicate_result is_array_of(ast::expr_t<P> const& x, F_type&& f_type, F_size&& f_size)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&x.value);
    if (not app)
        return failure("not an array because not an application but ", pretty_name(x.value));
    if (not std::holds_alternative<typename ast::expr_t<P>::array_t>(app->func.get().value))
        return failure("not an array because not an application of array_t but ", pretty_name(app->func.get().value));
    if (app->args.size() != 2ul)
        return failure("not an array because not an application of array_t with 2 arguments but ", app->args.size());
    if (auto const result = std::forward<F_type>(f_type)(app->args[0ul]); not result)
        return failure("array element type predicate failed: ", result.message());
    if (auto const result = std::forward<F_size>(f_size)(app->args[1ul]); not result)
        return failure("array size predicate failed: ", result.message());
    return true;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F_type, Predicate<ast::expr_t<P>> F_size>
constexpr auto array_of(F_type&& f_type, F_size&& f_size)
{
    return [f_type=std::forward<F_type>(f_type), f_size=std::forward<F_size>(f_size)] (ast::expr_t<P> const& x)
    {
        return is_app_of(x, is_array, f_type, f_size);
    };
}

} // namespace dep0::testing
