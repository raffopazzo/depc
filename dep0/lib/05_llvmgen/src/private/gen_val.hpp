#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

llvm::Value* gen_val(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const&,
    llvm::Value* dest);
llvm::Value* gen_func_call(
    global_context_t&,
    local_context_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&,
    llvm::Value* dest);

} // namespace dep0::llvmgen
