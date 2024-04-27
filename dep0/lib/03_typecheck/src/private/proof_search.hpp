#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of the given type in the given environment and context.
 * If none can be found, returns an empty optional.
 * A term is only viable if its use does not exceed what is allowed by the context.
 * For example, if the given context contains a variable of the desired type,
 * but its quantity is 0, it will only be used if the multiplier is also 0.
 * Similary if its quantity is 1 but it has already been used, then it will not be viable.
 * If no term could be found, the input usage is guaranteed to be unchanged;
 * in other words, the search is performed using a temporary usage object;
 * only once a term is found all usages are added to the input usage object.
 *
 * @param usage,usage_multiplier
 *      @see usage
 */
std::optional<expr_t>
proof_search(
    environment_t const&,
    context_t const&,
    expr_t const& type,
    usage_t& usage,
    ast::qty_t usage_multiplier);

} // namespace dep0::typecheck
