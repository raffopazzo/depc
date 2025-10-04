/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/context.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "dep0/ast/occurs_in.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/ast/size.hpp"
#include "dep0/match.hpp"

#include "private/rewrite.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

ctx_t::ctx_t(scoped_t) : m_flavour(scope_flavour_t::scoped_v), m_scope_id(0) { }

ctx_t::ctx_t(
    scope_flavour_t const flavour,
    std::size_t const scope_id,
    scope_map<source_text, expr_t::var_t> index,
    scope_map<expr_t::var_t, decl_t> values
) :
    m_flavour(flavour),
    m_scope_id(scope_id),
    m_index(std::move(index)),
    m_values(std::move(values))
{ }

// const member functions

std::optional<std::size_t> ctx_t::scope() const
{
    return m_flavour == scope_flavour_t::scoped_v ? std::optional{m_scope_id} : std::nullopt;
}

ctx_t ctx_t::extend() const
{
    return ctx_t(m_flavour, m_scope_id + 1ul, m_index.extend(), m_values.extend());
}

ctx_t ctx_t::extend_scoped() const
{
    return ctx_t(scope_flavour_t::scoped_v, m_scope_id + 1ul, m_index.extend(), m_values.extend());
}

ctx_t ctx_t::extend_unscoped() const
{
    return ctx_t(scope_flavour_t::unscoped_v, m_scope_id + 1ul, m_index.extend(), m_values.extend());
}

ctx_t ctx_t::rewrite(expr_t const& from, expr_t const& to) const
{
    auto new_values = m_values.deep_copy();
    for (auto& [_, decl]: std::ranges::subrange(new_values.cbegin(), new_values.cend()))
        if (auto new_type = typecheck::rewrite(from, to, decl.type))
            // TODO remove const_cast
            const_cast<expr_t&>(decl.type) = std::move(*new_type);
    return ctx_t(m_flavour, m_scope_id, m_index.extend(), std::move(new_values));
}

std::vector<std::reference_wrapper<ctx_t::decl_t const>> ctx_t::decls() const
{
    std::vector<std::reference_wrapper<ctx_t::decl_t const>> result;
    for (auto const& v: std::views::values(std::ranges::subrange(m_values.cbegin(), m_values.cend())))
        result.push_back(std::cref(v));
    return result;
}

ctx_t::decl_t const* ctx_t::operator[](expr_t::var_t const& var) const
{
    return m_values[var];
}

ctx_t::decl_t const* ctx_t::operator[](source_text const& name) const
{
    auto const var = m_index[name];
    return var ? m_values[*var] : nullptr;
}


// non-const member functions

void ctx_t::add_unnamed(ast::qty_t const qty, expr_t type)
{
    static std::size_t unnamed_idx = 0;
    static const source_text empty = source_text::from_literal("auto");
    auto const var = expr_t::var_t{empty, 0ul, unnamed_idx++};
    bool const inserted = m_values.try_emplace(var, std::nullopt, scope(), var, qty, std::move(type)).second;
    assert(inserted and "failed to add unnamed variable to context");
}

dep0::expected<expr_t::var_t>
ctx_t::try_emplace(source_text name, std::optional<source_loc_t> const loc, ast::qty_t const qty, expr_t type)
{
    auto const prev_var = m_index[name];
    auto const shadow_id = prev_var ? prev_var->shadow_id + 1ul : 0ul;
    auto const var = expr_t::var_t{name, 0ul, shadow_id};
    auto const [it, inserted] = m_index.try_emplace(name, var);
    if (inserted)
    {
        bool const inserted = m_values.try_emplace(var, loc, scope(), var, qty, std::move(type)).second;
        assert(inserted and "failed to add named variable to context");
        return var;
    }
    else
    {
        auto const prev = m_values[*prev_var];
        assert(prev and "failed to find previous declaration of existing variable name");
        std::ostringstream err;
        err << "cannot redefine `" << name << '`';
        pretty_print(err << ", previously defined as `", prev->type) << '`';
        if (prev->origin)
            err << " at " << prev->origin->line << ':' << prev->origin->col;
        return dep0::error_t(err.str(), loc);
    }
}

// non-member functions

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

std::ostream& pretty_print(std::ostream& os, ctx_t const& ctx)
{
    auto const length_of = [] (expr_t::var_t const& var)
    {
        // length of the name, plus:
        //   1. length of its quantity ("0 ", "1 ", "  ")
        //   2. and possibly the length of the index with the colon separator
        auto const log10 = [] (std::size_t const x) { return static_cast<std::size_t>(std::log10(x)); };
        return var.name.size() + 2 + (var.idx == 0ul ? 2ul : 2ul + log10(var.idx))
                                   + (var.shadow_id == 0ul ? 2ul : 2ul + log10(var.shadow_id));
    };
    auto const& decls = ctx.decls();
    auto const longest =
        decls.empty() ? 0ul :
        std::ranges::max(decls | std::views::transform([&] (ctx_t::decl_t const& x) { return length_of(x.var); }));
    auto const indent = (longest / 4ul) + 1ul;
    auto const alignment = indent * 4ul;
    for_each_line(
        os,
        decls,
        [&, padding=std::string()] (ctx_t::decl_t const& decl) mutable
        {
            os << (decl.qty == ast::qty_t::zero ? "0 " : decl.qty == ast::qty_t::one ? "1 " : "  ");
            pretty_print<properties_t>(os, decl.var);
            padding.resize(alignment - length_of(decl.var), ' ');
            os << padding << ": ";
            auto copy = decl.type;
            bool const changed = beta_delta_normalize(copy);
            pretty_print(os, changed and ast::size(copy) < ast::size(decl.type) ? copy : decl.type, indent);
        });
    return os;
}

} // namespace dep0::typecheck

