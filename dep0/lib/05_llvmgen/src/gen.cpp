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
    global_context_t global(llvm_module.get());
    // TODO: allow declarations in any order with 2 pass (both type and function defs)
    for (auto const& def: m.type_defs)
    {
        auto const& name = match(def.value, [] (auto const& x) -> auto const& { return x.name; });
        bool const inserted = global.try_emplace(typecheck::expr_t::global_t{name}, def).second;
        assert(inserted);
    }
    // LLVM can only generate functions for 1st order abstractions;
    // for 2nd order abstractions we rely on beta-delta normalization to produce a value or a 1st order application;
    for (auto const& decl: m.func_decls)
        if (auto proto = llvm_func_proto_t::from_pi(decl.signature))
            gen_func_decl(global, typecheck::expr_t::global_t{decl.name}, *proto);
    for (auto const& def: m.func_defs)
        if (auto proto = llvm_func_proto_t::from_abs(def.value))
            gen_func(global, typecheck::expr_t::global_t{def.name}, *proto, def.value);
    std::string err;
    llvm::raw_string_ostream ostream(err);
    return verify == verify_t::yes and llvm::verifyModule(llvm_module.get(), &ostream) // yes true means false...
        ? expected<unique_ref<llvm::Module>>{error_t{err}}
        : expected<unique_ref<llvm::Module>>{std::in_place, std::move(llvm_module)};
}

} // namespace dep0::llvmgen
