#include "dep0/typecheck/ast.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, kind_t, std::size_t indent)
{
    return os << "<kind>";
}

std::ostream& pretty_print(std::ostream& os, sort_t const& sort, std::size_t indent)
{
    match(sort, [&] (auto const& x) { pretty_print(os, x, indent); });
    return os;
}

} // namespace dep0::typecheck
