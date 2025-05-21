/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/llvmgen/gen.hpp"

#include "private/context.hpp"
#include "private/first_order_types.hpp"
#include "private/gen_func.hpp"
#include "private/gen_type.hpp"
#include "private/proto.hpp"

#include "dep0/fmap.hpp"
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

static expected<std::true_type> gen_impl(llvm::Module& llvm_module, typecheck::module_t const& m) noexcept
{
    global_ctx_t global(llvm_module);
    for (auto const& x: m.entries)
    {
        if (auto const def = std::get_if<typecheck::type_def_t>(&x))
            if (auto const s = std::get_if<typecheck::type_def_t::struct_t>(&def->value))
                if (not std::ranges::all_of(s->fields, [] (auto const& f) { return is_first_order_type(f.type); }))
                    return dep0::error_t(
                        "cannot emit program with higher order struct types",
                        def->properties.origin);
        match(
            x,
            [&] (typecheck::type_def_t const& def)
            {
                auto const& name = match(def.value, [] (auto const& x) -> auto const& { return x.name; });
                auto g = typecheck::expr_t::global_t{std::nullopt, name};
                auto const ty =
                    match(
                        def.value,
                        [&] (typecheck::type_def_t::integer_t const& x) -> llvm::Type*
                        {
                            return gen_type(global, x.width);
                        },
                        [&] (typecheck::type_def_t::struct_t const& s) -> llvm::Type*
                        {
                            auto const& f = fmap(s.fields, [&] (auto const& x) { return gen_type(global, x.type); });
                            return llvm::StructType::create(global.llvm_ctx, f, s.name.view());
                        });
                bool const inserted = global.try_emplace(g, global_ctx_t::type_def_t{def, ty}).second;
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
    return {};
}

expected<unique_ref<llvm::Module>>
gen(llvm::LLVMContext& llvm_ctx, std::string_view const name, typecheck::module_t const& m,
    llvm::TargetMachine& machine) noexcept
{
    auto result = build_empty_module(llvm_ctx, name, machine);
    if (auto ok = gen_impl(*result, m); not ok)
        return std::move(ok.error());
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
    if (auto ok = gen_impl(*result, m); not ok)
        return std::move(ok.error());
    return std::move(result);
}

} // namespace dep0::llvmgen
