#include "llvm_predicates/unreachable.hpp"

#include "dep0/testing/failure.hpp"

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_unreachable(llvm::Instruction const& x)
{
    auto const ret = dyn_cast<llvm::UnreachableInst>(&x);
    if (not ret)
        return dep0::testing::failure("instruction is not unreachable but: ", x.getOpcodeName());
    return true;
}
boost::test_tools::predicate_result is_unreachable(llvm::Instruction const* const p)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_unreachable(*p);
}

} // namespace dep0::llvmgen::testing::impl
