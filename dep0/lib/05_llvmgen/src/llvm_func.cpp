/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/llvm_func.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/Transforms/Utils/Local.h>

#include <vector>

namespace dep0::llvmgen {

llvm_func_t::llvm_func_t(llvm::Function* const f) :
    type(f->getFunctionType()),
    func(f)
{ }

llvm_func_t::llvm_func_t(llvm::FunctionType* const type, llvm::Value* const func) :
    type(type),
    func(func)
{ }

/**
 * @brief Move all `alloca` instructions to the entry block, except those that depend on intermediate values.
 *
 * This is recommended by LLVM guide in order to make the `mem2reg` pass effective.
 * As per their guide, `alloca` instructions need to also appear before any `call` instructions.
 * So we put them all right at the beginning of the entry block, thus forming a sort of "alloca group".
 * If the entry block already contains an alloca group, all instructions that are moved will be appended to the group.
 *
 * @remarks
 * All candidate `alloca` instructions will be moved, including those appearing inside loops.
 * It is not currently clear whether this is a sensible decision or not.
 */
static void move_allocas_to_entry_block(llvm::Function* const llvm_f)
{
    auto const should_be_moved = [] (llvm::AllocaInst const* const alloca)
    {
        auto const size = alloca->getArraySize();
        // I don't know if array-size could be null but, if it is, it would just mean 1 by default.
        return size == nullptr or llvm::isa<llvm::Argument>(size) or llvm::isa<llvm::ConstantInt>(size);
    };
    llvm::AllocaInst* last_alloca = nullptr;
    std::vector<llvm::AllocaInst*> allocas_to_move;
    for (bool alloca_group = true; auto& block: llvm_f->getBasicBlockList())
    {
        for (auto& inst: block)
        {
            if (auto const p = llvm::dyn_cast<llvm::AllocaInst>(&inst))
            {
                if (alloca_group)
                    last_alloca = p;
                else if (should_be_moved(p))
                    allocas_to_move.push_back(p);
            }
            else
                alloca_group = false;
        }
        // we assume to work on valid blocks, i.e. blocks with a terminator, so we must have seen a non-alloca
        assert(not alloca_group and "entry block does not have a terminator");
    }
    for (auto const alloca: allocas_to_move)
    {
        if (last_alloca)
            alloca->moveAfter(last_alloca);
        else
            alloca->moveBefore(&llvm_f->getEntryBlock().front());
        last_alloca = alloca;
    }
}

/**
 * @brief Remove unused values and instructions that have no side effects.
 *
 * Removing dead instructions may lead to more dead instructions,
 * so this transformation is applied recursively until no more dead instructions are present.
 */
static void remove_dead_instructions(llvm::Function* const llvm_f)
{
    std::vector<llvm::Instruction*> dead_instructions;
    do
    {
        dead_instructions.clear();
        for (auto& block: llvm_f->getBasicBlockList())
            for (auto& inst: block)
                if (llvm::isInstructionTriviallyDead(&inst))
                    dead_instructions.push_back(&inst);
        for (auto p: dead_instructions)
            p->eraseFromParent();
    }
    while (not dead_instructions.empty());
}

void finalize_llvm_func(llvm::Function* const llvm_f)
{
    remove_dead_instructions(llvm_f);
    move_allocas_to_entry_block(llvm_f);
}

} // namespace dep0::llvmgen
