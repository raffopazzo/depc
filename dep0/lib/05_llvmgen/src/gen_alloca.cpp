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
    if (auto const sigma = std::get_if<typecheck::expr_t::sigma_t>(&type.value))
    {
        if (result)
            result->emplace<needs_alloca_result::sigma_t>(sigma->args);
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

llvm::AllocaInst* gen_alloca_if_needed(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type)
{
    auto const alloca = match(
        needs_alloca(type),
        [] (needs_alloca_result::no_t) -> llvm::AllocaInst* { return nullptr; },
        [&] (needs_alloca_result::sigma_t const& sigma) -> llvm::AllocaInst*
        {
            // TODO we might need to malloc dependent tuples, which is not an `alloca`
            return builder.CreateAlloca(gen_type(global, type));
        },
        [&] (needs_alloca_result::array_t const& array) -> llvm::AllocaInst*
        {
            auto const total_size = gen_array_total_size(global, local, builder, array.properties);
            auto const type = gen_type(global, array.properties.element_type);
            return builder.CreateAlloca(type, total_size);
        });
    if (alloca)
        move_to_entry_block(alloca, builder.GetInsertBlock()->getParent());
    return alloca;
}

void move_to_entry_block(llvm::AllocaInst* const alloca, llvm::Function* const f)
{
    // scan the entry block and place the new `alloca` before the first non-`alloca`
    for (llvm::Instruction& i: f->getEntryBlock())
        if (auto const alloca2 = llvm::dyn_cast<llvm::AllocaInst>(&i))
        {
            if (alloca2 == alloca)
                return; // coincidentally, the new `alloca` is already in the "alloca group", so leave it here
        }
        else
        {
            alloca->moveBefore(&i);
            return;
        }
    assert(false and "entry block does not have a terminator");
}

} // namespace dep0::llvmgen
