#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

namespace impl {

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::expr_t<P>>... ArgPredicates>
boost::test_tools::predicate_result
is_app_of(typename ast::expr_t<P>::app_t const& app, F&& f_func, ArgPredicates&&... f_args)
{
    if (auto const result = std::forward<F>(f_func)(app.func.get()); not result)
        return failure("inside invoked function: ", result.message());
    if (app.args.size() != sizeof...(ArgPredicates))
        return failure("wrong number of arguments ", app.args.size(), " != ", sizeof...(ArgPredicates));
    auto result = boost::test_tools::predicate_result(true);
    auto it = app.args.begin();
    int next = 0;
    ([&]
    {
        auto const i = next++;
        if (result)
            if (auto const tmp = f_args(*it++); not tmp)
                result = failure("function argument ", i, ": ", tmp.message());
    }(), ...);
    return result;
}

} // namespace impl

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::expr_t<P>>... ArgPredicates>
boost::test_tools::predicate_result is_app_of(ast::expr_t<P> const& expr, F&& f_func, ArgPredicates&&... f_args)
{
    auto const app = std::get_if<typename ast::expr_t<P>::app_t>(&expr.value);
    if (not app)
        return failure("expression is not app_t but ", pretty_name(expr.value));
    return impl::is_app_of<P>(*app, std::forward<F>(f_func), std::forward<ArgPredicates>(f_args)...);
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::expr_t<P>>... ArgPredicates>
constexpr auto app_of(F&& f_func, ArgPredicates&&... f_args)
{
    return [f_func=std::forward<F>(f_func), ...f_args=std::forward<ArgPredicates>(f_args)] (ast::expr_t<P> const& x)
    {
        return is_app_of(x, f_func, f_args...);
    };
}

} // namespace dep0::testing
