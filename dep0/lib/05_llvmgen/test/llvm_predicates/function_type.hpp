/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"
#include "llvm_predicates/types.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Instructions.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
boost::test_tools::predicate_result is_function_type(llvm::Type const& x, std::tuple<Args...> args, F_ret&& f)
{
    using namespace dep0::testing;
    auto const func_type = llvm::dyn_cast<llvm::FunctionType>(&x);
    if (not func_type)
        return failure("type is not a function type but: ", to_string(x));
    auto constexpr N = sizeof...(Args);
    if (auto const n = func_type->getNumParams(); n != N)
        return failure("wrong number of arguments: ", N, " != ", n);
    auto const ret_type = func_type->getReturnType();
    if (not ret_type)
        return failure("function type has no return type");
    if (auto const result = std::forward<F_ret>(f)(*ret_type); not result)
        return failure("predicate failed for return type: ", result.message());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
            {
                if (auto const ty = func_type->getParamType(Is); not ty)
                    result = failure("argument type ", Is, " is null");
                else if (auto const tmp = std::get<Is>(args)(*ty); not tmp)
                    result = failure("argument type predicate at index ", Is, " failed: ", tmp.message());
            }
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
boost::test_tools::predicate_result is_function_type(llvm::Type const* const p, std::tuple<Args...> args, F_ret&& f)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_function_type(*p, std::move(args), std::forward<F_ret>(f));
}

} // namespace impl

inline constexpr auto is_function_type = boost::hana::overload(
    [] <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
    (llvm::Type const& x, std::tuple<Args...> args, F_ret&& f)
    {
        return impl::is_function_type(x, std::move(args), std::forward<F_ret>(f));
    },
    [] <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
    (llvm::Type const* const p, std::tuple<Args...> args, F_ret&& f)
    {
        return impl::is_function_type(p, std::move(args), std::forward<F_ret>(f));
    }
);

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
auto function_type(std::tuple<Args...> args, F_ret&& f)
{
    return [f=std::forward<F_ret>(f), args=std::move(args)] (llvm::Type const& x)
    {
        return is_function_type(x, args, f);
    };
}

template <Predicate<llvm::Type> F_ret, Predicate<llvm::Type>... Args>
auto fnptr_type(std::tuple<Args...> args, F_ret&& f)
{
    return pointer_to(function_type(std::move(args), std::forward<F_ret>(f)));
}

} // namespace dep0::llvmgen::testing
