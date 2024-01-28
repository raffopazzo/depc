#include "llvm_predicates_v2/to_string.hpp"

namespace dep0::llvmgen::testing::v2 {

std::string to_string(llvm::Type const& x)
{
    std::string result;
    llvm::raw_string_ostream s(result);
    x.print(s);
    return result;
}

std::string to_string(llvm::Value const& x)
{
    std::string result;
    llvm::raw_string_ostream s(result);
    x.print(s);
    return result;
}

} // namespace dep0::llvmgen::testing
