#include "dep0/ast/pretty_print.hpp"

namespace dep0::ast {

std::ostream& pretty_print(std::ostream& os, indexed_var_t const& x, std::size_t)
{
    os << x.txt;
    if (x.idx)
        os << ':' << x.idx;
    return os;
}

} // namespace dep0::ast
