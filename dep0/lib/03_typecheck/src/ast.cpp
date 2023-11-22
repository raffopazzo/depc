#include "dep0/typecheck/ast.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, sort_t const& sort, std::size_t indent)
{
    match(
        sort,
        [&] (expr_t const& expr) { pretty_print(os, expr, indent); },
        [&] (kind_t) { os << "<kind>"; });
    return os;
}

} // namespace dep0::typecheck
