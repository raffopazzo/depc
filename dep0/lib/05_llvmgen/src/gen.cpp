#include "dep0/llvmgen/gen.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

#include <algorithm>

namespace dep0::llvmgen {

struct context_t
{
    std::map<source_text, llvm::Value*> values; // including function objects
    std::map<source_text, llvm::FunctionType*> fun_types;
    std::map<source_text, llvm::Type*> types;
    // TODO what about extending a context?
};

// Snippet is a model of a block of IR code that is being built,
// could be for example from the body of a function definition or the body of an if-else branch.
// It contains an entry point and possibly many open blocks, i.e. blocks which currently do not have
// a terminator. All open blocks are closed by jumping to the next statement in the parent scope.
// If required could also add last insert point in case we want to append to where we left off,
// which may or may not be the end iterator of the last open block.
struct snippet_t
{
    llvm::BasicBlock* entry_block;
    std::vector<llvm::BasicBlock*> open_blocks;
};

// all gen functions must be forward-declared here

static llvm::Type* gen_type(context_t const&, llvm::LLVMContext&, typecheck::type_t const&);
static llvm::Value* gen_fun(context_t&, llvm::Module& , typecheck::func_def_t const&);
static snippet_t gen_body(
    context_t&,
    llvm::LLVMContext&,
    typecheck::body_t const&,
    std::string_view,
    llvm::Function*);
static void gen_stmt(context_t&, snippet_t&, llvm::IRBuilder<>&, typecheck::stmt_t const&, llvm::Function*);
static llvm::Value* gen_val(context_t&, llvm::IRBuilder<>& , typecheck::expr_t const&);

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m)
{
    context_t ctx;
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    // TODO: allow declarations in any order with 2 pass
    for (auto const& f: m.func_defs)
        gen_fun(ctx, llvm_module.get(), f);

    std::string err;
    llvm::raw_string_ostream ostream(err);
    return llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

// all gen functions must be implemented here
llvm::Type* gen_type(context_t const&, llvm::LLVMContext& llvm_ctx, typecheck::type_t const& x)
{
    struct visitor
    {
        llvm::LLVMContext& llvm_ctx;
        llvm::Type* operator()(typecheck::type_t::bool_t const&) { return llvm::Type::getInt1Ty(llvm_ctx); }
        // this may or may not be fine depending on how LLVM defines `void` (i.e. what properties it has)
        // for instance, what does it mean to have an array of `void` types in LLVM?
        llvm::Type* operator()(typecheck::type_t::unit_t const&) { return llvm::Type::getVoidTy(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::i8_t const&) { return llvm::Type::getInt8Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::i16_t const&) { return llvm::Type::getInt16Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::i32_t const&) { return llvm::Type::getInt32Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::i64_t const&) { return llvm::Type::getInt64Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::u8_t const&) { return llvm::Type::getInt8Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::u16_t const&) { return llvm::Type::getInt16Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::u32_t const&) { return llvm::Type::getInt32Ty(llvm_ctx); }
        llvm::Type* operator()(typecheck::type_t::u64_t const&) { return llvm::Type::getInt64Ty(llvm_ctx); }
    };
    return std::visit(visitor{llvm_ctx}, x.value);
}

static void gen_fun_attributes(llvm::Function* const func, typecheck::func_def_t const& x)
{
    struct return_attr_visitor
    {
        llvm::Attribute::AttrKind operator()(typecheck::type_t::bool_t const&) const { return llvm::Attribute::None; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::unit_t const&) const { return llvm::Attribute::None;; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::i8_t const&) const { return llvm::Attribute::SExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::i16_t const&) const { return llvm::Attribute::SExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::i32_t const&) const { return llvm::Attribute::SExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::i64_t const&) const { return llvm::Attribute::SExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::u8_t const&) const { return llvm::Attribute::ZExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::u16_t const&) const { return llvm::Attribute::ZExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::u32_t const&) const { return llvm::Attribute::ZExt; }
        llvm::Attribute::AttrKind operator()(typecheck::type_t::u64_t const&) const { return llvm::Attribute::ZExt; }
    };
    if (auto const attr = std::visit(return_attr_visitor{}, x.type.value); attr != llvm::Attribute::None)
        func->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

llvm::Value* gen_fun(context_t& ctx, llvm::Module& llvm_module, typecheck::func_def_t const& x)
{
    auto const funtype = llvm::FunctionType::get(gen_type(ctx, llvm_module.getContext(), x.type), {}, false);
    auto const func = llvm::Function::Create(funtype, llvm::Function::ExternalLinkage, x.name.view(), llvm_module);
    gen_fun_attributes(func, x);
    ctx.fun_types[x.name] = funtype;
    ctx.values[x.name] = func;
    auto snippet = gen_body(ctx, llvm_module.getContext(), x.body, "entry", func);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::type_t::unit_t>(x.type.value))
    {
        auto builder = llvm::IRBuilder<>(llvm_module.getContext());
        for (auto* const bb: snippet.open_blocks)
        {
            builder.SetInsertPoint(bb);
            builder.CreateRetVoid();
        }
    }
    return func;
}

snippet_t gen_body(
    context_t& ctx,
    llvm::LLVMContext& llvm_ctx,
    typecheck::body_t const& x,
    std::string_view const entry_block_name,
    llvm::Function* const func)
{
    snippet_t snippet;
    auto builder = llvm::IRBuilder<>(llvm_ctx);
    builder.SetInsertPoint(snippet.entry_block = llvm::BasicBlock::Create(llvm_ctx, entry_block_name, func));
    auto it = x.stmts.begin();
    if (it == x.stmts.end())
        snippet.open_blocks.push_back(snippet.entry_block);
    else
    {
        gen_stmt(ctx, snippet, builder, *it, func);
        while (++it != x.stmts.end())
        {
            if (snippet.open_blocks.size())
            {
                auto next = llvm::BasicBlock::Create(llvm_ctx, "next", func);
                for (auto& bb: snippet.open_blocks)
                {
                    builder.SetInsertPoint(bb);
                    builder.CreateBr(next);
                }
                snippet.open_blocks.clear();
                builder.SetInsertPoint(next);
            }
            gen_stmt(ctx, snippet, builder, *it, func);
        }
    }
    return snippet;
}

void gen_stmt(
    context_t& ctx,
    snippet_t& snipppet,
    llvm::IRBuilder<>& builder,
    typecheck::stmt_t const& x,
    llvm::Function* const func)
{
    struct visitor
    {
        context_t& ctx;
        snippet_t& snipppet;
        llvm::IRBuilder<>& builder;
        llvm::Function* const func;
        void operator()(typecheck::stmt_t::fun_call_t const& x)
        {
            builder.CreateCall(ctx.fun_types[x.name], ctx.values[x.name]);
        }
        void operator()(typecheck::stmt_t::if_else_t const& x)
        {
            auto* cond = gen_val(ctx, builder, x.cond);
            auto then = gen_body(ctx, builder.getContext(), x.true_branch, "then", func);
            std::ranges::copy(then.open_blocks, std::back_inserter(snipppet.open_blocks));
            if (x.false_branch)
            {
                auto else_ = gen_body(ctx, builder.getContext(), *x.false_branch, "else", func);
                std::ranges::copy(else_.open_blocks, std::back_inserter(snipppet.open_blocks));
                builder.CreateCondBr(cond, then.entry_block, else_.entry_block);
            }
            else
            {
                auto else_ = llvm::BasicBlock::Create(builder.getContext(), "else", func);
                builder.CreateCondBr(cond, then.entry_block, else_);
                snipppet.open_blocks.push_back(else_);
            }
        }
        void operator()(typecheck::stmt_t::return_t const& x)
        {
            if (x.expr)
                builder.CreateRet(gen_val(ctx, builder, *x.expr));
            else
                builder.CreateRetVoid();
        }
    };
    std::visit(visitor{ctx, snipppet, builder, func}, x.value);
}

llvm::Value* gen_val(context_t& ctx, llvm::IRBuilder<>& builder, typecheck::expr_t const& expr)
{
    struct visitor
    {
        context_t& ctx;
        llvm::IRBuilder<>& builder;
        typecheck::expr_t const& expr;

        llvm::Value* operator()(typecheck::expr_t::boolean_constant_t const& x)
        {
            return llvm::ConstantInt::getBool(builder.getContext(), x.value == "true");
        }
        llvm::Value* operator()(typecheck::expr_t::numeric_constant_t const& x)
        {
            auto const type = cast<llvm::IntegerType>(gen_type(ctx, builder.getContext(), expr.properties.type));
            return llvm::ConstantInt::get(type, x.number.view(), 10);
        }
        llvm::Value* operator()(typecheck::expr_t::fun_call_t const& x)
        {
            return builder.CreateCall(ctx.fun_types[x.name], ctx.values[x.name]);
        }
    };
    return std::visit(visitor{ctx, builder, expr}, expr.value);
}

}
