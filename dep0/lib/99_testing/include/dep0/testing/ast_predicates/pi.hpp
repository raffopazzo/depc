#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/ostream.hpp"
#include "dep0/testing/predicate.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P, Predicate<ast::func_arg_t<P>>... ArgPredicates, Predicate<ast::expr_t<P>> F>
boost::test_tools::predicate_result
is_pi_of(
    ast::expr_t<P> const& type,
    std::tuple<ArgPredicates...> const& f_args,
    F&& f_ret_type,
    ast::is_mutable_t const is_mutable = ast::is_mutable_t::no)
{
    auto const pi = std::get_if<typename ast::expr_t<P>::pi_t>(&type.value);
    if (not pi)
        return failure("type is not pi_t but ", pretty_name(type.value));
    if (pi->is_mutable != is_mutable)
        return failure("pi-type has wrong mutability", pi->is_mutable, " != ", is_mutable);
    if (auto const result = std::forward<F>(f_ret_type)(pi->ret_type.get()); not result)
        return failure("return type predicate failed: ", result.message());
    auto constexpr N = sizeof...(ArgPredicates);
    if (pi->args.size() != N)
        return failure("wrong number of arguments: ", N, " != ", pi->args.size());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_args)(pi->args[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <ast::Properties P, Predicate<ast::expr_t<P>> F, Predicate<ast::func_arg_t<P>>... ArgPredicates>
constexpr auto pi_of(std::tuple<ArgPredicates...> args, F&& ret_type, ast::is_mutable_t m = ast::is_mutable_t::no)
{
    return [args=std::move(args), ret_type=std::forward<F>(ret_type), m] (ast::expr_t<P> const& x)
    {
        return is_pi_of(x, args, ret_type, m);
    };
}

} // namespace dep0::testing
