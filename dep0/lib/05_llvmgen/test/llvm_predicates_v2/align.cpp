#include "llvm_predicates_v2/align.hpp"

#include "dep0/testing/failure.hpp"

namespace dep0::llvmgen::testing::v2::impl {

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
