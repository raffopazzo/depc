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

namespace dep0::typecheck {

/**
 * @brief Proof that an AST node is legal because it has a valid derivation.
 *
 * For example, when specialized to `expr_t`, this is a proof that a term or a type is legal.
 * By extension, when specialized to other AST nodes, this is a proof that the node is legal.
 *
 * It can only be constructed using `derivation_rules`, which is private within the typecheck module;
 * therefore typechecking is the only way to construct a legal AST.
 *
 * @remarks Currently this contains no fields so the only thing that it is proving is that
 * the AST node was constructed via `derivation_rules`.
 * Ideally each specialization should contain the real proof of why the node was legal but
 * that requires dependent types in C++.
 * It might still be useful to add some fields in some cases, but currently there are none.
 *
 * @warning This type is copiable, so one could forge a derivation by copying from another one.
 * But we are trying to "guard against Murphy, not Machiavelli".
 */
template <typename /* NodeType */>
struct derivation_t
{
    derivation_t(derivation_t const&) = default;
    derivation_t& operator=(derivation_t const&) = default;
    derivation_t(derivation_t&&) = default;
    derivation_t& operator=(derivation_t&&) = default;

    bool operator==(derivation_t const&) const = default;

    // TODO could add some properties of the specific derivation, if useful.

private:
    friend struct derivation_rules;
    derivation_t() = default;
};

} // namespace dep0::typecheck
