#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/scope_map.hpp"

#include <vector>

namespace dep0::typecheck {

class context_t
{
public:
    using value_type = std::variant<type_def_t, type_t, expr_t>;

private:
    scope_map<source_text, value_type> m_values;

    context_t(scope_map<source_text, value_type>);

public:
    using iterator = typename scope_map<source_text, value_type>::iterator;
    using const_iterator = typename scope_map<source_text, value_type>::const_iterator;

    context_t() = default;
    context_t(context_t const&) = default;
    context_t& operator=(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;

    context_t extend() const;

    // Allow iteration over the members of the current scope level.
    // Use `parent()` if you want to walk up the stack.
    const_iterator begin() const;
    const_iterator end() const;

    value_type const* operator[](source_text const&) const;

    template <typename... Args>
    auto try_emplace(source_text name, Args&&... args)
    {
        return m_values.try_emplace(std::move(name), std::forward<Args>(args)...);
    }
};

std::ostream& pretty_print(std::ostream& os, context_t const&);

} // namespace dep0::typecheck
