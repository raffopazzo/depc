#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/ast/delta_reduction.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <cassert>
#include <ostream>
#include <set>
#include <vector>

namespace dep0::typecheck {

class context_t
{
public:
    struct value_type
    {
        source_loc_t origin;
        std::variant<type_def_t, expr_t> value;
    };

    using const_iterator = typename scope_map<expr_t::var_t, value_type>::const_iterator;
    using delta_reduction_context_t = ast::delta_reduction::context_t<properties_t>;

    context_t() = default;
    context_t(context_t const&) = default;
    context_t& operator=(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;

    // const member functions
    delta_reduction_context_t const& delta_reduction_context() const;
    context_t extend() const;
    context_t rewrite(expr_t const& from, expr_t const& to) const;
    std::set<expr_t::var_t> vars() const;
    value_type const* operator[](expr_t::var_t const&) const;

    // non-const member functions
    dep0::expected<const_iterator> try_emplace(expr_t::var_t, source_loc_t, std::variant<type_def_t, expr_t>);

private:
    scope_map<expr_t::var_t, value_type> m_values;
    delta_reduction_context_t m_delta_reduction_context;

    context_t(scope_map<expr_t::var_t, value_type>, delta_reduction_context_t);
};

// non-member functions
std::ostream& pretty_print(std::ostream&, context_t const&);
std::ostream& pretty_print(std::ostream&, context_t::value_type const&);

} // namespace dep0::typecheck
