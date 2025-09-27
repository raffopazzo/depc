/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/context_ref.hpp"

#include "dep0/typecheck/context.hpp"

namespace dep0::typecheck {

ctx_ref_t::ctx_ref_t(ctx_t const& ctx) : ctx(std::make_shared<ctx_t>(std::move(ctx.extend()))) {}

} // namespace dep0::typecheck
