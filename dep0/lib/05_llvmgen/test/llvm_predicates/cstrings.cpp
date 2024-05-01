#include "llvm_predicates/cstrings.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Constants.h>

using namespace dep0::testing;

namespace dep0::llvmgen::testing::impl {

boost::test_tools::predicate_result is_cstr(llvm::Value const& x, std::string_view const v)
{
    auto const c = dyn_cast<llvm::ConstantDataSequential>(&x);
    if (not c)
        return failure("value is not a ConstantDataSequential but: ", to_string(x));
    if (not c->isCString())
        return failure("value is not a cstring");
    if (auto const& s = c->getAsCString().str(); s != v)
        return failure(s, " != ", v);
    return true;
}

boost::test_tools::predicate_result is_cstr(llvm::Value const* const p, std::string_view const v)
{
    if (not p)
        return failure("value is null");
    return is_cstr(*p, v);
}

} // namespace dep0::llvmgen::testing::impl
