/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_alloca.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

llvm::Value* gen_alloca(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    allocator_t const allocator,
    typecheck::expr_t const& type)
{
    auto const array = get_properties_if_array(type);
    auto const array_size = array ? gen_array_total_size(global, local, builder, *array) : nullptr; // default is 1
    auto const llvm_type = gen_type(global, array ? array->element_type : type);
    if (allocator == allocator_t::stack)
        return builder.CreateAlloca(llvm_type, array_size);
    else
    {
        auto const& data_layout = global.llvm_module.getDataLayout();
        auto const alloca = llvm::CallInst::CreateMalloc(
            builder.GetInsertBlock(),
            builder.getInt64Ty(),
            llvm_type,
            builder.getInt64(data_layout.getTypeAllocSize(llvm_type).getFixedSize()),
            array_size,
            nullptr);
        builder.GetInsertBlock()->getInstList().push_back(alloca);
        return alloca;
    }
}

} // namespace dep0::llvmgen
