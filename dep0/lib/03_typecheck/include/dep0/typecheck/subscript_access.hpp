/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::has_subscript_access()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <variant>

namespace dep0::typecheck {

/** @brief Contains the possible results returned from `has_subscript_access()`. */
namespace has_subscript_access_result
{
    /** @brief The type passed to `has_subscript_access()` cannot be accessed via the subscript operator. */
    struct no_t{};

    /** @brief The type passed to `has_subscript_access()` is a tuple, which can be accessed via a subscript. */
    struct sigma_t
    {
        std::vector<func_arg_t> const& args;
    };

    /** @brief The type passed to `has_subscript_access()` is an array, which can be accessed via a subscript. */
    struct array_t
    {
        expr_t const& element_type;
        expr_t const& size;
    };
}

using has_subscript_access_result_t =
    std::variant<
        has_subscript_access_result::no_t,
        has_subscript_access_result::sigma_t,
        has_subscript_access_result::array_t>;

/** @brief Decide whether a value of the given type can be accessed via the subscript operator, eg `xs[0]`. */
has_subscript_access_result_t has_subscript_access(expr_t const& type);

} // namespace dep0::typecheck
