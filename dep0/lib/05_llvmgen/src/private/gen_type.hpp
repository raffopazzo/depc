/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Set of functions to generate LLVM types.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "private/context.hpp"
#include "private/gen_array.hpp"
#include "private/proto.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace dep0::llvmgen {

/** @brief Generate an LLVM function type for the given prototype. */
llvm::FunctionType* gen_func_type(global_ctx_t const&, llvm_func_proto_t const&);

/** @brief Return the LLVM integer type of the given bit width. */
llvm::IntegerType* gen_type(global_ctx_t const&, ast::width_t);

/**
 * @brief Generate the LLVM type for the given type expression, which must be a 1st order type.
 * @param type Must be an expression of sort `typename_t` and it must represent a 1st order type.
 *
 * @remarks
 * The LLVM type generated is the best representation in LLVM of the DepC type.
 * This may be a primitive LLVM type, an LLVM struct or some pointer thereof.
 * This type is the "base type" used when generating other types or `getelementptr` instructions.
 * But when invoking a function, you may or may not need to obtain a pointer type
 * dependeing on the result of `pass_by_ptr()` and `is_pass_by_ptr()`.
 *
 * @see
 *   - @ref first_order_types
 *   - @ref first_order_types.hpp
 *   - @ref pass_by_ptr_vs_pass_by_value
 *   - @ref boxed_types
 */
llvm::Type* gen_type(global_ctx_t const&, typecheck::expr_t const& type);

/** @brief Contains the possible results returned by `pass_by_ptr()` */
namespace pass_by_ptr_result
{
    /** @brief The type passed to `pass_by_ptr()` is pass-by-value. */
    struct no_t{};

    /** @brief The type passed to `pass_by_ptr()` is pass-by-ptr because it is a struct. */
    struct struct_t
    {
        std::vector<typecheck::type_def_t::struct_t::field_t> const& fields;
    };

    /** @brief The type passed to `pass_by_ptr()` is pass-by-ptr because it is a tuple `(t1; ..., tN)`. */
    struct sigma_t
    {
        std::vector<typecheck::func_arg_t> const& args;
    };

    /** @brief The type passed to `pass_by_ptr()` is pass-by-ptr because it is an array. */
    struct array_t
    {
        array_properties_view_t properties;
    };
}
using pass_by_ptr_result_t =
    std::variant<
        pass_by_ptr_result::no_t,
        pass_by_ptr_result::struct_t,
        pass_by_ptr_result::sigma_t,
        pass_by_ptr_result::array_t>;

/**
 * @brief Decides whether the given type is pass-by-ptr or pass-by-value when invoking a function.
 * @see @ref pass_by_ptr_vs_pass_by_value
 */
pass_by_ptr_result_t pass_by_ptr(global_ctx_t const&, typecheck::expr_t const& type);

/** @brief Returns true if the given type is pass-by-ptr; false if it is pass-by-value. */
bool is_pass_by_ptr(global_ctx_t const&, typecheck::expr_t const&);

/** @brief Returns true if the given type is pass-by-value; false if it is pass-by-ptr. */
inline bool is_pass_by_val(global_ctx_t const& global, typecheck::expr_t const& type)
{
    return not is_pass_by_ptr(global, type);
}

/**
 * @brief Returns true if the given type is boxed, for example arrays are boxed.
 * @see @ref boxed_types
 */
bool is_boxed(typecheck::expr_t const& type);

/**
 * @brief Returns true if the given type is trivially destructible.
 *
 * Similarly to C++, a type is trivially destructible if deallocating its storage is enough.
 * For example primitive types and tuples of primitive types are trivially destructible.
 * Conversely, tuples containing boxed types are not because it might be necessary to release heap-allocated memory.
 */
bool is_trivially_destructible(global_ctx_t const&, typecheck::expr_t const& type);

inline bool is_trivially_copyable(global_ctx_t const& global, typecheck::expr_t const& type)
{
    // currently all trivially copyable types are also trivially destructible but this may change in future
    return is_trivially_destructible(global, type);
}

} // namespace dep0::llvmgen

/**
 * @page pass_by_ptr_vs_pass_by_value Pass-by-Pointer vs Pass-by-Value
 *
 * When invoking a function we need to decide whether arguments should be
 * passed by pointer or by value, and likewise for return values.
 * For primitive types you obviously always pass (and return) by value.
 * For more complex types it makes more sense to pass by pointer (or pointer to return value).
 * In some cases, it is even recommended by LLVM, for example for a `struct`
 * they recommend to pass by pointer and use `getelementptr` instead of
 * passing by value and using `insert/extractvalue` instructions.
 *
 * However, when generating an LLVM type, for example for the tuple `(i16_t, (i32_t, u32_t))`,
 * you intuitively expect `{i16, {i32, i32}}` not `{i16, {i32, i32}*}`.
 *
 * The decision of whether a pointer is required or not when invoking a function
 * is delegated to `dep0::llvmgen::pass_by_ptr()` whereas `dep0::llvmgen::gen_type()`
 * will only generate the LLVM type that best represents the given DepC type.
 *
 * This does not mean that `gen_type()` will never emit a pointer type;
 * for example for C-strings and functions it returns a pointer;
 * but in this case the pointer *is* the value to pass to (or return from) the function.
 */

/**
 * @page boxed_types Boxed Types
 *
 * When generating machine code you need to decide how a DepC type is represented in memory at run-time.
 * For primitive types and tuples of primitive types, for example `i8_t` or  `(i32_t, (i8_t, u16_t))`,
 * the solution is straightforward: you just lay out the correct number of bytes in memory corresponding to
 * the size of the primitive type or to the fields of the tuple, leaving padding between fields if necessary.
 * But with dependent types there is a problem, which is best illustrated using arrays.
 * The size of an array depends on its length, which can be known either at compile-time or at run-time.
 * For example the size of `array_t(i32_t, n)` is known only at run-time but `array_t(i32_t, 3)` is 12 bytes long.
 * With dependent types it is possible to cast between these two types in either direction.
 * In this example, if you somehow learn that `n = 3` you can cast from `array_t(i32_t, n)` to `array_t(i32_t, 3)`.
 * You can also cast in the opposite direction from `array_t(i32_t, 3)` to `array_t(i32_t, n)` by setting `n = 3`.
 * This means that for arrays the compiler cannot just lay out 12 bytes for a value of type `array_t(i32_t, 3)`.
 *
 * The solution in these cases is *Boxing*, which is the typical way of implementing type-erasure.
 * A type is boxed if its size cannot be proven constant in all possible cases.
 * Note that, as per the previous example, this happens regardless of whether the size is known at compile-time or not.
 * A type with size known at compile-time can still be cast to a type with size known at run-time.
 *
 * It is important to also know that Boxing does not necessarily imply heap-allocation.
 * if the compiler can prove that a boxed type does not ever escape from the current function,
 * then the box will refer to memory allocated on the stack.
 */
