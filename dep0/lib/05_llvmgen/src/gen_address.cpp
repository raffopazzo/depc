/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_address.hpp"

#include "private/gen_array.hpp"
#include "private/gen_type.hpp"
#include "private/gen_val.hpp"

#include "dep0/typecheck/subscript_access.hpp"

#include "dep0/ast/find_member_field.hpp"

namespace dep0::llvmgen {

namespace impl {

static llvm::Value*
gen_array_element_address(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::subscript_t const& subscript)
{
    auto const& array = subscript.object.get();
    auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
    auto const element_type = gen_type(global, properties.element_type);
    auto const base = gen_temporary_val(global, local, builder, array);
    auto const index_val = gen_temporary_val(global, local, builder, subscript.index.get());
    auto const offset = stride_size ? builder.CreateMul(stride_size, index_val) : index_val;
    return builder.CreateGEP(element_type, base, offset);
}

static llvm::Value*
gen_tuple_element_address(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::subscript_t const& subscript,
    llvm::Type* const tuple_type)
{
    auto const base = gen_temporary_val(global, local, builder, subscript.object.get());
    auto const index_const = std::get_if<typecheck::expr_t::numeric_constant_t>(&subscript.index.get().value);
    assert(index_const and "subscript operand on tuples must be a numeric literal");
    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
    auto const zero = llvm::ConstantInt::get(int32, 0);
    auto const i = index_const->value.convert_to<std::int32_t>();
    auto const index_val = llvm::ConstantInt::get(int32, i);
    return builder.CreateGEP(tuple_type, base, {zero, index_val});
}

} // namespace impl

llvm::Value*
gen_address(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::subscript_t const& subscript)
{
    auto const& object_type = std::get<typecheck::expr_t>(subscript.object.get().properties.sort.get());
    return match(
        typecheck::has_subscript_access(object_type),
        [] (typecheck::has_subscript_access_result::no_t) -> llvm::Value*
        {
            assert(false and "unexpected subscript expression; typechecking must be broken");
            __builtin_unreachable();
        },
        [&] (typecheck::has_subscript_access_result::sigma_t const& sigma) -> llvm::Value*
        {
            auto const tuple_type = gen_type(global, object_type);
            return impl::gen_tuple_element_address(global, local, builder, subscript, tuple_type);
        },
        [&] (typecheck::has_subscript_access_result::array_t const&) -> llvm::Value*
        {
            return impl::gen_array_element_address(global, local, builder, subscript);
        });
}

std::pair<llvm::Value*, llvm::Type*>
gen_field_address(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::member_t const& member)
{
    auto const& object_type = std::get<typecheck::expr_t>(member.object.get().properties.sort.get());
    auto const& g = std::get<typecheck::expr_t::global_t>(object_type.value);
    auto const type_def = std::get_if<global_ctx_t::type_def_t>(global[g]);
    assert(type_def and "only global structs can have member access");
    auto const& s = std::get<typecheck::type_def_t::struct_t>(type_def->def.value);
    auto const i = ast::find_member_index<typecheck::properties_t>(member.field, s);
    assert(i.has_value());
    auto const struct_type = gen_type(global, object_type);
    auto const base = gen_temporary_val(global, local, builder, member.object.get());
    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
    auto const zero = llvm::ConstantInt::get(int32, 0);
    auto const index = llvm::ConstantInt::get(int32, *i);
    auto const ptr = builder.CreateGEP(struct_type, base, {zero, index});
    auto const element_type = gen_type(global, s.fields[*i].type);
    return {ptr, element_type};
}

} // namespace dep0::llvmgen
