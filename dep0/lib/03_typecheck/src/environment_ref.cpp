/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/environment_ref.hpp"

#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

env_ref_t::env_ref_t(env_t const& env) : env(std::make_shared<env_t>(std::move(env.extend()))) { }

} // namespace dep0::typecheck
