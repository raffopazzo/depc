/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_alloca.hpp"

#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

/**
 * Helper function to share implementation code between needs_alloca and is_alloca_needed.
 *
 * @param type
 *      The type expression originally passed to needs_alloca and is_alloca_needed.
 *
 * @param result
 *      Optional pointer to the result, used by needs_alloca.
 *
 * @return bool True if the input type expression requires allocation.
 */
static bool needs_alloca_impl(typecheck::expr_t const& type, needs_alloca_result_t* const result)
{
    if (is_array(type))
    {
        if (result)
            result->emplace<needs_alloca_result::array_t>(get_array_properties(type));
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

llvm::Instruction* gen_alloca_if_needed(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type)
{
    return match(
        needs_alloca(type),
        [] (needs_alloca_result::no_t) -> llvm::Instruction* { return nullptr; },
        [&] (needs_alloca_result::array_t const& array) -> llvm::Instruction*
        {
            auto const total_size = gen_array_total_size(global, local, builder, array.properties);
            auto const type = gen_type(global, array.properties.element_type);
            return builder.CreateAlloca(type, total_size);
        });
}

} // namespace dep0::llvmgen
