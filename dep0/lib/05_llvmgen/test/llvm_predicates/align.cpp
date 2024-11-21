/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_predicates/align.hpp"

#include "dep0/testing/failure.hpp"

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_align_of(llvm::Align const& x, std::size_t const value)
{
    if (x.value() != value)
        return dep0::testing::failure("align value ", x.value(), " != ", value);
    return true;
}

boost::test_tools::predicate_result is_align_of(llvm::Align const* const p, std::size_t const value)
{
    if (not p)
        return dep0::testing::failure("align is null");
    return is_align_of(*p, value);
}

} // namespace dep0::llvmgen::testing::impl
