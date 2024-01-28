#include "private/gen_func.hpp"

#include "private/gen_alloca.hpp"
#include "private/gen_attrs.hpp"
#include "private/gen_body.hpp"
#include "private/gen_type.hpp"

#include <llvm/IR/Attributes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>

#include <string>

namespace dep0::llvmgen {

static void gen_func_args(local_context_t&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_attributes(local_context_t const&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_body(
    global_context_t&,
    local_context_t const&,
    llvm_func_proto_t const&,
    typecheck::body_t const&,
    llvm::Function*);

void gen_func_args(local_context_t& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    auto llvm_arg_it = llvm_f->arg_begin();
    if (is_alloca_needed(proto.ret_type()))
    {
        assert(llvm_f->arg_size() == proto.args().size() + 1ul and "function with sret must have 1 more argument");
        llvm_arg_it->addAttr(llvm::Attribute::StructRet);
        llvm_arg_it->addAttr(llvm::Attribute::NonNull);
        ++llvm_arg_it;
    }
    else
        assert(llvm_f->arg_size() == proto.args().size() and "function and prototype must have same arguments");
    for (auto const& arg: proto.args())
    {
        auto& llvm_arg = *llvm_arg_it++;
        if (auto const attr = get_sign_ext_attribute(local, arg.type); attr != llvm::Attribute::None)
            llvm_arg.addAttr(attr);
        if (is_alloca_needed(arg.type))
            llvm_arg.addAttr(llvm::Attribute::NonNull);
        if (arg.var)
        {
            if (arg.var->idx == 0ul)
                llvm_arg.setName(arg.var->name.view());
            bool inserted = false;
            if (std::holds_alternative<typecheck::expr_t::pi_t>(arg.type.value))
            {
                assert(llvm_arg.getType()->isPointerTy());
                auto const function_type = cast<llvm::FunctionType>(llvm_arg.getType()->getPointerElementType());
                assert(function_type);
                inserted = local.try_emplace(*arg.var, llvm_func_t(function_type, &llvm_arg)).second;
            }
            else
                inserted = local.try_emplace(*arg.var, &llvm_arg).second;
            assert(inserted);
        }
    }
}

void gen_func_attributes(local_context_t const& local, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    if (auto const attr = get_sign_ext_attribute(local, proto.ret_type()); attr != llvm::Attribute::None)
        llvm_f->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

void gen_func_body(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto,
    typecheck::body_t const& body,
    llvm::Function* const llvm_f)
{
    auto snippet = gen_body(global, local, body, "entry", llvm_f, nullptr);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::expr_t::unit_t>(proto.ret_type().value))
    {
        auto builder = llvm::IRBuilder<>(global.llvm_ctx);
        // Having open blocks means that the function has no return statement,
        // this implies its return type is `unit_t`, so just return `i8 0`.
        snippet.seal_open_blocks(builder, [zero=builder.getInt8(0)] (auto& builder) { builder.CreateRet(zero); });
    }
}

llvm::Value* gen_func(
    global_context_t& global,
    local_context_t const& local,
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
    auto const name = std::string{"$_func_"} + std::to_string(global.get_next_id());
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::PrivateLinkage,
            name,
            global.llvm_module);
    auto f_ctx = local.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
    return llvm_f;
}

void gen_func(
    global_context_t& global,
    local_context_t& local,
    source_text const& name,
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, local, proto),
            llvm::Function::ExternalLinkage,
            name.view(),
            global.llvm_module);
    // add function to the parent context; but generation must happen in the function context
    bool const inserted = local.try_emplace(typecheck::expr_t::var_t{name}, llvm_func_t(llvm_f)).second;
    assert(inserted);
    auto f_ctx = local.extend();
    gen_func_args(f_ctx, proto, llvm_f);
    gen_func_attributes(f_ctx, proto, llvm_f);
    gen_func_body(global, f_ctx, proto, f.body, llvm_f);
}

} // namespace dep0::llvmgen
