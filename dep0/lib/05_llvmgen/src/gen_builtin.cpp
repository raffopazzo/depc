/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_builtin.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/typecheck/builtin_call.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

/** Generate the LLVM value that results from a call to `::slice()`. */
static llvm::Value* gen_builtin_slice(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::is_builtin_call_result::slice_t const& slice,
    typecheck::expr_t const& ret_type,
    llvm::Value* const dest)
{
    auto const properties = get_array_properties(std::get<typecheck::expr_t>(slice.xs.properties.sort.get()));
    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
    auto const type = gen_type(global, properties.element_type);
    auto const base = gen_temporary_val(global, local, builder, slice.xs);
    auto const index = gen_temporary_val(global, local, builder, slice.k);
    auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
    auto const ptr = builder.CreateGEP(type, base, offset);
    return maybe_gen_store(global, local, builder, ptr, dest, ret_type);
}

llvm::Value* try_gen_builtin(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
    return match(
        typecheck::is_builtin_call(app),
        [] (typecheck::is_builtin_call_result::no_t) -> llvm::Value* { return nullptr; },
        [&] (typecheck::is_builtin_call_result::slice_t const& slice)
        {
            auto const& func_type = std::get<typecheck::expr_t>(app.func.get().properties.sort.get());
            auto const& ret_type = std::get<typecheck::expr_t::pi_t>(func_type.value).ret_type.get();
            return gen_builtin_slice(global, local, builder, slice, ret_type, dest);
        });
}

} // namespace dep0::llvmgen

