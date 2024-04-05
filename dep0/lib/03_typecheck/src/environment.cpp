#include "dep0/typecheck/environment.hpp"

#include "private/beta_delta_equivalence.hpp"

#include "dep0/ast/pretty_print.hpp"
#include "dep0/match.hpp"

#include <cassert>
#include <sstream>

namespace dep0::typecheck {

environment_t::environment_t(
    scope_map<expr_t::global_t, value_type> fwd_decls,
    scope_map<expr_t::global_t, value_type> definitions
) : m_fwd_decls(std::move(fwd_decls)),
    m_definitions(std::move(definitions))
{ }

// const member functions

environment_t environment_t::extend() const
{
    return environment_t(m_fwd_decls.extend(), m_definitions.extend());
}

environment_t::value_type const* environment_t::operator[](expr_t::global_t const& global) const
{
    if (auto const p = m_definitions[global])
        return p;
    else
        return m_fwd_decls[global];
}

// non-const member functions

dep0::expected<std::true_type> environment_t::try_emplace(expr_t::global_t global, value_type v)
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
                [&] (func_decl_t const&) { err << "cannot add function declaration "; },
                [&] (func_def_t const&) { err << "cannot add function definition "; });
            pretty_print<properties_t>(err << '`', global) << '`';
            auto const origin = match(prev, [] (auto const& x) { return x.properties.origin; });
            err << ", previously defined at " << origin.line << ':' << origin.col << ", as `";
            match(
                prev,
                [&] (type_def_t const& x) { pretty_print(err, x); },
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
        [&] (func_decl_t const& decl) -> dep0::expected<std::true_type>
        {
            // if we already encountered a function declaration or definition with this name,
            // the two signatures must be equivalent
            if (auto const* const prev = this->operator[](global))
                return match(
                    *prev,
                    [&] (type_def_t const&) -> dep0::expected<std::true_type>
                    {
                        return reject(*prev);
                    },
                    [&] (auto const& old) -> dep0::expected<std::true_type>
                    {
                        if (is_beta_delta_equivalent(*this, {}, decl.properties.sort.get(), old.properties.sort.get()))
                            return std::true_type{};
                        else
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
