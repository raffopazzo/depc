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
 * @brief Type of allocator to use when allocating new memory, typically from `gen_alloca()`.
 * @remarks For now only stack and heap (via malloc) are available but it might be
 * possible to extend this one day to allow custom allocators.
 */
enum class allocator_t
{
    stack,
    heap
};

/**
 * @brief Generates instructions to allocate 1 value of the given type, for example `array_t(i32_t, n)`.
 *
 * Memory is allocated either on the stack or on the heap according to the given allocator type.
 * For stack allocations, an `alloca` instruction is generated and placed at the top of the
 * entry block of the current LLVM function (inferred from the IR builder).
 * For heap allocations, a run-time call to `malloc()` is generated instead;
 * it is the caller responsibility to generate a run-time call to `free()`.
 */
llvm::Value* gen_alloca(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    allocator_t,
    typecheck::expr_t const& type);

/**
 * @brief Move the given `alloca` to the entry block of the given function, if possible.
 *
 * An `alloca` cannot be moved before any of its operands,
 * for example if the size is the multiplication of two other values,
 * the `alloca` cannot be moved before computing the result.
 * If the `alloca` cannot be moved, this function does nothing.
 *
 * This is recommended by LLVM guide in order to make the `mem2reg` pass effective.
 * As per their guide, the `alloca` needs to also appear before any `call` instructions.
 * So we put it right after the most recent `alloca`, thus creating a sort of "alloca group".
 *
 * @remarks This function is called automatically from `gen_alloca()` but,
 * if you manually generate an `alloca`, it is highly recommended that you call this function.
 */
void try_move_to_entry_block(llvm::AllocaInst*, llvm::Function*);

} // namespace dep0::llvmgen
