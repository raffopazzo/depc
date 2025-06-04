/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Helper functions that return views inside AST nodes.
 */
#pragma once

#include "dep0/ast/ast.hpp"

#include <functional>
#include <optional>

namespace dep0::ast {

/** @brief The return type of `get_if_array` holding a reference to the element type and size of an array. */
template <Properties P>
struct array_view_t
{
    std::reference_wrapper<expr_t<P> const> element_type;
    std::reference_wrapper<expr_t<P> const> size;
};

/** @brief The return type of `get_if_ref` holding a reference to the element type. */
template <Properties P>
struct ref_view_t
{
    std::reference_wrapper<expr_t<P> const> element_type;
    std::reference_wrapper<expr_t<P> const> scope;
};

/**
 * @brief If the argument is a valid array type, return a view to its element type and size.
 *
 * For example `%array_t(%i32_t, n)` and `%array_t(%i32_t, n) because lemma` are valid array types.
 *
 * @remarks A multi-dimensional array has another array in its element type.
 */
template <Properties P>
std::optional<array_view_t<P>> get_if_array(expr_t<P> const& x)
{
    if (auto const because = std::get_if<typename expr_t<P>::because_t>(&x.value))
        return get_if_array(because->value.get());
    std::optional<array_view_t<P>> result;
    if (auto const app = std::get_if<typename expr_t<P>::app_t>(&x.value))
        if (std::holds_alternative<typename expr_t<P>::array_t>(app->func.get().value))
            if (app->args.size() == 2ul)
                result.emplace(std::cref(app->args[0]), std::cref(app->args[1]));
    return result;
}

/**
 * @brief If the argument is a valid reference type, return a view to the type of the referenced element.
 *
 * For example `%ref_t(%i32_t, a)` and `%ref_t(%i32_t, a) because lemma` are valid reference types.
 */
template <Properties P>
std::optional<ref_view_t<P>> get_if_ref(expr_t<P> const& x)
{
    if (auto const because = std::get_if<typename expr_t<P>::because_t>(&x.value))
        return get_if_ref(because->value.get());
    std::optional<ref_view_t<P>> result;
    if (auto const app = std::get_if<typename expr_t<P>::app_t>(&x.value))
        if (std::holds_alternative<typename expr_t<P>::ref_t>(app->func.get().value))
            if (app->args.size() == 2ul)
                result.emplace(std::cref(app->args[0]), std::cref(app->args[1]));
    return result;
}

} // namespace dep0::ast
