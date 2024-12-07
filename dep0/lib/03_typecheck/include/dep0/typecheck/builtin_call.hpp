/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_builtin_call()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <variant>

namespace dep0::typecheck {

/** @brief All possible results returned from `is_builtin_call()`. */
namespace is_builtin_call_result
{
    /** @brief The expression passed to `is_builtin_call()` is not a call to a builtin function. */
    struct no_t{};

    /** @brief The expression passed to `is_builtin_call()`  is a call of the form `::slice(t, n, xs, k, p)`. */
    struct slice_t
    {
        expr_t const& t;
        expr_t const& n;
        expr_t const& xs;
        expr_t const& k;
        expr_t const& p;
    };
}

using is_builtin_call_result_t =
    std::variant<
        is_builtin_call_result::no_t,
        is_builtin_call_result::slice_t>;

/**
 * @brief Decide whether the given expression is a call to some builtin function
 *
 * For example `::slice(cstr_t, argc, argv, 1, auto)`.
 */
is_builtin_call_result_t is_builtin_call(expr_t::app_t const&);

} // namespace dep0::typecheck
