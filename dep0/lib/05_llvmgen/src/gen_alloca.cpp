/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_alloca.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

llvm::AllocaInst* gen_alloca(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& type)
{
    auto const alloca = [&]
    {
        auto const array_properties = get_properties_if_array(type);
        if (array_properties)
        {
            auto const total_size = gen_array_total_size(global, local, builder, *array_properties);
            auto const llvm_type = gen_type(global, array_properties->element_type);
            return builder.CreateAlloca(llvm_type, total_size);
        }
        else
            return builder.CreateAlloca(gen_type(global, type));
    }();
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
