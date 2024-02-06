#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen::testing {

std::string to_string(llvm::Type const&);
std::string to_string(llvm::Value const&);

} // namespace dep0::llvmgen::testing
