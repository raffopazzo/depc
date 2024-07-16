#include "private/gen_body.hpp"

#include "private/gen_val.hpp"

#include "dep0/typecheck/is_impossible.hpp"
#include "dep0/typecheck/is_mutable.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

static bool has_unit_type(typecheck::expr_t const& expr)
{
    auto const type = std::get_if<typecheck::expr_t>(&expr.properties.sort.get());
    return type and std::holds_alternative<typecheck::expr_t::unit_t>(type->value);
}

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
 *      If not `nullptr` and if this is a return statement,
 *      this function will emit appropriate IR instructions to
 *      store/memcpy/memset the resulting LLVM value at the runtime location
 *      pointed by this LLVM value, which must be of pointer type.
 */
static void gen_stmt(
    global_ctx_t&,
    local_ctx_t const&,
    snippet_t& current_snippet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const&,
    llvm::Function* parent_function,
    llvm::Value* inlined_result);

static void gen_stmts(
    global_ctx_t& global,
    local_ctx_t const& local,
    snippet_t& snippet,
    llvm::IRBuilder<>& builder,
    typecheck::body_t const& body,
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    auto it = body.stmts.begin();
    if (it == body.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(global, local, snippet, builder, *it, llvm_f, inlined_result);
        while (++it != body.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(global.llvm_ctx, "next", llvm_f);
                snippet.seal_open_blocks(builder, [next] (auto& builder) { builder.CreateBr(next); });
                builder.SetInsertPoint(next);
            }
            gen_stmt(global, local, snippet, builder, *it, llvm_f, inlined_result);
        }
    }
}


snippet_t gen_body(
    global_ctx_t& global,
    local_ctx_t const& local,
    typecheck::body_t const& body,
    std::string_view const entry_block_name,
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(global.llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(global.llvm_ctx, entry_block_name, llvm_f));
    if (is_impossible(body))
    {
        builder.CreateUnreachable();
        return snippet;
    }
    gen_stmts(global, local, snippet, builder, body, llvm_f, inlined_result);
    return snippet;
}

void gen_stmt(
    global_ctx_t& global,
    local_ctx_t const& local,
    snippet_t& snippet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& stmt,
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(global, local, builder, x, nullptr);
        },
        [&] (typecheck::stmt_t::if_else_t const& x)
        {
            // Let's eliminate impossible branches.
            // Note that at least one branch must be possible.
            // Because, if both branches were impossible, `gen_body` would emit `unreachable` and we wouldn't be here.
            // Now, if only one branch is impossible, we can just generate code for the other in the current snippet.
            // If so, we need to be careful to generate code for the condition too, unless it's immutable.
            if (is_impossible(x.true_branch))
            {
                if (is_mutable(x.cond))
                    gen_val(global, local, builder, x.cond, nullptr);
                if (x.false_branch)
                    gen_stmts(global, local, snippet, builder, *x.false_branch, llvm_f, inlined_result);
                return;
            }
            if (x.false_branch and is_impossible(*x.false_branch))
            {
                if (is_mutable(x.cond))
                    gen_val(global, local, builder, x.cond, nullptr);
                gen_stmts(global, local, snippet, builder, x.true_branch, llvm_f, inlined_result);
                return;
            }
            // Otherwise both branches are possible and we need to emit each one in their own basic block.
            auto const cond = gen_val(global, local, builder, x.cond, nullptr);
            auto const then = gen_body(global, local, x.true_branch, "then", llvm_f, inlined_result);
            std::ranges::copy(then.open_blocks, std::back_inserter(snippet.open_blocks));
            if (x.false_branch)
            {
                auto const else_ = gen_body(global, local, *x.false_branch, "else", llvm_f, inlined_result);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snippet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto const else_ = llvm::BasicBlock::Create(global.llvm_ctx, "else", llvm_f);
                builder.CreateCondBr(cond, then.entry_block, else_);
                snippet.open_blocks.push_back(else_);
            }
        },
        [&] (typecheck::stmt_t::return_t const& x)
        {
            if (not x.expr)
            {
                // only unit_t can be missing a return expr
                builder.CreateRet(gen_val_unit(global));
                return;
            }
            if (inlined_result)
            {
                gen_val(global, local, builder, *x.expr, inlined_result);
                snippet.open_blocks.push_back(builder.GetInsertBlock());
                return;
            }
            auto const arg0 = llvm_f->arg_empty() ? nullptr : llvm_f->getArg(0ul);
            auto const dest = arg0 and arg0->hasAttribute(llvm::Attribute::StructRet) ? arg0 : nullptr;
            // always generate a value, even for expressions of type unit_t,
            // because it might be a function call with side effects;
            // and, if it is, just return the unit value, without complicating the CFG
            auto const ret_val = gen_val(global, local, builder, *x.expr, dest);
            if (dest)
                builder.CreateRetVoid();
            else if (has_unit_type(*x.expr))
                builder.CreateRet(gen_val_unit(global));
            else
                builder.CreateRet(ret_val);
        },
        [&] (typecheck::stmt_t::impossible_t const&)
        {
            builder.CreateUnreachable();
        });
}

} // namespace dep0::llvmgen
