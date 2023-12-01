#include "dep0/typecheck/context.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include "private/rewrite.hpp"

#include "dep0/ast/beta_delta_reduction.hpp"

#include <cassert>
#include <ranges>

namespace dep0::typecheck {

context_t::context_t(scope_map<expr_t::var_t, value_type> values, delta_reduction_context_t ctx) :
    m_values(std::move(values)),
    m_delta_reduction_context(std::move(ctx))
{ }


context_t context_t::extend() const
{
    return context_t(m_values.extend(), m_delta_reduction_context.extend());
}

auto context_t::begin() const -> const_iterator
{
    return m_values.begin();
}

auto context_t::end() const -> const_iterator
{
    return m_values.end();
}

auto context_t::operator[](expr_t::var_t const& name) const -> value_type const*
{
    return m_values[name];
}

std::set<expr_t::var_t> context_t::vars() const
{
    std::set<expr_t::var_t> result;
    for (auto x = std::optional{m_values}; x.has_value(); x = x->parent())
        for (auto const kv: *x)
            result.insert(kv.first);
    return result;
}

context_t context_t::rewrite(expr_t const& from, expr_t const& to) const
{
    auto result = extend();
    for (auto const& var: vars())
        match(
            *(*this)[var],
            [] (type_def_t const&) { },
            [&] (expr_t const& expr)
            {
                if (auto new_sort = typecheck::rewrite(from, to, expr.properties.sort.get()))
                {
                    // TODO should check that the new type actually got shorter after reduction,
                    // in case we only managed to perform delta-reduction, in which case better
                    // stick to the original, as it's easier to read for the user in case of error
                    match(
                        *new_sort,
                        [&] (expr_t& new_type) { ast::beta_delta_normalize(m_delta_reduction_context, new_type); },
                        [] (kind_t) { });
                    auto new_expr = expr;
                    new_expr.properties.sort = std::move(*new_sort);
                    bool const inserted = result.try_emplace(var, new_expr).second;
                    assert(inserted);
                }
            });
    return result;
}

auto context_t::delta_reduction_context() const -> delta_reduction_context_t const&
{
    return m_delta_reduction_context;
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
        ctx.vars(),
        [&] (expr_t::var_t const& var)
        {
            auto const val = ctx[var];
            assert(val);
            pretty_print<properties_t>(os, var) << ": ";
            match(
                *val,
                [&] (type_def_t const& t) { pretty_print(os, t); },
                [&] (expr_t const& x) { pretty_print(os, x.properties.sort.get()); });
        });
    return os;
}

std::ostream& pretty_print(std::ostream& os, context_t::value_type const& v)
{
    return match(v, [&] (auto const& x) -> std::ostream& { return pretty_print<properties_t>(os, x); });
}

} // namespace dep0::typecheck

