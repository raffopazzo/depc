#include "llvm_predicates_v2/return.hpp"

namespace dep0::llvmgen::testing::v2::impl {

boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const& x)
{
    auto const ret = dyn_cast<llvm::ReturnInst>(&x);
    if (not ret)
        return dep0::testing::failure("instruction is not return but: ", x.getOpcodeName());
    if (ret->getNumOperands() != 0)
        return dep0::testing::failure("return instruction has a value but it should be void");
    return true;
}
boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const* const p)
{
    if (not p)
        return dep0::testing::failure("instruction is null");
    return is_return_of_void(*p);
}

boost::test_tools::predicate_result is_return_of_void(llvm::Value const& x)
{
    auto const instr = dyn_cast<llvm::Instruction>(&x);
    if (not instr)
        return dep0::testing::failure("value is not an instruction but: ValueID=", x.getValueID());
    return is_return_of_void(*instr);
}

boost::test_tools::predicate_result is_return_of_void(llvm::Value const* const p)
{
    if (not p)
        return dep0::testing::failure("value is null");
    return is_return_of_void(*p);
}

} // namespace dep0::llvmgen::testing::v2::impl
