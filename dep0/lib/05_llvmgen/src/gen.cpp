#include "dep0/llvmgen/gen.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

namespace dep0::llvmgen {

struct context_t
{
    std::map<source_text, llvm::Value*> values; // including function objects
    std::map<source_text, llvm::FunctionType*> fun_types;
    // TODO what about extending a context?
};

// all gen functions must be forward-declared here

static llvm::Type* gen(context_t&, llvm::Module&, typecheck::type_t const&);
static llvm::Value* gen(context_t&, llvm::Module& , typecheck::func_def_t const&);
static llvm::BasicBlock* gen(context_t&, llvm::Module& , typecheck::body_t const&);
static llvm::Instruction* gen(context_t&, llvm::IRBuilder<>& , typecheck::stmt_t const&);
static llvm::Value* gen(context_t&, llvm::IRBuilder<>& , typecheck::expr_t const&);

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m)
{
    context_t ctx;
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    // TODO: allow declarations in any order with 2 pass
    for (auto const& f: m.func_defs)
        gen(ctx, llvm_module.get(), f);

    std::string err;
    llvm::raw_string_ostream ostream(err);
    return llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

// all gen functions must be implemented here

llvm::Type* gen(context_t&, llvm::Module& llvm_module, typecheck::type_t const& x)
{
    struct visitor
    {
        llvm::Module& llvm_module;
        llvm::Type* operator()(typecheck::type_t::unit_t const&)
        {
            // this may or may not be fine depending on how LLVM defines `void` (i.e. what properties it has)
            // for instance, what does it men to have an array of `void` types in LLVM?
            return llvm::Type::getVoidTy(llvm_module.getContext());
        }
        llvm::Type* operator()(typecheck::type_t::int_t const&)
        {
            return llvm::Type::getInt32Ty(llvm_module.getContext());
        }
    };
    return std::visit(visitor{llvm_module}, x.value);
}

llvm::Value* gen(context_t& ctx, llvm::Module& llvm_module, typecheck::func_def_t const& x)
{
    auto const funtype = llvm::FunctionType::get(gen(ctx, llvm_module, x.type), {}, false);
    auto const func = llvm::Function::Create(funtype, llvm::Function::ExternalLinkage, x.name.view(), llvm_module);
    ctx.fun_types[x.name] = funtype;
    ctx.values[x.name] = func;
    gen(ctx, llvm_module, x.body)->insertInto(func);
    return func;
}

llvm::BasicBlock* gen(context_t& ctx, llvm::Module& llvm_module, typecheck::body_t const& x)
{
    auto builder = llvm::IRBuilder<>(llvm_module.getContext());
    auto const entry = llvm::BasicBlock::Create(llvm_module.getContext(), "entry");
    builder.SetInsertPoint(entry);
    for (auto const& s: x.stmts)
        gen(ctx, builder, s);
    return entry;
}

llvm::Instruction* gen(context_t& ctx, llvm::IRBuilder<>& builder, typecheck::stmt_t const& x)
{
    struct visitor
    {
        context_t& ctx;
        llvm::IRBuilder<>& builder;
        llvm::Instruction* operator()(typecheck::stmt_t::return_t const& x)
        {
            if (x.expr)
                return builder.CreateRet(gen(ctx, builder, *x.expr));
            else
                return builder.CreateRetVoid();
        }
    };
    return std::visit(visitor{ctx, builder}, x.value);
}

llvm::Value* gen(context_t& ctx, llvm::IRBuilder<>& builder, typecheck::expr_t const& expr)
{
    struct visitor
    {
        context_t& ctx;
        llvm::IRBuilder<>& builder;
        typecheck::expr_t const& expr;

        llvm::Value* operator()(typecheck::expr_t::numeric_constant_t const& x)
        {
            // currently only `int` is derivable, so `typecheck::expr_t` is proof that this is a valid integer number
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), x.number.view(), 10);
        }
        llvm::Value* operator()(typecheck::expr_t::fun_call_t const& x)
        {
            return builder.CreateCall(ctx.fun_types[x.name], ctx.values[x.name]);
        }
    };
    return std::visit(visitor{ctx, builder, expr}, expr.value);
}

}
