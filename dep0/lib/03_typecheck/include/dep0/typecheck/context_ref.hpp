/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::ctx_ref_t`.
 */
#pragma once

#include <memory>

namespace dep0::typecheck {

struct ctx_t;

/**
 * @brief Immutable and read-only reference to a typing context that can be cheaply copied.
 *
 * Immutable means that insertions to the original context, after a reference is constructed,
 * will not be visible via the reference.
 * This allows legal expressions to carry around a reference to the context in which they were typechecked.
 */
class ctx_ref_t
{
    std::shared_ptr<ctx_t> ctx;

public:
    explicit ctx_ref_t(ctx_t const&);

    ctx_t const& operator*() const { return *ctx; }
    ctx_t const* operator->() const { return ctx.get(); }
};

} // namespace dep0::typecheck
