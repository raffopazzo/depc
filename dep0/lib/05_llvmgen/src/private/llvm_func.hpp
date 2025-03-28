/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::llvmgen::llvm_func_t`.
 */
#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * @brief Like `llvm::FunctionCallee`, it represents a callable function together with its type.
 *
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

/**
 * @brief Performs a standard set of transformations on the given function.
 *
 * Examples of transformations performed on the function include moving all `alloca` instructions
 * to the beginning of the entry block and removing dead instructions.
 *
 * @remarks This should be invoked after all instructions in the function have been generated.
 */
void finalize_llvm_func(llvm::Function*);

} // namespace dep0::llvmgen
