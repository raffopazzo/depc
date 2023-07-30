#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/scope_map.hpp"

#include <vector>

namespace dep0::typecheck {

class context_t
{
public:
    template <typename T>
    struct entry_t
    {
        source_loc_t loc;
        T value;
    };
private:
    // TODO alternatively consider having only one `scope_map<>` of `variant<>` for a more robust shadowing check
    scope_map<source_text, entry_t<type_def_t>> m_typedefs;
    scope_map<source_text, entry_t<type_t::arr_t>> m_protos; // TODO should this be types in general?
    scope_map<source_text, entry_t<expr_t::abs_t::arg_t>> m_args; // TODO also local variables at some point

    context_t(
        scope_map<source_text, entry_t<type_def_t>>,
        scope_map<source_text, entry_t<type_t::arr_t>>,
        scope_map<source_text, entry_t<expr_t::abs_t::arg_t>>);

public:
    using typedefs_iterator = typename scope_map<source_text, entry_t<type_def_t>>::iterator;
    using protos_iterator = typename scope_map<source_text, entry_t<type_t::arr_t>>::iterator;
    using args_iterator = typename scope_map<source_text, entry_t<expr_t::abs_t::arg_t>>::iterator;

    using typedefs_const_iterator = typename scope_map<source_text, entry_t<type_def_t>>::const_iterator;
    using protos_const_iterator = typename scope_map<source_text, entry_t<type_t::arr_t>>::const_iterator;
    using args_const_iterator = typename scope_map<source_text, entry_t<expr_t::abs_t::arg_t>>::const_iterator;

    context_t() = default;
    context_t(context_t const&) = default;
    context_t& operator=(context_t const&) = default;
    context_t(context_t&&) = default;
    context_t& operator=(context_t&&) = default;

    context_t extend() const;

    // Allow iteration over the members of the current scope level.
    // Use `parent()` if you want to walk up the stack.
    typedefs_const_iterator typedefs_begin() const;
    typedefs_const_iterator typedefs_end() const;
    protos_const_iterator protos_begin() const;
    protos_const_iterator protos_end() const;
    args_const_iterator args_begin() const;
    args_const_iterator args_end() const;

    entry_t<type_def_t> const* find_typedef(source_text const&) const;
    entry_t<type_t::arr_t> const* find_proto(source_text const&) const;
    entry_t<expr_t::abs_t::arg_t> const* find_arg(source_text const&) const;

    template <typename... Args>
    auto try_emplace_typedef(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_typedefs.try_emplace(std::move(name), loc, type_def_t{std::forward<Args>(args)...});
    }

    template <typename... Args>
    auto try_emplace_proto(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_protos.try_emplace(std::move(name), loc, type_t::arr_t{std::forward<Args>(args)...});
    }

    template <typename... Args>
    auto try_emplace_arg(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_args.try_emplace(std::move(name), loc, expr_t::abs_t::arg_t{std::forward<Args>(args)...});
    }
};

std::ostream& pretty_print(std::ostream& os, context_t const&);

} // namespace dep0::typecheck
