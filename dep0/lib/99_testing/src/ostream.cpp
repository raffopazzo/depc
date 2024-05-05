#include "dep0/testing/ostream.hpp"

namespace dep0::ast {

std::ostream& operator<<(std::ostream& os, is_mutable_t const m)
{
    return os << (m == is_mutable_t::yes ? "yes" : "no");
}

} // namespace dep0::ast
