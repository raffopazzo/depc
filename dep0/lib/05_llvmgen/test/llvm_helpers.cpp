/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "llvm_helpers.hpp"

std::vector<llvm::BasicBlock const*> get_blocks(llvm::Function const& f)
{
    std::vector<llvm::BasicBlock const*> blocks;
    blocks.reserve(f.size());
    for (llvm::BasicBlock const& b: f)
        blocks.push_back(&b);
    return blocks;
}

std::vector<llvm::Instruction const*> get_instructions(llvm::BasicBlock const& b)
{
    std::vector<llvm::Instruction const*> instructions;
    instructions.reserve(b.size());
    for (llvm::Instruction const& x: b)
        instructions.push_back(&x);
    return instructions;
}

