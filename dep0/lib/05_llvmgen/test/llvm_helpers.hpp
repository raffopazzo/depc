/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>

#include <vector>

/** Return all basic blocks from the given function. */
std::vector<llvm::BasicBlock const*> get_blocks(llvm::Function const&);

/** Return all instructinos from the given basic block. */
std::vector<llvm::Instruction const*> get_instructions(llvm::BasicBlock const&);
