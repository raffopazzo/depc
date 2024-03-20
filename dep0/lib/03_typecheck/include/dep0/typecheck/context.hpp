#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <ostream>
#include <set>

namespace dep0::typecheck {

/**
 * A context contains terms currently bound to binding variables, i.e. the function scope.
 */
class context_t
{
public:
    struct value_type
    {
        std::optional<source_loc_t> origin;
        expr_t value;
    };

    using const_iterator = typename scope_map<expr_t::var_t, value_type>::const_iterator;

    context_t() = default;
    context_t(context_t const&) = default;
    context_t& operator=(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;

    // const member functions
    context_t extend() const;
    context_t rewrite(expr_t const& from, expr_t const& to) const;
    std::set<expr_t::var_t> vars() const;
    value_type const* operator[](expr_t::var_t const&) const;

    // non-const member functions
    dep0::expected<const_iterator> try_emplace(expr_t::var_t, std::optional<source_loc_t>, expr_t);

private:
    scope_map<expr_t::var_t, value_type> m_values;

    context_t(scope_map<expr_t::var_t, value_type>);
};

// non-member functions
std::ostream& pretty_print(std::ostream&, context_t const&);
std::ostream& pretty_print(std::ostream&, context_t::value_type const&);

} // namespace dep0::typecheck
