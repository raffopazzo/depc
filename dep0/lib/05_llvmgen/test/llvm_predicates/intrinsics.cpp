/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_predicates/intrinsics.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Intrinsics.h>

using namespace dep0::testing;

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_memcpy(llvm::Function const& x)
{
    if (not x.isIntrinsic())
        return failure("function is not intrinsic");
    if (auto const id = x.getIntrinsicID(); id != llvm::Intrinsic::memcpy)
        return failure("intrinsic function is not memcpy but: ", llvm::Intrinsic::getName(id).str());
    return true;
}

boost::test_tools::predicate_result is_memcpy(llvm::Function const* const p)
{
    if (not p)
        return failure("function is null");
    return is_memcpy(*p);
}

boost::test_tools::predicate_result is_memcpy(llvm::Value const& x)
{
    auto const p = llvm::dyn_cast<llvm::Function>(&x);
    if (not p)
        return failure("value is not a function");
    return is_memcpy(*p);
}

boost::test_tools::predicate_result is_memcpy(llvm::Value const* const p)
{
    if (not p)
        return failure("value is null");
    return is_memcpy(*p);
}

} // namespace dep0::llvmgen::testing::impl
