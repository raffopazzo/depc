/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines the concept of "transformation" for the transformation stage of the compiler pipeline.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"

#include <concepts>
#include <functional>
#include <type_traits>

namespace dep0::transform {

/**
 * @brief Transformations modify a module in-place, for example to perform optimisations.
 * 
 * The transformation can modify the AST at will but **must not** "break" types,
 * i.e. all types in the final AST must be beta-delta-equivalent to
 * the ones in the initial AST.
 *
 * A transformation must either succeed or return the reason why it failed.
 */
template <typename F>
concept Transform = std::is_invocable_r_v<expected<std::true_type>, F, typecheck::module_t&>;

/** @brief Type-erasing adapter of a transformation, which is itself a transformation. */
using transform_t = std::function<expected<std::true_type>(typecheck::module_t&)>;
static_assert(Transform<transform_t>);

} // namespace dep0::transform
