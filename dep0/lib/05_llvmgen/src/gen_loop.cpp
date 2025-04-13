/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_loop.hpp"

namespace dep0::llvmgen {

void gen_for_loop_downward_unrolled(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    std::size_t initial_value,
    std::size_t const sentinel_value,
    gen_loop_body_t gen_body)
{
    while (initial_value-- > sentinel_value)
        gen_body(global, local, builder, builder.getInt32(initial_value));
}

void gen_for_loop_downward(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    ast::sign_t const sign,
    llvm::Value* const initial_value,
    llvm::Value* const sentinel_value,
    gen_loop_body_t gen_body)
{
    if (sign == ast::sign_t::unsigned_v)
        if (auto const a = llvm::dyn_cast<llvm::ConstantInt>(initial_value))
            if (auto const b = llvm::dyn_cast<llvm::ConstantInt>(sentinel_value))
                if (a->getZExtValue() - b->getZExtValue() < 2ul)
                    return gen_for_loop_downward_unrolled(
                        global, local, builder, a->getZExtValue(), b->getZExtValue(), gen_body);
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
    gen_body(global, local, builder, next_value);
    auto const cmp = builder.CreateCmp(predicate, next_value, sentinel_value);
    builder.CreateCondBr(cmp, loop, next);
    builder.SetInsertPoint(next);
}

void gen_for_loop_upward_unrolled(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    std::size_t initial_value,
    std::size_t const sentinel_value,
    gen_loop_body_t gen_body)
{
    while (initial_value < sentinel_value)
        gen_body(global, local, builder, builder.getInt32(initial_value++));
}

void gen_for_loop_upward(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    ast::sign_t const sign,
    llvm::Value* const initial_value,
    llvm::Value* const sentinel_value,
    gen_loop_body_t gen_body)
{
    if (sign == ast::sign_t::unsigned_v)
        if (auto const a = llvm::dyn_cast<llvm::ConstantInt>(initial_value))
            if (auto const b = llvm::dyn_cast<llvm::ConstantInt>(sentinel_value))
                if (b->getZExtValue() - a->getZExtValue() < 2ul)
                    return gen_for_loop_upward_unrolled(
                        global, local, builder, a->getZExtValue(), b->getZExtValue(), gen_body);
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
    gen_body(global, local, builder, phi);
    auto const cmp = builder.CreateCmp(predicate, next_value, sentinel_value);
    builder.CreateCondBr(cmp, loop, next);
    builder.SetInsertPoint(next);
}

} // namespace dep0::llvmgen
