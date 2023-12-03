#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

dep0::expected<std::true_type> is_beta_delta_equivalent(context_t const&, sort_t const&, sort_t const&);

} // namespace dep0::typecheck
