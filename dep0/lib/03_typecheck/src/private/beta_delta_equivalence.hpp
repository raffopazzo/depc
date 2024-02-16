#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * Checks whether two types are equivalent under beta-delta conversion rules;
 * if they are not equivalent, it returns the reason why.
 */
dep0::expected<std::true_type>
is_beta_delta_equivalent(
    environment_t const&,
    context_t const&,
    sort_t const&,
    sort_t const&);

} // namespace dep0::typecheck
