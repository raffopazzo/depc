#include "dep0/llvmgen/gen.hpp"

#include "private/context.hpp"
#include "private/gen_func.hpp"
#include "private/proto.hpp"

#include "dep0/match.hpp"

#include <llvm/IR/Verifier.h>

#include <string>

namespace dep0::llvmgen {

expected<unique_ref<llvm::Module>>
    gen(llvm::LLVMContext& llvm_ctx, std::string_view const name, typecheck::module_t const& m, verify_t verify)
    noexcept
{
    auto llvm_module = make_ref<llvm::Module>(name, llvm_ctx);
    global_ctx_t global(llvm_module.get());
    for (auto const& x: m.entries)
        match(
            x,
            [&] (typecheck::type_def_t const& def)
            {
                auto const& name = match(def.value, [] (auto const& x) -> auto const& { return x.name; });
                bool const inserted = global.try_emplace(typecheck::expr_t::global_t{name}, def).second;
                assert(inserted);
            },
            [] (typecheck::axiom_t const&)
            {
                // axioms cannot be invoked at run-time, so we don't need to do anything here
            },
            [&] (typecheck::extern_decl_t const& decl)
            {
                if (auto proto = llvm_func_proto_t::from_pi(decl.signature))
                    gen_extern_decl(global, typecheck::expr_t::global_t{decl.name}, *proto);
            },
            // LLVM can only generate functions for 1st order abstractions;
            // for 2nd order abstractions we rely on beta-delta normalization to produce
            // a value or a 1st order application;
            [&] (typecheck::func_decl_t const& decl)
            {
                if (auto proto = llvm_func_proto_t::from_pi(decl.signature))
                    gen_func_decl(global, typecheck::expr_t::global_t{decl.name}, *proto);
            },
            [&] (typecheck::func_def_t const& def)
            {
                if (auto proto = llvm_func_proto_t::from_abs(def.value))
                    gen_func(global, typecheck::expr_t::global_t{def.name}, *proto, def.value);
            });
    std::string err;
    llvm::raw_string_ostream ostream(err);
    return verify == verify_t::yes and llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

} // namespace dep0::llvmgen
