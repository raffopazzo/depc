/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/llvmgen/gen.hpp"

#include "private/context.hpp"
#include "private/gen_func.hpp"
#include "private/proto.hpp"

#include "dep0/match.hpp"

#include <llvm/IR/Verifier.h>

#include <string>

namespace dep0::llvmgen {

static unique_ref<llvm::Module>
build_empty_module(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    llvm::TargetMachine& machine)
{
    auto m = make_ref<llvm::Module>(name, llvm_ctx);
    m->setDataLayout(machine.createDataLayout());
    m->setTargetTriple(machine.getTargetTriple().str());
    return m;
}

static void gen_impl(llvm::Module& llvm_module, typecheck::module_t const& m) noexcept
{
    global_ctx_t global(llvm_module);
    for (auto const& x: m.entries)
        match(
            x,
            [&] (typecheck::type_def_t const& def)
            {
                auto const& name = match(def.value, [] (auto const& x) -> auto const& { return x.name; });
                bool const inserted = global.try_emplace(typecheck::expr_t::global_t{std::nullopt, name}, def).second;
                assert(inserted);
            },
            [] (typecheck::axiom_t const&)
            {
                // axioms cannot be invoked at run-time, so we don't need to do anything here
            },
            [&] (typecheck::extern_decl_t const& decl)
            {
                if (auto proto = llvm_func_proto_t::from_pi(decl.signature))
                    gen_extern_decl(global, typecheck::expr_t::global_t{std::nullopt, decl.name}, *proto);
            },
            // LLVM can only generate functions for 1st order abstractions;
            // for 2nd order abstractions we rely on beta-delta normalization to produce
            // a value or a 1st order application;
            [&] (typecheck::func_decl_t const& decl)
            {
                if (auto proto = llvm_func_proto_t::from_pi(decl.signature))
                    gen_func_decl(global, typecheck::expr_t::global_t{std::nullopt, decl.name}, *proto);
            },
            [&] (typecheck::func_def_t const& def)
            {
                if (auto proto = llvm_func_proto_t::from_abs(def.value))
                    gen_func(global, typecheck::expr_t::global_t{std::nullopt, def.name}, *proto, def.value);
            });
}

expected<unique_ref<llvm::Module>>
gen(llvm::LLVMContext& llvm_ctx, std::string_view const name, typecheck::module_t const& m,
    llvm::TargetMachine& machine) noexcept
{
    auto result = build_empty_module(llvm_ctx, name, machine);
    gen_impl(*result, m);
    std::string err;
    llvm::raw_string_ostream ostream(err);
    if (llvm::verifyModule(*result, &ostream)) // yes true means false...
        return error_t{err};
    else
        return std::move(result);
}

expected<unique_ref<llvm::Module>>
gen_unverified(
    llvm::LLVMContext& llvm_ctx,
    std::string_view const name,
    typecheck::module_t const& m,
    llvm::TargetMachine& machine) noexcept
{
    auto result = build_empty_module(llvm_ctx, name, machine);
    gen_impl(*result, m);
    return std::move(result);
}

} // namespace dep0::llvmgen
