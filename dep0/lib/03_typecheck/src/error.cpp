#include "dep0/typecheck/error.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "dep0/ast/pretty_print.hpp"

#include <algorithm>
#include <ranges>
#include <sstream>

namespace cxx23 {

template <typename R, typename T, typename F>
T fold_left(R&& range, T init, F&& f)
{
    for (auto const& x: std::forward<R>(range))
        init = f(init, x);
    return init;
}

} // namespace cxx23

namespace dep0::typecheck {

error_t error_t::from_error(dep0::error_t err)
{
    return error_t{std::move(err), std::nullopt};
}

error_t error_t::from_error(dep0::error_t err, environment_t env, context_t ctx, sort_t tgt)
{
    return error_t{std::move(err), std::tuple{std::move(env), std::move(ctx), std::move(tgt)}};
}

std::ostream& pretty_print(std::ostream& os, error_t const& err)
{
    dep0::pretty_print(os, err);
    if (err.context)
    {
        auto const [ctx_str, tgt_str] = [&]
        {
            auto const& [env, ctx, sort] = *err.context;
            std::ostringstream buf1, buf2;
            pretty_print(buf1, ctx);
            match(
                sort,
                [&] (expr_t type)
                {
                    beta_delta_normalize(env, ctx, type);
                    pretty_print(buf2, type);
                },
                [&] (kind_t)
                {
                    pretty_print(buf2, kind_t{});
                });
            return std::pair{buf1.str(), buf2.str()};
        }();
        os << std::endl << "In context:" << std::endl;
        os << ctx_str << std::endl;
        auto const num_dashes =
            cxx23::fold_left(
                std::views::split(std::string_view(ctx_str), '\n'),
                tgt_str.size(),
                [] (std::size_t const acc, auto const& line)
                {
                    return std::max(acc, static_cast<std::size_t>(std::ranges::distance(line)));
                });
        os << std::string(num_dashes, '-') << std::endl;
        os << tgt_str;
    }
    return os;
}

} // namespace dep0::typecheck
