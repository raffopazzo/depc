/*
 * Copyright Raffaele Rossi 2023 - 2024.
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
llvm::FunctionType* gen_func_type(global_ctx_t&, llvm_func_proto_t const&);

/** @brief Return the LLVM integer type of the given bit width. */
llvm::IntegerType* gen_type(global_ctx_t&, ast::width_t);

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
 */
llvm::Type* gen_type(global_ctx_t&, typecheck::expr_t const& type);

/** @brief Contains the possible results returned by `pass_by_ptr()` */
namespace pass_by_ptr_result
{
    /** @brief The type passed to `pass_by_ptr()` is pass-by-value. */
    struct no_t{};

    /** @brief The type passed to `pass_by_ptr()` is pass-by-ptr because it is a tuple `(t1; ...; tN)`. */
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
 */
bool is_boxed(typecheck::expr_t const& type);

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
