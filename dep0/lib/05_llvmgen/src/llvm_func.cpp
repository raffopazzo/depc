#include "private/llvm_func.hpp"

namespace dep0::llvmgen {

llvm_func_t::llvm_func_t(llvm::Function* const f) :
    type(f->getFunctionType()),
    func(f)
{ }

llvm_func_t::llvm_func_t(llvm::FunctionType* const type, llvm::Value* const func) :
    type(type),
    func(func)
{ }

} // namespace dep0::llvmgen
