#include "dep0/typecheck/error.hpp"

#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <algorithm>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    dep0::pretty_print(os, err) << std::endl;
    os << "In context:" << std::endl;
    if (err.tgt)
    {
        std::ostringstream buf;
        pretty_print(buf, err.ctx);
        auto const& ctx_str = buf.str();
        os << ctx_str << std::endl;
        auto length_of_last_line = 0ul;
        for (auto const& line: std::views::split(std::string_view(ctx_str), '\n'))
            length_of_last_line = std::ranges::distance(line);
        os << std::string(std::max(length_of_last_line, 10ul), '-') << std::endl;
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
    else
    {
        dep0::pretty_print(os, err);
    }
    return os;
}

} // namespace dep0::typecheck
