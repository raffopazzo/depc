#pragma once

#include "dep0/source.hpp"

namespace dep0::ast {

/**
 * Represents a single attribute, eg `[[builtin]]`.
 * Currently an attribute can only be a single string but
 * in future could be extended to expressions eg `[[assume(x > 0)]]`.
 */
struct attribute_t
{
    source_text value;
};

} // namespace dep0::ast
