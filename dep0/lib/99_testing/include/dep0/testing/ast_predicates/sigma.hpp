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

template <ast::Properties P, Predicate<ast::func_arg_t<P>>... ArgPredicates>
boost::test_tools::predicate_result
is_sigma_of(ast::expr_t<P> const& type, std::tuple<ArgPredicates...> const& f_args)
{
    auto const sigma = std::get_if<typename ast::expr_t<P>::sigma_t>(&type.value);
    if (not sigma)
        return failure("type is not sigma_t but ", pretty_name(type.value));
    auto constexpr N = sizeof...(ArgPredicates);
    if (sigma->args.size() != N)
        return failure("wrong number of arguments: ", N, " != ", sigma->args.size());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_args)(sigma->args[Is]); not tmp)
                    result = failure("argument predicate at index ", Is, " failed: ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <ast::Properties P, Predicate<ast::func_arg_t<P>>... ArgPredicates>
constexpr auto sigma_of(std::tuple<ArgPredicates...> args)
{
    return [args=std::move(args)] (ast::expr_t<P> const& x)
    {
        return is_sigma_of(x, args);
    };
}

} // namespace dep0::testing
