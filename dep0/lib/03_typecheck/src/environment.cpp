#include "dep0/typecheck/environment.hpp"

#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include <sstream>

namespace dep0::typecheck {

environment_t::environment_t(scope_map<expr_t::global_t, value_type> values) :
    m_values(std::move(values))
{ }

// const member functions

environment_t environment_t::extend() const
{
    return environment_t(m_values.extend());
}

environment_t::value_type const* environment_t::operator[](expr_t::global_t const& global) const
{
    return m_values[global];
}

// non-const member functions

dep0::expected<environment_t::const_iterator>
environment_t::try_emplace(
    expr_t::global_t global,
    std::optional<source_loc_t> const loc,
    std::variant<func_def_t, type_def_t> v)
{
    auto const res = m_values.try_emplace(std::move(global), loc, std::move(v));
    if (res.second)
    {
        return dep0::expected<const_iterator>(res.first);
    }
    else
    {
        auto const& prev = res.first->second;
        std::ostringstream err;
        pretty_print<properties_t>(err << "cannot redefine `", global) << '`';
        err << ", previously defined ";
        if (prev.origin)
            err << "at " << prev.origin->line << ':' << prev.origin->col << " ";
        err << "as `";
        match(
            prev.value,
            [&] (func_def_t const& x) { pretty_print(err, x.properties.sort.get()); },
            [&] (type_def_t const& x) { pretty_print(err, x); });
        err << '`';
        return dep0::error_t(err.str(), loc);
    }
}

} // namespace dep0::typecheck
