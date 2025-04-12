/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_list_initializable()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/maybe_const_ref.hpp"

#include <variant>

namespace dep0::typecheck {

/** @brief Contains the possible results returned from `is_list_initializable()`. */
namespace is_list_initializable_result
{
    /** @brief The type passed to `is_list_initializable()` cannot be initialized with an initializer list. */
    struct no_t{};

    /** @brief The type passed to `is_list_initializable()`  is `unit_t`, which can be initialized with `{}`. */
    struct unit_t{};

    /** @brief The type passed to `is_list_initializable()` is `true_t(true)`, which can be initialized with `{}`. */
    struct true_t{};

    /**
     * @brief The type passed to `is_list_initializable()` is a tuple `(t1; ...; tN)`.
     *
     * The tuple can be initialized with `{value1, ..., valueN}` of appropriate types,
     * which may depend on previous values.
     */
    template <const_t Const>
    struct sigma_t
    {
        maybe_const_ref<Const, std::vector<func_arg_t>> args;

        /**
         * @brief Allows converting from ref to const-ref.
         * @details This is useful when pattern matching on a mutable reference but the case handler is immutable.
         */
        operator sigma_t<const_t::yes>() const { return {args}; }
    };
    using sigma_const_t = sigma_t<const_t::yes>;
    using sigma_ref_t = sigma_t<const_t::no>;

    /**
     * @brief The type passed to `is_list_initializable()` is an `array_t(type, N)` for some constant `N`,
     * which can be initialized with `{value1, ..., valueN}`.
     */
    template <const_t Const>
    struct array_t
    {
        maybe_const_ref<Const, expr_t> element_type;
        maybe_const_ref<Const, expr_t::numeric_constant_t> size;

        /**
         * @brief Allows converting from ref to const-ref.
         * @details This is useful when pattern matching on a mutable reference but the case handler is immutable.
         */
        operator array_t<const_t::yes>() const { return {element_type, size}; }
    };
    using array_const_t = array_t<const_t::yes>;
    using array_ref_t = array_t<const_t::no>;
};

template <const_t Const>
using is_list_initializable_result_t =
    std::variant<
        is_list_initializable_result::no_t,
        is_list_initializable_result::unit_t,
        is_list_initializable_result::true_t,
        is_list_initializable_result::sigma_t<Const>,
        is_list_initializable_result::array_t<Const>>;

/**
 * @brief Decide whether the given type can be initialized with a (possibly empty) initializer list,
 * i.e. an expression of the form `{expr1, ..., exprN}`.
 */
is_list_initializable_result_t<const_t::yes> is_list_initializable(expr_t const& type);
is_list_initializable_result_t<const_t::no> is_list_initializable(expr_t& type);

} // namespace dep0::typecheck
