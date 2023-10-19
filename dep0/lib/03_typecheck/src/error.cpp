#include "dep0/typecheck/error.hpp"
#include "dep0/ast/pretty_print.hpp"

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    dep0::pretty_print(os, err) << std::endl;
    os << "In context:" << std::endl;
    pretty_print(os, err.ctx);
    if (err.tgt)
        pretty_print(os << std::endl << "------------" << std::endl, *err.tgt);
    return os;
}

} // namespace dep0::typecheck
