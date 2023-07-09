#include "dep0/llvmgen/gen.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace dep0::llvmgen {

// all gen functions must be forward-declared here

static llvm::Type* gen(llvm::Module&, typecheck::type_t const&);
static llvm::Value* gen(llvm::Module& , typecheck::func_def_t const&);
static llvm::BasicBlock* gen(llvm::Module& , typecheck::body_t const&);
static llvm::Instruction* gen(llvm::IRBuilder<>& , typecheck::stmt_t const&);
static llvm::Value* gen(llvm::IRBuilder<>& , typecheck::expr_t const&);

expected<unique_ref<llvm::Module>> gen(
    llvm::LLVMContext& ctx,
    std::string_view const name,
    typecheck::module_t const& m)
{
    auto llvm_module = make_ref<llvm::Module>(name, ctx);
    for (auto const& f: m.func_defs)
        gen(llvm_module.get(), f);
    return llvm_module;
}

// all gen functions must be implemented here

llvm::Type* gen(llvm::Module& llvm_module, typecheck::type_t const& x)
{
    struct visitor
    {
        llvm::Module& llvm_module;
        llvm::Type* operator()(typecheck::type_t::int_t const&)
        {
            return llvm::Type::getInt32Ty(llvm_module.getContext());
        }
    };
    return std::visit(visitor{llvm_module}, x.value);
}

llvm::Value* gen(llvm::Module& llvm_module, typecheck::func_def_t const& x)
{
    auto const funtype = llvm::FunctionType::get(gen(llvm_module, x.type), {}, false); 
    auto const func = llvm::Function::Create(funtype, llvm::Function::ExternalLinkage, x.name.txt, llvm_module);
    gen(llvm_module, x.body)->insertInto(func);
    return func;
}

llvm::BasicBlock* gen(llvm::Module& llvm_module, typecheck::body_t const& x)
{
    auto builder = llvm::IRBuilder<>(llvm_module.getContext());
    auto const entry = llvm::BasicBlock::Create(llvm_module.getContext(), "entry");
    builder.SetInsertPoint(entry);
    for (auto const& s: x.stmts)
        gen(builder, s);
    return entry;
}

llvm::Instruction* gen(llvm::IRBuilder<>& builder, typecheck::stmt_t const& x)
{
    struct visitor
    {
        llvm::IRBuilder<>& builder;
        llvm::Instruction* operator()(typecheck::stmt_t::return_t const& x)
        {
            if (x.expr)
                return builder.CreateRet(gen(builder, *x.expr));
            else
                return builder.CreateRetVoid();
        }
    };
    return std::visit(visitor{builder}, x.value);
}

llvm::Value* gen(llvm::IRBuilder<>& builder, typecheck::expr_t const& x)
{
    struct visitor
    {
        llvm::IRBuilder<>& builder;
        llvm::Value* operator()(typecheck::expr_t::numeric_constant_t const& x)
        {
            // currently only `int` is derivable, so `typecheck::expr_t` is proof that this is a valid integer number
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), x.number.txt, 10);
        }
    };
    return std::visit(visitor{builder}, x.value);
}

}
