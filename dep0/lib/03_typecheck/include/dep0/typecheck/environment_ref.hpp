/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::typecheck::env_ref_t`.
 */
#pragma once

#include <memory>

namespace dep0::typecheck {

struct env_t;

/**
 * @brief Immutable and read-only reference to an environment that can be cheaply copied.
 *
 * Immutable means that insertions to the original environment, after a reference is constructed,
 * will not be visible via the reference.
 * This allows legal expressions to carry around a reference to the environment in which they were typechecked.
 */
class env_ref_t
{
    std::shared_ptr<env_t> env;

public:
    explicit env_ref_t(env_t const&);

    env_t const& operator*() const { return *env; }
    env_t const* operator->() const { return env.get(); }
};

} // namespace dep0::typecheck
