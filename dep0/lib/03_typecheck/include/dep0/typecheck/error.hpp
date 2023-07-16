#pragma once

#include "dep0/typecheck/tt/context.hpp"
#include "dep0/error.hpp"

#include <ostream>

namespace dep0::typecheck {

// Extend the basic error type with the context information available when the typecheck failure occurred.
struct error_t : dep0::error_t
{
    tt::context_t ctx;
    std::optional<tt::type_t> tgt;

    static error_t from_error(dep0::error_t err, tt::context_t ctx)
    {
        return error_t{std::move(err), std::move(ctx)};
    }
    static error_t from_error(dep0::error_t err, tt::context_t ctx, tt::type_t tgt)
    {
        return error_t{std::move(err), std::move(ctx), std::move(tgt)};
    }
};

template <typename T>
using expected = dep0::expected<T, error_t>;

std::ostream& pretty_print(std::ostream& os, error_t const&);

} // namespace dep0::typecheck
