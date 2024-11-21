/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <variant>

namespace dep0::typecheck {

/**
 * Contains the possible results returned from `is_list_initializable()`.
 */
namespace is_list_initializable_result
{
    /**
     * The type cannot be initialized with an initializer list.
     */
    struct no_t{};

    /**
     * The type is `unit_t`, which can be initialized with `{}`.
     */
    struct unit_t{};

    /**
     * The type is `true_t(true)`, which can be initialized with `{}`.
     */
    struct true_t{};

    /**
     * The type is an `array_t(type, N)` for some constant `N`,
     * which can be initialized with `{value1, ..., valueN}`.
     */
    struct array_t
    {
        expr_t const& element_type;
        expr_t::numeric_constant_t const& size;
    };
};

using is_list_initializable_result_t =
    std::variant<
        is_list_initializable_result::no_t,
        is_list_initializable_result::unit_t,
        is_list_initializable_result::true_t,
        is_list_initializable_result::array_t>;

/**
 * Decide whether the given type can be initialized with a (possibly empty) initializer list,
 * i.e. an expression of the form `{expr1, ..., exprN}`.
 */
is_list_initializable_result_t is_list_initializable(expr_t const& type);

} // namespace dep0::typecheck
