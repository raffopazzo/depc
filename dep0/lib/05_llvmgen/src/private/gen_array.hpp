/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper functions useful when generating LLVM IR dealing with arrays.
 */
#pragma once

#include "private/context.hpp"

#include "dep0/typecheck/ast.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <optional>
#include <vector>

namespace dep0::llvmgen {

/**
 * @brief Holds useful properties to use when emitting LLVM code for (possibly multidimensional) arrays.
 * @warning This object is a view into the type expression from which it was constructed;
 * therefore usage after destruction of the originating type expression is undefined behaviour.
 */
struct array_properties_view_t
{
    /** @brief The type of the elements stored by a (possibly multidimensional) array. */
    typecheck::expr_t const& element_type;

    /**
     * @brief The dimensions for the array, each of which can be a constant, variable or other expression.
     *
     * Contains only one entry for monodimensional arrays and many for multidimensional ones.
     * For example `array_t(array_t(array_t(i32_t, 2), 3), 4)` will have dimensions `{4,3,2}`.
     */
    std::vector<typecheck::expr_t const*> dimensions;
};

/** Return the size of the array described by the given properties, if known at compile-time; nullopt otherwise. */
std::optional<boost::multiprecision::cpp_int> get_compile_time_size(array_properties_view_t const&);

/** Return true if the size of the array described by the given properties is known at compile-time. */
bool has_compile_time_size(array_properties_view_t const&);

/**
 * @brief Checks whether the given type expression is an array, i.e. the application of `array_t`.
 * @param type The type to check, which must have sort `typename_t`.
 * @return True if the given type is an array.
 */
bool is_array(typecheck::expr_t const& type);

/**
 * @brief Try to extract the properties of an array type, if possible.
 * @param type The type expression to inspect, which must have sort `typename_t`.
 * @return The properties of the array type, or an empty optional if the type is not an array.
 */
std::optional<array_properties_view_t> get_properties_if_array(typecheck::expr_t const& type);

/**
 * @brief Extract the properties of the array type, assuming that the input type is in fact an array.
 * @warning If the given expression is not an array type the behaviour is undefined.
 * @param type The type expression to inspect, which must have sort `typename_t`.
 * @return The properties of the given array type.
 */
array_properties_view_t get_array_properties(typecheck::expr_t const& type);

/**
 * @brief Generate an LLVM value representing the total size (in number of elements, not in bytes) of an array.
 *
 * The array can also be multidimensional, for example the total size for `array_t(array_t(i32_t, 3), 4)` is 12.
 */
llvm::Value* gen_array_total_size(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    array_properties_view_t const&);

/**
 * @brief Generate an LLVM value representing the stride size (in number of elements, not in bytes) of an array.
 *
 * For example, the stride size for `array_t(array_t(i32_t, 3), 4)` is 3.
 *
 * @remarks A monodimensional array does not have a stride size.
 * @return The LLVM value representing the stride size if the array is multidimensional, otherwise nullptr.
 */
llvm::Value* gen_stride_size_if_needed(
    global_ctx_t&,
    local_ctx_t const&,
    llvm::IRBuilder<>&,
    array_properties_view_t const&);

} // namespace dep0::llvmgen

