#include "dep0/typecheck/context.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include <ranges>

namespace dep0::typecheck {

context_t::context_t(scope_map<ast::indexed_var_t, value_type> values) :
    m_values(std::move(values))
{ }


context_t context_t::extend() const
{
    return context_t(m_values.extend());
}

auto context_t::begin() const -> const_iterator
{
    return m_values.begin();
}

auto context_t::end() const -> const_iterator
{
    return m_values.end();
}

auto context_t::operator[](ast::indexed_var_t const& name) const -> value_type const*
{
    return m_values[name];
}

template <typename R, typename F>
std::ostream& for_each_line(std::ostream& os, R&& r, F&& f)
{
    for (bool first = true; auto const& x: std::forward<R>(r))
    {
        if (not std::exchange(first, false))
            os << std::endl;
        f(x);
    }
    return os;
}

std::ostream& pretty_print(std::ostream& os, context_t const& ctx)
{
    for_each_line(
        os,
        std::ranges::subrange(ctx.begin(), ctx.end()),
        [&] (auto const& x)
        {
            pretty_print(os, x.first) << ": ";
            match(
                x.second,
                [&] (type_def_t const& t) { pretty_print(os, t); },
                [&] (type_t::var_t const& t) { pretty_print<properties_t>(os, t); },
                [&] (expr_t const& x) { pretty_print(os, x.properties.sort); });
        });
    return os;
}

std::ostream& pretty_print(std::ostream& os, context_t::value_type const& v)
{
    return match(v, [&] (auto const& x) -> std::ostream& { return pretty_print<properties_t>(os, x); });
}

} // namespace dep0::typecheck

