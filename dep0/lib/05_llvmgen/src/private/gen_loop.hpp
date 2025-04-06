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

namespace dep0::llvmgen {

template <typename F>
void gen_for_loop_downward_unrolled(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    std::size_t initial_value,
    std::size_t const sentinel_value,
    F&& body_gen)
{
    while (initial_value-- > sentinel_value)
        body_gen(global, local, builder, builder.getInt32(initial_value));
}

/**
 * @brief Generate a for-loop that iterates downwards from an initial value down to a sentinel value.
 *
 * @param body_gen  Generator of the loop body, which takes the iterator variable as argument.
 */
template <typename F>
void gen_for_loop_downward(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    ast::sign_t const sign,
    llvm::Value* const initial_value,
    llvm::Value* const sentinel_value,
    F&& body_gen)
{
    if (sign == ast::sign_t::unsigned_v)
        if (auto const a = llvm::dyn_cast<llvm::ConstantInt>(initial_value))
            if (auto const b = llvm::dyn_cast<llvm::ConstantInt>(sentinel_value))
                if (a->getZExtValue() - b->getZExtValue() < 2ul)
                    return gen_for_loop_downward_unrolled(
                        global, local, builder, a->getZExtValue(), b->getZExtValue(), body_gen);
    auto const header = builder.GetInsertBlock();
    auto const loop = llvm::BasicBlock::Create(global.llvm_ctx, "loop", header->getParent());
    auto const next = llvm::BasicBlock::Create(global.llvm_ctx, "next", header->getParent());
    auto const predicate = sign == ast::sign_t::signed_v ? llvm::CmpInst::ICMP_SGT : llvm::CmpInst::ICMP_UGT;
    auto const cmp0 = builder.CreateCmp(predicate, initial_value, sentinel_value);
    builder.CreateCondBr(cmp0, loop, next);
    builder.SetInsertPoint(loop);
    auto const value_type = initial_value->getType();
    auto const phi = builder.CreatePHI(value_type, 2);
    phi->addIncoming(initial_value, header);
    auto const next_value = builder.CreateSub(phi, llvm::ConstantInt::get(value_type, 1));
    phi->addIncoming(next_value, loop);
    body_gen(global, local, builder, next_value);
    auto const cmp = builder.CreateCmp(predicate, next_value, sentinel_value);
    builder.CreateCondBr(cmp, loop, next);
    builder.SetInsertPoint(next);
}

template <typename F>
void gen_for_loop_upward_unrolled(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    std::size_t initial_value,
    std::size_t const sentinel_value,
    F&& body_gen)
{
    while (initial_value < sentinel_value)
        body_gen(global, local, builder, builder.getInt32(initial_value++));
}

/**
 * @brief Generate a for-loop that iterates upwards from an initial value up to a sentinel value.
 *
 * @param body_gen  Generator of the loop body, which takes the iterator variable as argument.
 */
template <typename F>
void gen_for_loop_upward(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    ast::sign_t const sign,
    llvm::Value* const initial_value,
    llvm::Value* const sentinel_value,
    F&& body_gen)
{
    if (sign == ast::sign_t::unsigned_v)
        if (auto const a = llvm::dyn_cast<llvm::ConstantInt>(initial_value))
            if (auto const b = llvm::dyn_cast<llvm::ConstantInt>(sentinel_value))
                if (b->getZExtValue() - a->getZExtValue() < 2ul)
                    return gen_for_loop_upward_unrolled(
                        global, local, builder, a->getZExtValue(), b->getZExtValue(), body_gen);
    auto const header = builder.GetInsertBlock();
    auto const loop = llvm::BasicBlock::Create(global.llvm_ctx, "loop", header->getParent());
    auto const next = llvm::BasicBlock::Create(global.llvm_ctx, "next", header->getParent());
    auto const predicate = sign == ast::sign_t::signed_v ? llvm::CmpInst::ICMP_SLT : llvm::CmpInst::ICMP_ULT;
    auto const cmp0 = builder.CreateCmp(predicate, initial_value, sentinel_value);
    builder.CreateCondBr(cmp0, loop, next);
    builder.SetInsertPoint(loop);
    auto const value_type = initial_value->getType();
    auto const phi = builder.CreatePHI(value_type, 2);
    phi->addIncoming(initial_value, header);
    auto const next_value = builder.CreateAdd(phi, llvm::ConstantInt::get(value_type, 1));
    phi->addIncoming(next_value, loop);
    body_gen(global, local, builder, phi);
    auto const cmp = builder.CreateCmp(predicate, next_value, sentinel_value);
    builder.CreateCondBr(cmp, loop, next);
    builder.SetInsertPoint(next);
}

} // namespace dep0::llvmgen
