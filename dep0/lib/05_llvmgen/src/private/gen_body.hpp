#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <string_view>
#include <vector>

namespace dep0::llvmgen {

/**
 * Snippet is a model of a block of IR code that is being built,
 * could be for example from the body of a function definition or the body of an if-else branch.
 * It contains an entry point and possibly many open blocks, i.e. blocks currently without a terminator.
 */
struct snippet_t
{
    llvm::BasicBlock* entry_block;
    std::vector<llvm::BasicBlock*> open_blocks;

    /**
     * Iterate over all currently open blocks and invoke the given function to close them all.
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
 * Generate IR code for a body of DepC statements.
 * This function will use a fresh IR builder every time.
 *
 * @param entry_block_name
 *      The name for the entry block of the generated snippet of IR code.
 *
 * @param parent_function
 *      All generated blocks will be added to this LLVM function.
 *
 * @param inlined_result
 *      If not nullptr and if this body contains return statements,
 *      this function will emit appropriate IR instructions to
 *      store/memcpy/memset the resulting LLVM value at the runtime location
 *      pointed by this LLVM value, which must be of pointer type.
 *
 * @return
 *      A snippet object containing the entry block of the generated IR code along with any open blocks.
 *      It is the caller responsibility to close all open blocks before finalizing the codegen phase.
 */
snippet_t gen_body(
    global_ctx_t&,
    local_ctx_t const& ,
    typecheck::body_t const&,
    std::string_view entry_block_name,
    llvm::Function* parent_function,
    llvm::Value* inlined_result);

/**
 * Generate IR code for a DepC statement.
 * This function will use the given IR builder, instead of a new one.
 *
 * @param current_snippet
 *      If this function produces new open blocks, they will be appended to this snippet.
 *
 * @param builder
 *      The builder used to generate IR code.
 *
 * @param parent_function
 *      All generated blocks will be added to this LLVM function.
 *
 * @param inlined_result
 *      If not nullptr and If this is a return statement,
 *      this function will emit appropriate IR instructions to
 *      store/memcpy/memset the resulting LLVM value at the runtime location
 *      pointed by this LLVM value, which must be of pointer type.
 */
void gen_stmt(
    global_ctx_t&,
    local_ctx_t const&,
    snippet_t& current_snippet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const&,
    llvm::Function* parent_function,
    llvm::Value* inlined_result);

} // namespace dep0::llvmgen
