#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * Like `llvm::FunctionCallee`, it represents a callable function together with its type.
 * But unlike it, this allows access to the two fields via const-ref.
 * If upstream fixes their API, we can remove this and use theirs instead.
 */
struct llvm_func_t
{
    llvm::FunctionType* type;
    llvm::Value* func;

    explicit llvm_func_t(llvm::Function*);
    llvm_func_t(llvm::FunctionType*, llvm::Value*);
};

} // namespace dep0::llvmgen
