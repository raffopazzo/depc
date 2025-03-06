/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief SIngle-function header declaring `dep0::llvmgen::try_gen_builtin()`.
 */
#pragma once

#include "private/context.hpp"
#include "private/gen_val.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace dep0::llvmgen {

/**
 * @brief Generate an LLVM value corresponding to a builtin function call.
 *
 * If the given function application invokes a builtin function,
 * generates an LLVM value corresponding to that builtin function;
 * otherwise returns `nullptr`.
 *
 * @return The generated LLVM value (or its location if not using temporary storage)
 * or `nullptr` if the run-time call is not to a builtin function.
 */
llvm::Value* try_gen_builtin(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t::app_t const&,
    value_storage_t);

} // namespace dep0::llvmgen
