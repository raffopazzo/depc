#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"
#include "dep0/scope_map.hpp"

#include <optional>
#include <variant>

namespace dep0::typecheck {

/**
 * An environment contains the definitions that can be unfolded during delta-reduction.
 */
class environment_t
{
public:
    struct value_type
    {
        std::optional<source_loc_t> origin;
        std::variant<func_def_t, type_def_t> value;
    };

    using const_iterator = typename scope_map<expr_t::global_t, value_type>::const_iterator;

    environment_t() = default;
    environment_t(environment_t const&) = default;
    environment_t& operator=(environment_t const&) = default;
    environment_t(environment_t&&) = default;
    environment_t& operator=(environment_t&&) = default;

    // const member functions
    environment_t extend() const;
    value_type const* operator[](expr_t::global_t const&) const;

    // non-const member functions
    dep0::expected<const_iterator>
        try_emplace(expr_t::global_t, std::optional<source_loc_t>, std::variant<func_def_t, type_def_t>);

private:
    scope_map<expr_t::global_t, value_type> m_values;

    environment_t(scope_map<expr_t::global_t, value_type>);
};

} // namespace dep0::typecheck
