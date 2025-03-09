/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_body.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/typecheck/is_impossible.hpp"
#include "dep0/typecheck/is_mutable.hpp"

#include "dep0/match.hpp"

#include <ranges>

namespace dep0::llvmgen {

static bool has_unit_type(typecheck::expr_t const& expr)
{
    auto const type = std::get_if<typecheck::expr_t>(&expr.properties.sort.get());
    return type and std::holds_alternative<typecheck::expr_t::unit_t>(type->value);
}

/**
 * @brief Generate IR code for a DepC statement.
 *
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

/**
 * @brief Generate the destructor for a value of the given type.
 *
 * Destructors are generated in reverse order and recursively inside-out.
 */
static void gen_destructor(
    global_ctx_t const& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    llvm::Value* const value,
    typecheck::expr_t const& type)
{
    assert(not is_trivially_destructible(global, type) and "trivially destructible types do not need a destructor");
    match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) { },
        [] (typecheck::expr_t::true_t const&) { },
        [] (typecheck::expr_t::auto_t const&) { },
        [] (typecheck::expr_t::bool_t const&) { },
        [] (typecheck::expr_t::cstr_t const&) { },
        [] (typecheck::expr_t::unit_t const&) { },
        [] (typecheck::expr_t::i8_t const&) { },
        [] (typecheck::expr_t::i16_t const&) { },
        [] (typecheck::expr_t::i32_t const&) { },
        [] (typecheck::expr_t::i64_t const&) { },
        [] (typecheck::expr_t::u8_t const&) { },
        [] (typecheck::expr_t::u16_t const&) { },
        [] (typecheck::expr_t::u32_t const&) { },
        [] (typecheck::expr_t::u64_t const&) { },
        [] (typecheck::expr_t::boolean_constant_t const&) { },
        [] (typecheck::expr_t::numeric_constant_t const&) { },
        [] (typecheck::expr_t::string_literal_t const&) { },
        [] (typecheck::expr_t::boolean_expr_t const&) { },
        [] (typecheck::expr_t::relation_expr_t const&) { },
        [] (typecheck::expr_t::arith_expr_t const&) { },
        [] (typecheck::expr_t::var_t const&) { },
        [&] (typecheck::expr_t::global_t const& g)
        {
            auto const val = global[g];
            assert(val and "unknown type");
            match(
                *val,
                [] (llvm_func_t const&) { },
                [] (typecheck::type_def_t const&) { });
        },
        [&] (typecheck::expr_t::app_t const& app)
        {
            match(
                app.func.get().value,
                [] (typecheck::expr_t::true_t const&) { },
                [&] (typecheck::expr_t::array_t const&)
                {
                    // TODO needs a test: iterate over all items and invoke their destructor
                },
                [] (auto const&) { });
        },
        [] (typecheck::expr_t::abs_t const&) { },
        [] (typecheck::expr_t::pi_t const& x) { /* TODO this will change once we introduce closures */ },
        [&] (typecheck::expr_t::sigma_t const& x)
        {
            auto const gep = [&, llvm_type = gen_type(global, type), zero = builder.getInt32(0)] (std::size_t const i)
            {
                return builder.CreateGEP(llvm_type, value, {zero, builder.getInt32(i)});
            };
            for (auto const i: std::views::reverse(std::views::iota(0ul, x.args.size())))
                if (is_boxed(x.args[i].type))
                {
                    auto const free =
                        llvm::CallInst::CreateFree(
                            builder.CreateLoad(gen_type(global, x.args[i].type), gep(i)),
                            builder.GetInsertBlock());
                    builder.GetInsertBlock()->getInstList().push_back(free);
                }
                // TODO needs a test:
                // else if (not is_trivially_destructible(global, x.args[i].type))
                //     gen_destructor(global, local, builder, gep(i), x.args[i].type);
        },
        [] (typecheck::expr_t::array_t const&) { },
        [] (typecheck::expr_t::init_list_t const&) { },
        [] (typecheck::expr_t::subscript_t const&) { },
        [] (typecheck::expr_t::because_t const& x)
        {
            // TODO needs a test: gen_destructor(global, local, builder, value, x.value.get());
        });
}

/**
 * @brief Generate all destructor calls associated to the given local context.
 *
 * Destructors are generated in reverse order and recursively inside-out.
 *
 * @remarks When this function returns the list of destructors of the local context will be cleared.
 */
static void gen_destructors(global_ctx_t const& global, local_ctx_t& local, llvm::IRBuilder<>& builder)
{
    for (auto const& [value, type]: std::views::reverse(local.destructors))
        gen_destructor(global, local, builder, value, type);
    local.destructors.clear();
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
    local_ctx_t const& body_local,
    snippet_t& snippet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& stmt,
    llvm::Function* const llvm_f,
    llvm::Value* const inlined_result)
{
    auto local = body_local.extend();
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t const& x)
        {
            gen_func_call(global, local, builder, x, value_category_t::temporary, nullptr);
            // TODO gen_destructors(global, local, builder); needs a test
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
                {
                    gen_temporary_val(global, local, builder, x.cond);
                    // TODO gen_destructors(global, local, builder); needs a test
                }
                if (x.false_branch)
                    gen_stmts(global, local, snippet, builder, *x.false_branch, llvm_f, inlined_result);
                return;
            }
            if (x.false_branch and is_impossible(*x.false_branch))
            {
                if (is_mutable(x.cond))
                {
                    gen_temporary_val(global, local, builder, x.cond);
                    // TODO gen_destructors(global, local, builder); needs a test
                }
                gen_stmts(global, local, snippet, builder, x.true_branch, llvm_f, inlined_result);
                return;
            }
            // Otherwise both branches are possible and we need to emit each one in their own basic block.
            // Also, before entering either branch, we need to make sure we invoke all destructors registered
            // whilst generating the branch condition.
            auto const cond = gen_temporary_val(global, local, builder, x.cond);
            gen_destructors(global, local, builder);
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
                // Only unit_t can be missing a return expr.
                // Also note that no destructor has been registered from the beginning of this function.
                builder.CreateRet(gen_val_unit(global));
                return;
            }
            if (inlined_result)
            {
                // the enclosing function is being inlined, so the returned value is a temporary in the outer function
                // TODO this might be wrong, think `return [] { return {3, {1,2,3}} }();` needs a test
                gen_val(global, local, builder, *x.expr, value_category_t::temporary, inlined_result);
                // TODO when and how do we generate destructors of inlined lambdas? currently we would be leaking
                snippet.open_blocks.push_back(builder.GetInsertBlock());
                return;
            }
            auto const arg0 = llvm_f->arg_empty() ? nullptr : llvm_f->getArg(0ul);
            auto const dest = arg0 and arg0->hasAttribute(llvm::Attribute::StructRet) ? arg0 : nullptr;
            // always generate a value, even for expressions of type unit_t,
            // because it might be a function call with side effects;
            // and, if it is, just return the unit value, without complicating the CFG
            auto const ret_val = gen_val(global, local, builder, *x.expr, value_category_t::result, dest);
            gen_destructors(global, local, builder);
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
