#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/scope_map.hpp"

#include <vector>

namespace dep0::typecheck {

struct func_proto_t // this should really be in the AST
{
    type_t ret_type;
    std::vector<func_def_t::arg_t> args;
};

std::ostream& pretty_print(std::ostream&, func_proto_t const&);

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
    scope_map<source_text, entry_t<type_def_t>> m_typedefs;
    scope_map<source_text, entry_t<func_proto_t>> m_protos;
    scope_map<source_text, entry_t<func_def_t::arg_t>> m_args;

    context_t(
        scope_map<source_text, entry_t<type_def_t>>,
        scope_map<source_text, entry_t<func_proto_t>>,
        scope_map<source_text, entry_t<func_def_t::arg_t>>);

public:
    using typedefs_const_iterator = typename scope_map<source_text, entry_t<type_def_t>>::const_iterator;
    using protos_const_iterator = typename scope_map<source_text, entry_t<func_proto_t>>::const_iterator;
    using args_const_iterator = typename scope_map<source_text, entry_t<func_def_t::arg_t>>::const_iterator;

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
    entry_t<func_proto_t> const* find_proto(source_text const&) const;
    entry_t<func_def_t::arg_t> const* find_arg(source_text const&) const;

    // TODO add some way to store where a certain typedef/proto/arg was encountered,
    // so if we find duplicates we can inform the user where the previous definition/declaration was

    template <typename... Args>
    auto try_emplace_typedef(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_typedefs.try_emplace(std::move(name), loc, type_def_t{std::forward<Args>(args)...});
    }

    template <typename... Args>
    auto try_emplace_proto(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_protos.try_emplace(std::move(name), loc, func_proto_t{std::forward<Args>(args)...});
    }

    template <typename... Args>
    auto try_emplace_arg(source_text name, source_loc_t loc, Args&&... args)
    {
        return m_args.try_emplace(std::move(name), loc, func_def_t::arg_t{std::forward<Args>(args)...});
    }
};

std::ostream& pretty_print(std::ostream& os, context_t const&);

} // namespace dep0::typecheck
