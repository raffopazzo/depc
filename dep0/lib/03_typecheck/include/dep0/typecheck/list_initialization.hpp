#pragma once

#include "dep0/typecheck/ast.hpp"

#include <variant>

namespace dep0::typecheck {

/**
 * Contains the possible results returned by `is_list_initializable()`.
 */
namespace is_list_initializable_result
{
    /**
     * The type cannot be initialized by an initializer list.
     */
    struct no_t{};

    /**
     * The type is `true_t(true)`, which can be initialized by `{}`.
     */
    struct true_t{};

    /**
     * The type is an `array_t(type, N)` for some constant `N`,
     * which can be initialized by `{value1, ..., valueN}`.
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
        is_list_initializable_result::true_t,
        is_list_initializable_result::array_t>;

/**
 * Decide whether the given type can be initialized by a (possibly empty) initializer list,
 * i.e. an expression of the form `{expr1, ..., exprN}`.
 */
is_list_initializable_result_t is_list_initializable(expr_t const& type);

} // namespace dep0::typecheck
