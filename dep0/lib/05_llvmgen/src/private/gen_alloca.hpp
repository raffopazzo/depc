/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions useful to generate the LLVM `alloca` instruction, when necessary.
 */
#pragma once

#include "private/context.hpp"
#include "private/gen_array.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <variant>

namespace dep0::llvmgen {

/** @brief Contains the possible results returned by `needs_alloca()` */
namespace needs_alloca_result
{
    /** @brief The type passed to `needs_alloca()` does not require an allocation. */
    struct no_t{};

    /** @brief The type passed to `needs_alloca()` requires an allocation because it is a tuple `(t1; ...; tN)`. */
    struct sigma_t
    {
        std::vector<typecheck::func_arg_t> const& args;
    };

    /** @brief The type passed to `needs_alloca()` requires an allocation because it is an array. */
    struct array_t
    {
        array_properties_view_t properties;
    };
}
using needs_alloca_result_t =
    std::variant<
        needs_alloca_result::no_t,
        needs_alloca_result::sigma_t,
        needs_alloca_result::array_t>;

/**
 * @brief Decides whether the given type expression requires an allocation or not.
 * @param type The type expression to check, which must have sort `typename_t`.
 */
needs_alloca_result_t needs_alloca(typecheck::expr_t const& type);

/**
 * @brief Checks whether the given type expression requires an allocation or not.
 * @param type The type expression to check, which must have sort `typename_t`.
 * @return True if allocation is required for the given type, false otherwise.
 */
bool is_alloca_needed(typecheck::expr_t const& type);

/**
 * @brief Generates an `alloca` instruction, but only if the input type requires an allocation.
 *
 * @param type
 *      A type expression that may or may not require an allocation;
 *      for example `array_t(i32_t, n)` does but `i32_t` on its own does not.
 *
 * @return The generated `alloca` instruction, or `nullptr` if allocation is not required for the given type.
 * 
 * @remarks The new `alloca` instruction is placed at the top of the entry block of the current LLVM function.
 * @see @ref `move_to_entry_block()`.
 */
llvm::AllocaInst* gen_alloca_if_needed(
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
 * @remarks This function is called automatically from `gen_alloca_if_needed()` but,
 * if you manually generate an `alloca`, it is highly recommended that you call this function.
 */
void move_to_entry_block(llvm::AllocaInst*, llvm::Function*);

} // namespace dep0::llvmgen
