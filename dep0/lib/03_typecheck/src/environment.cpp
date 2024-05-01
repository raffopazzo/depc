#include "dep0/typecheck/environment.hpp"

#include "private/beta_delta_equivalence.hpp"

#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include <cassert>
#include <ranges>
#include <sstream>

namespace dep0::typecheck {

env_t::env_t(
    scope_map<expr_t::global_t, value_type> fwd_decls,
    scope_map<expr_t::global_t, value_type> definitions
) : m_fwd_decls(std::move(fwd_decls)),
    m_definitions(std::move(definitions))
{ }

// const member functions

env_t env_t::extend() const
{
    return env_t(m_fwd_decls.extend(), m_definitions.extend());
}

std::set<expr_t::global_t> env_t::globals() const
{
    std::set<expr_t::global_t> result;
    for (auto const& m: {m_definitions, m_fwd_decls})
        for (auto x = std::optional{m}; x.has_value(); x = x->parent())
            std::ranges::copy(std::views::keys(*x), std::inserter(result, result.end()));
    return result;
}

env_t::value_type const* env_t::operator[](expr_t::global_t const& global) const
{
    if (auto const p = m_definitions[global])
        return p;
    else
        return m_fwd_decls[global];
}

// non-const member functions

dep0::expected<std::true_type> env_t::try_emplace(expr_t::global_t global, value_type v)
{
    auto const accept =
        [&] (scope_map<expr_t::global_t, value_type>& dest) -> dep0::expected<std::true_type>
        {
            bool const inserted = dest.try_emplace(std::move(global), std::move(v)).second;
            assert(inserted);
            return std::true_type{};
        };
    auto const reject =
        [&] (value_type const& prev) -> dep0::expected<std::true_type>
        {
            std::ostringstream err;
            match(
                v,
                [&] (type_def_t const&) { err << "cannot add type definition "; },
                [&] (axiom_t const&) { err << "cannot add axiom "; },
                [&] (extern_decl_t const&) { err << "cannot add extern function declaration "; },
                [&] (func_decl_t const&) { err << "cannot add function declaration "; },
                [&] (func_def_t const&) { err << "cannot add function definition "; });
            pretty_print<properties_t>(err << '`', global) << '`';
            auto const origin = match(prev, [] (auto const& x) { return x.properties.origin; });
            err << ", previously defined at " << origin.line << ':' << origin.col << ", as `";
            match(
                prev,
                [&] (type_def_t const& x) { pretty_print(err, x); },
                [&] (axiom_t const& x) { pretty_print(err, x); },
                [&] (extern_decl_t const& x) { pretty_print(err, x.properties.sort.get()); },
                [&] (func_decl_t const& x) { pretty_print(err, x.properties.sort.get()); },
                [&] (func_def_t const& x) { pretty_print(err, x.properties.sort.get()); });
            err << '`';
            auto const loc = match(v, [] (auto const& x) { return x.properties.origin; });
            return dep0::error_t(err.str(), loc);
        };
    return match(
        v,
        [&] (type_def_t const&) -> dep0::expected<std::true_type>
        {
            // a new type definition must have a new unique name
            if (auto const* const prev = this->operator[](global))
                return reject(*prev);
            else
                return accept(m_definitions);
        },
        [&] (axiom_t const& axiom) -> dep0::expected<std::true_type>
        {
            // a new axiom must have a new unique name
            if (auto const* const prev = this->operator[](global))
                return reject(*prev);
            else
                return accept(m_definitions);
        },
        [&] (extern_decl_t const& decl) -> dep0::expected<std::true_type>
        {
            // a new extern declaration must have a new unique name
            if (auto const* const prev = this->operator[](global))
                return reject(*prev);
            else
                return accept(m_definitions);
        },
        [&] (func_decl_t const& decl) -> dep0::expected<std::true_type>
        {
            // if we already encountered a function declaration or definition with this name,
            // the two signatures must be equivalent
            if (auto const* const prev = this->operator[](global))
                return match(
                    *prev,
                    [&] <typename T> (T const& old) -> dep0::expected<std::true_type>
                    requires (std::is_same_v<T, func_decl_t> or std::is_same_v<T, func_def_t>)
                    {
                        if (is_beta_delta_equivalent(*this, {}, decl.properties.sort.get(), old.properties.sort.get()))
                            return std::true_type{};
                        else
                            return reject(*prev);
                    },
                    [&] (auto const& old) -> dep0::expected<std::true_type>
                    {
                        return reject(*prev);
                    });
            else
                return accept(m_fwd_decls);
        },
        [&] (func_def_t const& def) -> dep0::expected<std::true_type>
        {
            // if we already encountered a function declaration (but not a definition!) with this name,
            // the two signatures must be equivalent
            if (auto const* const prev = this->operator[](global))
                return match(
                    *prev,
                    [&] (func_decl_t const& decl) -> dep0::expected<std::true_type>
                    {
                        if (is_beta_delta_equivalent(*this, {}, def.properties.sort.get(), decl.properties.sort.get()))
                            return accept(m_definitions);
                        else
                            return reject(*prev);
                    },
                    [&] (auto const&) -> dep0::expected<std::true_type>
                    {
                        return reject(*prev);
                    });
            else
                return accept(m_definitions);
        });
}

} // namespace dep0::typecheck
