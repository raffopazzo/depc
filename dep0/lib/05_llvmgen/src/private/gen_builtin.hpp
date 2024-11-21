/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * If the given function application invokes a builtin function,
 * generates an LLVM value corresponding to that builtin function;
 * otherwise returns `nullptr`.
 *
 * @param dest
 *      If this parameter is not nullptr, this function will emit appropriate IR instructions
 *      to store/memcpy/memset the result of the function call at the runtime location pointed by
 *      this LLVM value, which must therefore be of an appropriate pointer type.
 *
 * @return The generated LLVM value (or `dest` if it was not nullptr) or nullptr if not a builtin function.
 */
llvm::Value* try_gen_builtin(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&,
    llvm::Value* dest);

} // namespace dep0::llvmgen
