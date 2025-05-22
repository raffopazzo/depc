/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "llvm_predicates/add.hpp"
#include "llvm_predicates/align.hpp"
#include "llvm_predicates/alloca.hpp"
#include "llvm_predicates/and.hpp"
#include "llvm_predicates/argument.hpp"
#include "llvm_predicates/bitcast.hpp"
#include "llvm_predicates/branch.hpp"
#include "llvm_predicates/call.hpp"
#include "llvm_predicates/cmp.hpp"
#include "llvm_predicates/constants.hpp"
#include "llvm_predicates/cstrings.hpp"
#include "llvm_predicates/div.hpp"
#include "llvm_predicates/function.hpp"
#include "llvm_predicates/function_type.hpp"
#include "llvm_predicates/gep.hpp"
#include "llvm_predicates/global.hpp"
#include "llvm_predicates/intrinsics.hpp"
#include "llvm_predicates/load.hpp"
#include "llvm_predicates/mul.hpp"
#include "llvm_predicates/or.hpp"
#include "llvm_predicates/phi.hpp"
#include "llvm_predicates/return.hpp"
#include "llvm_predicates/select.hpp"
#include "llvm_predicates/store.hpp"
#include "llvm_predicates/sub.hpp"
#include "llvm_predicates/to_string.hpp"
#include "llvm_predicates/types.hpp"
#include "llvm_predicates/unreachable.hpp"
#include "llvm_predicates/xor.hpp"
#include "llvm_predicates/zeroinitializer.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Value.h>

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

inline auto exactly(llvm::Value const* const p)
{
    return [p] (llvm::Value const& x) -> boost::test_tools::predicate_result
    {
        if (not p)
            return dep0::testing::failure("value is null");
        else if (p != &x)
            return dep0::testing::failure(
                p, '(', p->getName().str(), ") != ",
                &x, '(', x.getName().str(), ')');
        else
            return true;
    };
}

inline auto exactly(llvm::Type const* const p)
{
    return [p] (llvm::Type const& x) -> boost::test_tools::predicate_result
    {
        if (not p)
            return dep0::testing::failure("type is null");
        else if (p != &x)
            return dep0::testing::failure(
                p, '(', to_string(*p), ") != ",
                &x, '(', to_string(x), ')');
        else
            return true;
    };
}

template <Predicate<llvm::Instruction>... F>
boost::test_tools::predicate_result is_block_of(llvm::BasicBlock const& x, std::tuple<F...> f)
{
    auto constexpr N = sizeof...(F);
    if (auto const n = x.size(); n != N)
        return dep0::testing::failure("wrong number of instructions: ", n, " != ", N);
    auto result = boost::test_tools::predicate_result(true);
    auto it = x.begin();
    [&] <std::size_t... I> (std::index_sequence<I...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<I>(f)(*it++); not tmp)
                    result = dep0::testing::failure("inside instruction ", I, ": ", tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

} // namespace dep0::llvmgen::testing
