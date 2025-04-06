/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_body.hpp"

#include "private/gen_array.hpp"
#include "private/gen_loop.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"
#include "private/llvm_func.hpp"

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
 * @brief Generate the destructor for the given type.
 *
 * Destructors are generated in reverse order and recursively inside-out.
 *
 * @remarks Normally a destructor only takes 1 argument, but for an array it also takes its length.
 */
static llvm_func_t gen_destructor(global_ctx_t&, typecheck::expr_t const&);

/**
 * @brief Invoke the destructor on a value of the given type.
 *
 * @remarks If this is the first time this function is called for the given type,
 * a new destructor function will be emitted and registered in the global context.
 */
static void gen_destructor_call(global_ctx_t&, local_ctx_t&, llvm::IRBuilder<>&, llvm::Value*, typecheck::expr_t const&);

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

static llvm_func_t gen_destructor(global_ctx_t& global, typecheck::expr_t const& type)
{
    assert(not is_trivially_destructible(global, type) and "trivially destructible types do not need a destructor");
    if (auto const because = std::get_if<typecheck::expr_t::because_t>(&type.value))
        return gen_destructor(global, because->value.get());
    local_ctx_t local;
    auto const name = std::string{".dtor."} + std::to_string(global.get_next_id());
    auto const arg_type = is_boxed(type) ? gen_type(global, type) : gen_type(global, type)->getPointerTo();
    auto const void_type = llvm::Type::getVoidTy(global.llvm_ctx);
    auto const f_ty =
        is_array(type)
        ? llvm::FunctionType::get(void_type, {arg_type, llvm::Type::getInt64Ty(global.llvm_ctx)}, false)
        : llvm::FunctionType::get(void_type, {arg_type}, false);
    auto const llvm_f = llvm::Function::Create(f_ty, llvm::Function::PrivateLinkage, name, global.llvm_module);
    // currently the first argument is always a non-null pointer,
    // and only arrays take a second arugment which can be marked zext
    llvm_f->getArg(0)->addAttr(llvm::Attribute::NonNull);
    if (llvm_f->arg_size() > 1)
        llvm_f->getArg(1)->addAttr(llvm::Attribute::ZExt);
    auto builder = llvm::IRBuilder<>(global.llvm_ctx);
    builder.SetInsertPoint(llvm::BasicBlock::Create(global.llvm_ctx, "entry", llvm_f));
    auto const value = llvm_f->getArg(0);
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
            auto const properties = get_properties_if_array(type);
            assert(properties and "non-trivially-destructible type is not an array");
            gen_for_loop_downward(
                global, local, builder,
                ast::sign_t::unsigned_v,
                llvm_f->getArg(1),
                builder.getInt64(0),
                [
                    &element_type=properties->element_type,
                    llvm_type=gen_type(global, properties->element_type),
                    array=value
                ]
                (global_ctx_t& global, local_ctx_t& local, llvm::IRBuilder<>& builder, llvm::Value* const index)
                {
                    auto const element_ptr = builder.CreateGEP(llvm_type, array, index);
                    gen_destructor_call(global, local, builder, element_ptr, element_type);
                });
        },
        [] (typecheck::expr_t::abs_t const&) { },
        [] (typecheck::expr_t::pi_t const& x) { /* TODO this will change once we introduce closures */ },
        [&] (typecheck::expr_t::sigma_t const& x)
        {
            auto const gep = [&, llvm_type = gen_type(global, type), zero = builder.getInt32(0)] (std::size_t const i)
            {
                return builder.CreateGEP(llvm_type, value, {zero, builder.getInt32(i)});
            };
            auto sigma_ctx = local.extend();
            for (auto const i: std::views::iota(0ul, x.args.size()))
                if (x.args[i].var)
                    sigma_ctx.try_emplace(
                        *x.args[i].var,
                        is_boxed(x.args[i].type) or is_pass_by_val(global, x.args[i].type)
                        ? builder.CreateLoad(gen_type(global, x.args[i].type), gep(i))
                        : gep(i));
            for (auto const i: std::views::reverse(std::views::iota(0ul, x.args.size())))
            {
                if (is_boxed(x.args[i].type))
                {
                    auto const element_ptr = builder.CreateLoad(gen_type(global, x.args[i].type), gep(i));
                    if (not is_trivially_destructible(global, x.args[i].type))
                        gen_destructor_call(global, sigma_ctx, builder, element_ptr, x.args[i].type);
                    auto const free = llvm::CallInst::CreateFree(element_ptr, builder.GetInsertBlock());
                    builder.GetInsertBlock()->getInstList().push_back(free);
                }
                else if (not is_trivially_destructible(global, x.args[i].type))
                    gen_destructor_call(global, sigma_ctx, builder, gep(i), x.args[i].type);
            }
            // TODO what should we do here? maybe just append to parent destructors? explain or add a test
            assert(sigma_ctx.destructors.empty() and "TODO invoke destructors");
        },
        [] (typecheck::expr_t::array_t const&) { },
        [] (typecheck::expr_t::init_list_t const&) { },
        [] (typecheck::expr_t::subscript_t const&) { },
        [] (typecheck::expr_t::because_t const&)
        {
            assert(false and "destructor of because-type should delegate to destructor of underlying value type");
        });
    builder.CreateRetVoid();
    // TODO should call destructors here; needs a test
    assert(local.destructors.empty() and "destructor will allocate more memory");
    finalize_llvm_func(llvm_f);
    return llvm_func_t(f_ty, llvm_f);
}

static void gen_destructor_call(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    llvm::Value* const value,
    typecheck::expr_t const& type)
{
    auto destructor = global.get_destructor(type);
    if (not destructor)
    {
        destructor = gen_destructor(global, type);
        global.store_destructor(type, *destructor);
    }
    if (auto const properties = get_properties_if_array(type))
    {
        auto const size = gen_array_total_size(global, local, builder, *properties);
        auto const call = builder.CreateCall(destructor->type, destructor->func, {value, size});
        call->addParamAttr(1, llvm::Attribute::ZExt);
    }
    else
        builder.CreateCall(destructor->type, destructor->func, {value});
}

/**
 * @brief Generate all destructor calls associated to the given local context.
 *
 * Destructors are generated in reverse order and recursively inside-out.
 *
 * @remarks When this function returns the list of destructors of the local context will be cleared.
 */
static void gen_destructors(global_ctx_t& global, local_ctx_t& local, llvm::IRBuilder<>& builder)
{
    while (not local.destructors.empty())
    {
        auto destructors = std::move(local.destructors);
        for (auto const& [value, type]: std::views::reverse(destructors))
            gen_destructor_call(global, local, builder, value, type);
        // if calling destructors leads to needing more destructor calls,
        // we can just loop around until we drain the list,
        // but we really don't expect this to happen
        assert(local.destructors.empty() and "gen_destructor_call() created more need for destructors");
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
