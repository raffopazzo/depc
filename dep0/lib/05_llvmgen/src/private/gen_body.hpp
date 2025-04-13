/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Types and functions useful when generating LLVM IR for a body of DepC statements.
 */
#pragma once

#include "private/context.hpp"
#include "private/gen_val.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <string_view>
#include <optional>
#include <vector>

namespace dep0::llvmgen {

/**
 * @brief Snippet of LLVM IR code comprised of one or more basic blocks.
 *
 * For example the body of a function definition or an if-else branch.
 *
 * Only the entry block is stored here.
 * If the snippet is comprised of more basic blocks they will be linked
 * to one another and somehow reachable from the entry block.
 *
 * It may also contain open blocks, i.e. blocks currently without a terminator.
 * It is the caller responsibility to terminate all open blocks as necessary.
 * Once an open block is terminated, it is removed from the list of open blocks.
 */
struct snippet_t
{
    llvm::BasicBlock* entry_block;
    std::vector<llvm::BasicBlock*> open_blocks;

    /**
     * @brief Iterate over all currently open blocks and invoke the given function to close them all.
     *
     * @param builder
     *      This builder will be passed to the callback function;
     *      before every function call, its insert point will be set to the next open block.
     *
     * @param f
     *      This function will be called on each open block and must emit a terminator instruction.
     */
    template <typename F>
    void seal_open_blocks(llvm::IRBuilder<>& builder, F&& f)
    {
        for (auto& bb: open_blocks)
        {
            assert(not bb->getTerminator());
            builder.SetInsertPoint(bb);
            f(builder);
            assert(bb->getTerminator());
        }
        open_blocks.clear();
    }
};

/**
 * @brief If a function body is being inlined, this struct will contain the runtime location where
 * the result value of the inlined function must be stored and its value category.
 *
 * For example, `if (inlined_function())` will store a `temporary` value at some runtime location,
 * whilst `return inlined_function()` will store a `result` value at that location.
 */
struct inlined_result_t
{
    value_category_t value_category;
    llvm::Value* dest;
};

/**
 * @brief Generate IR code for a body of DepC statements.
 * @remarks This function will use a fresh IR builder every time.
 * In other words, each nested invocation to generate a nested body will use a new IR builder.
 *
 * @param entry_block_name
 *      The name for the entry block of the generated snippet of IR code.
 *
 * @param parent_function
 *      All generated blocks will be added to this LLVM function.
 *
 * @param inlined_result
 *      If not `nullptr` and if this body contains `return` statements,
 *      this function will emit appropriate IR instructions to
 *      `store/memcpy/memset` the resulting LLVM value at the runtime location
 *      pointed by this LLVM value, which must be of pointer type.
 *      Any run-time memory allocation will be performed on the stack.
 *
 * @return
 *      A snippet object containing the entry block of the generated IR code along with any open blocks.
 *      It is the caller responsibility to close all open blocks before finalizing the codegen phase.
 */
snippet_t gen_body(
    global_ctx_t&,
    local_ctx_t const&,
    typecheck::body_t const&,
    std::string_view entry_block_name,
    llvm::Function* parent_function,
    std::optional<inlined_result_t>);

} // namespace dep0::llvmgen
