/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_predicates/types.hpp"
#include "llvm_predicates/to_string.hpp"

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_i1(llvm::Type const& x)
{
    if (not x.isIntegerTy(1))
        return dep0::testing::failure("type is not 1bit integer but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_i1(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i1(*p);
}

boost::test_tools::predicate_result is_i8(llvm::Type const& x)
{
    if (not x.isIntegerTy(8))
        return dep0::testing::failure("type is not 8bit integer but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_i8(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i8(*p);
}

boost::test_tools::predicate_result is_i16(llvm::Type const& x)
{
    if (not x.isIntegerTy(16))
        return dep0::testing::failure("type is not 16bit integer but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_i16(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i16(*p);
}

boost::test_tools::predicate_result is_i32(llvm::Type const& x)
{
    if (not x.isIntegerTy(32))
        return dep0::testing::failure("type is not 32bit integer but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_i32(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i32(*p);
}

boost::test_tools::predicate_result is_i64(llvm::Type const& x)
{
    if (not x.isIntegerTy(64))
        return dep0::testing::failure("type is not 64bit integer but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_i64(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i64(*p);
}

boost::test_tools::predicate_result is_void(llvm::Type const& x)
{
    if (not x.isVoidTy())
        return dep0::testing::failure("type is not void but ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_void(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_void(*p);
}

} // namespace dep0::llvmgen::testing::impl
