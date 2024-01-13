#include "llvm_predicates_v2/types.hpp"

namespace dep0::llvmgen::testing::v2::impl {

boost::test_tools::predicate_result is_i1(llvm::Type const& x)
{
    if (not x.isIntegerTy(1))
        return dep0::testing::failure("type is not 1bit integer");
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
        return dep0::testing::failure("type is not 8bit integer");
    return true;
}

boost::test_tools::predicate_result is_i8(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_i8(*p);
}

boost::test_tools::predicate_result is_i32(llvm::Type const& x)
{
    if (not x.isIntegerTy(32))
        return dep0::testing::failure("type is not 32bit integer");
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
        return dep0::testing::failure("type is not 64bit integer");
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
        return dep0::testing::failure("type is not void");
    return true;
}

boost::test_tools::predicate_result is_void(llvm::Type const* const p)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_void(*p);
}

} // namespace dep0::llvmgen::testing::v2::impl
