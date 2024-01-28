#include "private/gen_alloca.hpp"

#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

/**
 * Helper method to share implementation code between needs_alloca and is_alloca_needed.
 *
 * @param type      The type expression originally passed to needs_alloca and is_alloca_needed.
 * @param result    Optional pointer to the result, used by needs_alloca.
 *
 * @return bool True if the input type expression requires allocation.
 */
static bool needs_alloca_impl(typecheck::expr_t const& type, needs_alloca_result_t* const result)
{
    if (auto array_properties = get_properties_if_array(type))
    {
        if (result)
            result->emplace<needs_alloca_result::array_t>(std::move(*array_properties));
        return true;
    }
    if (result)
        result->emplace<needs_alloca_result::no_t>();
    return false;
}

needs_alloca_result_t needs_alloca(typecheck::expr_t const& type)
{
    needs_alloca_result_t result;
    needs_alloca_impl(type, &result);
    return result;
}

bool is_alloca_needed(typecheck::expr_t const& type)
{
    return needs_alloca_impl(type, nullptr);
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
            auto const total_size = gen_array_total_size(global, local, builder, array.properties);
            auto const type = gen_type(global, local, array.properties.element_type);
            return builder.CreateAlloca(type, total_size);
        });
}

} // namespace dep0::llvmgen
