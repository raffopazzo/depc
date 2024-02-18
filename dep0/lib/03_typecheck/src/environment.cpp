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
environment_t::try_emplace(expr_t::global_t global, value_type v)
{
    auto const loc = match(v, [] (auto const& x) { return x.properties.origin; }); // take copy before moving v
    auto const res = m_values.try_emplace(std::move(global), std::move(v));
    if (res.second)
    {
        return dep0::expected<const_iterator>(res.first);
    }
    else
    {
        auto const& prev = res.first->second;
        std::ostringstream err;
        pretty_print<properties_t>(err << "cannot redefine `", global) << '`';
        auto const origin = match(prev, [] (auto const& x) { return x.properties.origin; });
        err << ", previously defined at " << origin.line << ':' << origin.col << ", as `";
        match(
            prev,
            [&] (type_def_t const& x) { pretty_print(err, x); },
            [&] (func_decl_t const& x) { pretty_print(err, x.properties.sort.get()); },
            [&] (func_def_t const& x) { pretty_print(err, x.properties.sort.get()); });
        err << '`';
        return dep0::error_t(err.str(), loc);
    }
}

} // namespace dep0::typecheck
