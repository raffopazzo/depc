#include "llvm_predicates/zeroinitializer.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Constants.h>

using namespace dep0::testing;

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_zeroinitializer(llvm::Value const& x)
{
    auto const c = dyn_cast<llvm::ConstantAggregateZero>(&x);
    if (not c)
        return failure("value is not a zeroinitializer but: ", to_string(x));
    return true;
}

boost::test_tools::predicate_result is_zeroinitializer(llvm::Value const* const p)
{
    if (not p)
        return failure("value is null");
    return is_zeroinitializer(*p);
}

} // namespace dep0::llvmgen::testing::impl
