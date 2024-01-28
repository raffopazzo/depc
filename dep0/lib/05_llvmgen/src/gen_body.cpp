#include "private/gen_body.hpp"

#include "private/gen_val.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

static bool has_unit_type(typecheck::expr_t const& expr)
{
    auto const type = std::get_if<typecheck::expr_t>(&expr.properties.sort.get());
    return type and std::holds_alternative<typecheck::expr_t::unit_t>(type->value);
}

snippet_t gen_body(
    global_context_t& global,
    local_context_t const& local,
    typecheck::body_t const& body,
    std::string_view const entry_block_name,
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(global.llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(global.llvm_ctx, entry_block_name, llvm_f));
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
    return snippet;
}

void gen_stmt(
    global_context_t& global,
    local_context_t const& local,
    snippet_t& snipppet,
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
            auto const cond = gen_val(global, local, builder, x.cond, nullptr);
            auto const then = gen_body(global, local, x.true_branch, "then", llvm_f, inlined_result);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto const else_ = gen_body(global, local, *x.false_branch, "else", llvm_f, inlined_result);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snipppet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto const else_ = llvm::BasicBlock::Create(global.llvm_ctx, "else", llvm_f);
                builder.CreateCondBr(cond, then.entry_block, else_);
                snipppet.open_blocks.push_back(else_);
            }
        },
        [&] (typecheck::stmt_t::return_t const& x)
        {
            if (not x.expr)
            {
                // only unit_t can be missing a return expr
                builder.CreateRet(builder.getInt8(0));
                return;
            }
            if (inlined_result)
            {
                gen_val(global, local, builder, *x.expr, inlined_result);
                snipppet.open_blocks.push_back(builder.GetInsertBlock());
                return;
            }
            auto const arg0 = llvm_f->arg_empty() ? nullptr : llvm_f->getArg(0ul);
            auto const dest = arg0 and arg0->hasAttribute(llvm::Attribute::StructRet) ? arg0 : nullptr;
            // always generate a value, even for expressions of type unit_t,
            // because it might be a function call with side effects;
            // and, if it is, just return 0, without complicating the CFG
            auto const ret_val = gen_val(global, local, builder, *x.expr, dest);
            if (dest)
                builder.CreateRetVoid();
            else if (has_unit_type(*x.expr))
                builder.CreateRet(builder.getInt8(0));
            else
                builder.CreateRet(ret_val);
        });
}

} // namespace dep0::llvmgen
