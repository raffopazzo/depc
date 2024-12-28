/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions useful to generate and manipulate the LLVM `alloca` instruction.
 */
#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <variant>

namespace dep0::llvmgen {

/**
 * @brief Generates an `alloca` instruction for the given type, for example `array_t(i32_t, n)`.
 * @param type The most common example would be some `array_t(i32_t, n)` but this also works for a simple `i32_t`.
 * @remarks The new `alloca` instruction is placed at the top of the entry block of the current LLVM function.
 * @see @ref `move_to_entry_block()`.
 */
llvm::AllocaInst* gen_alloca(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    typecheck::expr_t const& type);

/**
 * @brief Move the given `alloca` to the entry block of the given function.
 * 
 * This is recommended by LLVM guide in order to make the `mem2reg` pass effective.
 * As per their guide, the `alloca` needs to also appear before any `call` instructions.
 * So we put it right after the most recent `alloca`, thus creating a sort of "alloca group".
 * 
 * @remarks This function is called automatically from `gen_alloca()` but,
 * if you manually generate an `alloca`, it is highly recommended that you call this function.
 */
void move_to_entry_block(llvm::AllocaInst*, llvm::Function*);

} // namespace dep0::llvmgen
