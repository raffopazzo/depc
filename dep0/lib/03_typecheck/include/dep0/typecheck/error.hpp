#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"

#include "dep0/error.hpp"

#include <ostream>

namespace dep0::typecheck {

// Extend the basic error type with the context information available when the typecheck failure occurred.
struct error_t : dep0::error_t
{
    std::optional<std::pair<context_t, sort_t>> context;

    static error_t from_error(dep0::error_t);
    static error_t from_error(dep0::error_t, context_t, sort_t);
};

template <typename T>
using expected = dep0::expected<T, error_t>;

std::ostream& pretty_print(std::ostream& os, error_t const&);

} // namespace dep0::typecheck
