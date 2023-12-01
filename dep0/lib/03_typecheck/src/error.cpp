#include "dep0/typecheck/error.hpp"

#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/pretty_print.hpp"

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    dep0::pretty_print(os, err) << std::endl;
    os << "In context:" << std::endl;
    pretty_print(os, err.ctx);
    if (err.tgt)
    {
        os << std::endl << "------------" << std::endl;
        match(
            *err.tgt,
            [&] (expr_t type)
            {
                ast::beta_delta_normalize(err.ctx.delta_reduction_context(), type);
                pretty_print(os, type);
            },
            [&] (kind_t)
            {
                pretty_print(os, kind_t{});
            });
    }
    return os;
}

} // namespace dep0::typecheck
