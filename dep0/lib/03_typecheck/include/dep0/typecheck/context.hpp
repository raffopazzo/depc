#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/ast/delta_reduction.hpp"
#include "dep0/scope_map.hpp"

#include <cassert>
#include <vector>

namespace dep0::typecheck {

class context_t
{
public:
    using delta_reduction_context_t = ast::delta_reduction::context_t<properties_t>;
    using value_type = std::variant<type_def_t, expr_t>;

private:
    // TODO key could be `expr_t::var_t` now
    scope_map<ast::indexed_var_t, value_type> m_values;
    delta_reduction_context_t m_delta_reduction_context;

    context_t(scope_map<ast::indexed_var_t, value_type>, delta_reduction_context_t);

public:
    using iterator = typename scope_map<ast::indexed_var_t, value_type>::iterator;
    using const_iterator = typename scope_map<ast::indexed_var_t, value_type>::const_iterator;

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

    value_type const* operator[](ast::indexed_var_t const&) const;

    template <typename... Args>
    auto try_emplace(ast::indexed_var_t name, Args&&... args)
    {
        auto const res = m_values.try_emplace(std::move(name), std::forward<Args>(args)...);
        if (res.second)
        {
            if (auto const expr = std::get_if<expr_t>(&res.first->second))
                if (auto const abs = std::get_if<expr_t::abs_t>(&expr->value))
                {
                    bool const inserted = m_delta_reduction_context.try_emplace(res.first->first, *abs).second;
                    assert(inserted);
                    return res;
                }
            bool const inserted =
                m_delta_reduction_context.try_emplace(
                    res.first->first,
                    ast::delta_reduction::something_else_t{}
                ).second;
            assert(inserted);
        }
        return res;
    }

    delta_reduction_context_t const& delta_reduction_context() const;
};

std::ostream& pretty_print(std::ostream&, context_t const&);
std::ostream& pretty_print(std::ostream&, context_t::value_type const&);

} // namespace dep0::typecheck
