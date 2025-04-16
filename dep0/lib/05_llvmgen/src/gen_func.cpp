/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_func.hpp"

#include "private/gen_attrs.hpp"
#include "private/gen_body.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"
#include "private/llvm_func.hpp"

#include <llvm/IR/Attributes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>

#include <string>

namespace dep0::llvmgen {

static void gen_func_args(global_ctx_t&, local_ctx_t&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_attributes(global_ctx_t const&, llvm_func_proto_t const&, llvm::Function*);
static void gen_func_body(
    global_ctx_t&,
    local_ctx_t const&,
    llvm_func_proto_t const&,
    typecheck::body_t const&,
    llvm::Function*);

void gen_func_args(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm_func_proto_t const& proto,
    llvm::Function* const llvm_f)
{
    auto llvm_arg_it = llvm_f->arg_begin();
    if (is_pass_by_ptr(global, proto.ret_type()))
    {
        assert(llvm_f->arg_size() == proto.runtime_args().size() + 1ul and "function with sret must have 1 more argument");
        auto const maybe_array = get_properties_if_array(proto.ret_type());
        auto const return_value_type = gen_type(global, maybe_array ? maybe_array->element_type : proto.ret_type());
        llvm_arg_it->addAttr(llvm::Attribute::getWithStructRetType(global.llvm_ctx, return_value_type));
        llvm_arg_it->addAttr(llvm::Attribute::NonNull);
        ++llvm_arg_it;
    }
    else
        assert(llvm_f->arg_size() == proto.runtime_args().size() and "function and prototype must have same arguments");
    for (auto const& arg: proto.runtime_args())
    {
        auto& llvm_arg = *llvm_arg_it++;
        if (auto const attr = get_sign_ext_attribute(global, arg.type); attr != llvm::Attribute::None)
            llvm_arg.addAttr(attr);
        if (llvm_arg.getType()->isPointerTy()) // for pointer types, currently we never emit null pointer values
            // TODO should we also set noalias, byval, etc? maybe for the return argument too?
            llvm_arg.addAttr(llvm::Attribute::NonNull);
        if (arg.var)
        {
            if (arg.var->idx == 0ul)
                llvm_arg.setName(arg.var->name.view());
            bool inserted = false;
            if (auto const pi = std::get_if<typecheck::expr_t::pi_t>(&arg.type.value))
            {
                assert(llvm_arg.getType()->isPointerTy());
                auto const proto = llvm_func_proto_t::from_pi(*pi);
                assert(proto.has_value() and "function arguments must be of the 1st order");
                auto const function_type = gen_func_type(global, *proto);
                inserted = local.try_emplace(*arg.var, llvm_func_t(function_type, &llvm_arg)).second;
            }
            else
                inserted = local.try_emplace(*arg.var, &llvm_arg).second;
            assert(inserted);
        }
    }
}

void gen_func_attributes(global_ctx_t const& global, llvm_func_proto_t const& proto, llvm::Function* const llvm_f)
{
    if (auto const attr = get_sign_ext_attribute(global, proto.ret_type()); attr != llvm::Attribute::None)
        llvm_f->addAttribute(llvm::AttributeList::ReturnIndex, attr);
}

void gen_func_body(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm_func_proto_t const& proto,
    typecheck::body_t const& body,
    llvm::Function* const llvm_f)
{
    auto snippet = gen_body(global, local, body, "entry", llvm_f, std::nullopt);
    if (snippet.open_blocks.size() and std::holds_alternative<typecheck::expr_t::unit_t>(proto.ret_type().value))
    {
        auto builder = llvm::IRBuilder<>(global.llvm_ctx);
        // Having open blocks means that the function has no return statement,
        // this implies its return type is `unit_t`, so just return `i8 0`.
        snippet.seal_open_blocks(builder, [unit=gen_val_unit(global)] (auto& builder) { builder.CreateRet(unit); });
    }
    finalize_llvm_func(llvm_f);
}

void gen_extern_decl(
    global_ctx_t& global,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const& proto)
{
    gen_func_decl(global, name, proto);
}

void gen_func_decl(
    global_ctx_t& global,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const& proto)
{
    if (not global[name])
    {
        auto const llvm_f =
            llvm::Function::Create(
                gen_func_type(global, proto),
                llvm::Function::ExternalLinkage,
                name.name.view(),
                global.llvm_module);
        bool const inserted = global.try_emplace(name, llvm_func_t(llvm_f)).second;
        assert(inserted);
        local_ctx_t local;
        gen_func_args(global, local, proto, llvm_f);
        gen_func_attributes(global, proto, llvm_f);
    }
}

llvm::Value* gen_func(
    global_ctx_t& global,
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
    auto const name = std::string{"$_func_"} + std::to_string(global.get_next_id());
    auto const llvm_f =
        llvm::Function::Create(
            gen_func_type(global, proto),
            llvm::Function::PrivateLinkage,
            name,
            global.llvm_module);
    local_ctx_t local;
    gen_func_args(global, local, proto, llvm_f);
    gen_func_attributes(global, proto, llvm_f);
    gen_func_body(global, local, proto, f.body, llvm_f);
    return llvm_f;
}

void gen_func(
    global_ctx_t& global,
    typecheck::expr_t::global_t const& name,
    llvm_func_proto_t const& proto,
    typecheck::expr_t::abs_t const& f)
{
    // we may have already constructed a function object from `gen_func_decl`;
    // if that is the case, all we have to do now is generate the body
    llvm::Function* llvm_f;
    local_ctx_t local;
    if (auto* const p = global[name])
    {
        llvm_f = llvm::dyn_cast<llvm::Function>(std::get<llvm_func_t>(*p).func);
        assert(llvm_f);
    }
    else
    {
        llvm_f =
            llvm::Function::Create(
                gen_func_type(global, proto),
                llvm::Function::ExternalLinkage,
                name.name.view(),
                global.llvm_module);
        bool const inserted = global.try_emplace(name, llvm_func_t(llvm_f)).second;
        assert(inserted);
        gen_func_args(global, local, proto, llvm_f);
        gen_func_attributes(global, proto, llvm_f);
    }
    gen_func_body(global, local, proto, f.body, llvm_f);
}

} // namespace dep0::llvmgen
