#include "private/gen_alloca.hpp"

#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

needs_alloca_result_t needs_alloca(typecheck::expr_t const& type)
{
    if (auto const app = get_if_app_of_array(type))
        return needs_alloca_result::array_t{app->args[0ul], app->args[1ul]};
    return needs_alloca_result::no_t{};
}

bool is_alloca_needed(typecheck::expr_t const& type)
{
    return not std::holds_alternative<needs_alloca_result::no_t>(needs_alloca(type));
}

llvm::Instruction* gen_alloca(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type,
    llvm::Value* const size)
{
    return builder.CreateAlloca(gen_type(global, local, type), size);
}

llvm::Instruction* gen_alloca_if_needed(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type)
{
    return match(
        needs_alloca(type),
        [] (needs_alloca_result::no_t) -> llvm::Instruction* { return nullptr; },
        [&] (needs_alloca_result::array_t const& array) -> llvm::Instruction*
        {
            auto const size = gen_val(global, local, builder, array.size, nullptr);
            return gen_alloca(global, local, builder, array.type, size);
        });
}

} // namespace dep0::llvmgen
