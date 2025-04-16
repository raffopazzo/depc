/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Set of functions to generate LLVM loops.
 */
#pragma once

#include "private/context.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <functional>

namespace dep0::llvmgen {

/** @brief Generator of the loop body, which takes the iterator variable as argument. */
using gen_loop_body_t = std::function<void(global_ctx_t&, local_ctx_t&, llvm::IRBuilder<>&, llvm::Value*)>;

/** @brief Generate an unrolled for-loop counting downwards from an initial value down to a sentinel value. */
void gen_for_loop_downward_unrolled(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    std::size_t initial_value,
    std::size_t sentinel_value,
    gen_loop_body_t);

/**
 * @brief Generate a for-loop that iterates downwards from an initial value down to a sentinel value.
 * @param sign  Specifies whether initial and sentinel values are signed or unsigned integers.
 */
void gen_for_loop_downward(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    ast::sign_t sign,
    llvm::Value* initial_value,
    llvm::Value* sentinel_value,
    gen_loop_body_t);

/** @brief Generate an unrolled for-loop counting upwards from an initial value up to a sentinel value. */
void gen_for_loop_upward_unrolled(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    std::size_t initial_value,
    std::size_t sentinel_value,
    gen_loop_body_t);

/**
 * @brief Generate a for-loop that iterates upwards from an initial value up to a sentinel value.
 * @param sign  Specifies whether initial and sentinel values are signed or unsigned integers.
 */
void gen_for_loop_upward(
    global_ctx_t&,
    local_ctx_t&,
    llvm::IRBuilder<>&,
    ast::sign_t sign,
    llvm::Value* initial_value,
    llvm::Value* sentinel_value,
    gen_loop_body_t);

} // namespace dep0::llvmgen
