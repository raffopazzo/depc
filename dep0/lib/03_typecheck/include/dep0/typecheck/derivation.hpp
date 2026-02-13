/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::derivation_t`
 */
#pragma once

#include "dep0/typecheck/ast_properties.hpp"
#include "dep0/typecheck/context_ref.hpp"
#include "dep0/typecheck/environment_ref.hpp"

#include "dep0/ast/ast.hpp"

namespace dep0::typecheck {

struct location_map_t;

template <typename NodeType>
struct derivation_properties_t
{ };

template <>
struct derivation_properties_t<ast::expr_t<properties_t>>
{
    env_ref_t env;
    ctx_ref_t ctx;
};

template <>
struct derivation_properties_t<ast::body_t<properties_t>>
{
    std::optional<ctx_ref_t> next_ctx;
    boost::recursive_wrapper<location_map_t> location_map;
};

template <>
struct derivation_properties_t<ast::stmt_t<properties_t>>
{
    std::optional<ctx_ref_t> next_ctx;
    boost::recursive_wrapper<location_map_t> location_map;
};

/**
 * @brief Proof that an AST node is legal because it has a valid derivation.
 *
 * For example, when specialized to `expr_t`, this is a proof that a term or a type is legal.
 * By extension, when specialized to other AST nodes, this is a proof that the node is legal.
 *
 * It can only be constructed using `derivation_rules`, which is private within the typecheck module;
 * therefore typechecking is the only way to construct a legal AST.
 *
 * @warning This type is copiable, so one could forge a derivation by copying from another one.
 * But we are trying to "guard against Murphy, not Machiavelli".
 */
template <typename NodeType>
struct derivation_t : derivation_properties_t<NodeType>
{
    derivation_t(derivation_t const&) = default;
    derivation_t& operator=(derivation_t const&) = default;
    derivation_t(derivation_t&&) = default;
    derivation_t& operator=(derivation_t&&) = default;

    bool operator==(derivation_t const&) const = default;

private:
    friend struct derivation_rules;
    derivation_t(derivation_properties_t<NodeType> properties)
        : derivation_properties_t<NodeType>(std::move(properties))
    { }
};

} // namespace dep0::typecheck
