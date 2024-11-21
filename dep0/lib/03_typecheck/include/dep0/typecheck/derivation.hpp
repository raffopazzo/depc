/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

namespace dep0::typecheck {

// In general, a derivation is a proof that a term or a type is valid, although currently it has no properties.
// By extension, it can also be a proof that other AST nodes (eg func_def_t or module_t) are valid.
// It can only be constructed using `derivation_rules`, which is private within the typecheck module;
// therefore typechecking is the only way to construct a legal AST.
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
