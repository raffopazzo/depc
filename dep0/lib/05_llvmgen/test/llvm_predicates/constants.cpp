/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_predicates/constants.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Constants.h>

using namespace dep0::testing;

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_constant(llvm::Value const& x, bool const v)
{
    auto const c = dyn_cast<llvm::ConstantInt>(&x);
    if (not c)
        return failure("value is not a constant but: ", to_string(x));
    if (auto const value = c->isOne(); value != v)
        return failure(value, " != ", v);
    return true;
}

boost::test_tools::predicate_result is_constant(llvm::Value const* const p, bool const v)
{
    if (not p)
        return failure("value is null");
    return is_constant(*p, v);
}

boost::test_tools::predicate_result is_constant(llvm::Value const& x, int const v)
{
    auto const c = dyn_cast<llvm::ConstantInt>(&x);
    if (not c)
        return failure("value is not a constant but: ", to_string(x));
    if (auto const value = c->getSExtValue(); value != v)
        return failure(value, " != ", v);
    return true;
}

boost::test_tools::predicate_result is_constant(llvm::Value const* const p, int const v)
{
    if (not p)
        return failure("value is null");
    return is_constant(*p, v);
}

boost::test_tools::predicate_result is_constant(llvm::Value const& x, std::int64_t const v)
{
    auto const c = dyn_cast<llvm::ConstantInt>(&x);
    if (not c)
        return failure("value is not a constant but: ", to_string(x));
    if (auto const value = c->getSExtValue(); value != v)
        return failure(value, " != ", v);
    return true;
}

boost::test_tools::predicate_result is_constant(llvm::Value const* const p, std::int64_t const v)
{
    if (not p)
        return failure("value is null");
    return is_constant(*p, v);
}

boost::test_tools::predicate_result is_constant(llvm::Value const& x, std::uint64_t const v)
{
    auto const c = dyn_cast<llvm::ConstantInt>(&x);
    if (not c)
        return failure("value is not a constant but: ", to_string(x));
    if (auto const value = c->getZExtValue(); value != v)
        return failure(value, " != ", v);
    return true;
}

boost::test_tools::predicate_result is_constant(llvm::Value const* const p, std::uint64_t const v)
{
    if (not p)
        return failure("value is null");
    return is_constant(*p, v);
}

} // namespace dep0::llvmgen::testing::impl
