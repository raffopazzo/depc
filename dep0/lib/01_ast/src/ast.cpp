#include "dep0/ast/ast.hpp"

namespace dep0::ast {

std::ostream& pretty_print(std::ostream& os, indexed_var_t const& var)
{
    os << var.txt;
    if (var.idx)
        os << ':' << var.idx;
    return os;
}

} // namespace dep0::ast
