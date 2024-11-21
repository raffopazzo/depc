/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
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
