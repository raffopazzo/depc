#include "dep0/typecheck/context.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"

#include <ranges>

namespace dep0::typecheck {

context_t::context_t(scope_map<source_text, value_type> values) :
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

auto context_t::operator[](source_text const& name) const -> value_type const*
{
    return m_values[name];
}

template <typename R, typename F>
std::ostream& for_each_line(std::ostream& os, R&& r, F&& f)
{
    bool first = true;
    for (auto const& x: std::forward<R>(r))
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
            match(
                x.second,
                [&] (type_def_t const& t)
                {
                    pretty_print(os << x.first << ": ", t);
                },
                [&] (type_t const& t)
                {
                    pretty_print(os << x.first << ": ", t);
                },
                [&] (expr_t const& t)
                {
                    pretty_print(os << x.first << ": ", t.properties.sort);
                });
        });
    return os;
}

} // namespace dep0::typecheck

