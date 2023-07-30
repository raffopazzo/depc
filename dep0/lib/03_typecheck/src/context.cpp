#include "dep0/typecheck/context.hpp"
#include "dep0/fmap.hpp"
#include "dep0/match.hpp"

#include <ranges>

namespace dep0::typecheck {

context_t::context_t(
    scope_map<source_text, entry_t<type_def_t>> typedefs,
    scope_map<source_text, entry_t<type_t::arr_t>> protos,
    scope_map<source_text, entry_t<expr_t::abs_t::arg_t>> args
) : m_typedefs(std::move(typedefs)),
    m_protos(std::move(protos)),
    m_args(std::move(args))
{ }


context_t context_t::extend() const
{
    return context_t(
        m_typedefs.extend(),
        m_protos.extend(),
        m_args.extend());
}

auto context_t::typedefs_begin() const -> typedefs_const_iterator
{
    return m_typedefs.begin();
}

auto context_t::typedefs_end() const -> typedefs_const_iterator
{
    return m_typedefs.end();
}

auto context_t::protos_begin() const -> protos_const_iterator
{
    return m_protos.begin();
}

auto context_t::protos_end() const -> protos_const_iterator
{
    return m_protos.end();
}

auto context_t::args_begin() const -> args_const_iterator
{
    return m_args.begin();
}

auto context_t::args_end() const -> args_const_iterator
{
    return m_args.end();
}

auto context_t::find_typedef(source_text const& name) const -> entry_t<type_def_t> const*
{
    return m_typedefs[name];
}

auto context_t::find_proto(source_text const& name) const -> entry_t<type_t::arr_t> const*
{
    return m_protos[name];
}

auto context_t::find_arg(source_text const& name) const -> entry_t<expr_t::abs_t::arg_t> const*
{
    return m_args[name];
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
        std::ranges::subrange(ctx.protos_begin(), ctx.protos_end()),
        [&] (auto const& x)
        {
            pretty_print(os << x.first << ": ", x.second.value);
        });
    for_each_line(
        os,
        std::ranges::subrange(ctx.args_begin(), ctx.args_end()),
        [&] (auto const& x)
        {
            pretty_print(os << x.first << ": ", x.second.value.sort);
        });
    return os;
}

} // namespace dep0::typecheck

