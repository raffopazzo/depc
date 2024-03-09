#pragma once

#include "dep0/typecheck/ast.hpp"

#include <boost/logic/tribool.hpp>

namespace dep0::typecheck {

/**
 * @return  true if the statement definitely contains side effects;
 *          false if it definitely does not contain any side effect;
 *          indeterminate if it might contain side effects.
 */
boost::logic::tribool has_side_effects(stmt_t const&);

/**
 * @return  true if the expression definitely contains side effects;
 *          false if it definitely does not contain any side effect;
 *          indeterminate if it might contain side effects.
 */
boost::logic::tribool has_side_effects(expr_t const&);

} // namespace dep0::typecheck
