/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "private/beta_reduction.hpp"
#include "private/delta_unfold.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

namespace impl {

static bool beta_delta_normalize(axiom_t&);
static bool beta_delta_normalize(extern_decl_t&);
static bool beta_delta_normalize(type_def_t&);
static bool beta_delta_normalize(func_decl_t&);
static bool beta_delta_normalize(func_def_t&);
static bool beta_delta_normalize(sort_t&);
static bool beta_delta_normalize(body_t&);

bool beta_delta_normalize(axiom_t& axiom)
{
    bool changed = false;
    for (func_arg_t& arg: axiom.signature.args)
        changed |= beta_delta_normalize(arg.type);
    changed |= beta_delta_normalize(axiom.signature.ret_type.get());
    changed |= beta_delta_normalize(axiom.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(extern_decl_t&)
{
    // extern declarations cannot contain dependencies or other type-expressions,
    // so there is nothing to normalize here
    return false;
}

bool beta_delta_normalize(type_def_t& def)
{
    return match(
        def.value,
        [] (type_def_t::integer_t const&) { return false; },
        [] (type_def_t::struct_t& s)
        {
            bool changed = true;
            for (auto& field: s.fields)
                changed |= beta_delta_normalize(field.type);
            return changed;
        });
}

bool beta_delta_normalize(func_decl_t& decl)
{
    bool changed = false;
    for (func_arg_t& arg: decl.signature.args)
        changed |= beta_delta_normalize(arg.type);
    changed |= beta_delta_normalize(decl.signature.ret_type.get());
    changed |= beta_delta_normalize(decl.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(func_def_t& def)
{
    bool changed = false;
    for (func_arg_t& arg: def.value.args)
        changed |= beta_delta_normalize(arg.type);
    changed |= beta_delta_normalize(def.value.ret_type.get());
    changed |= beta_delta_normalize(def.value.body);
    changed |= beta_delta_normalize(def.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(sort_t& sort)
{
    return match(
        sort,
        [] (expr_t& type) { return beta_delta_normalize(type); },
        [] (kind_t const&) { return false; });
}

bool beta_delta_normalize(body_t& body)
{
    bool changed = beta_normalize(body);
    while (delta_unfold(body))
    {
        changed = true;
        beta_normalize(body);
    }
    return changed;
}

} // namespace impl

bool beta_delta_normalize(module_t& m)
{
    bool changed = false;
    for (auto& x: m.entries)
        changed |= match(x, [] (auto& x) { return impl::beta_delta_normalize(x); });
    return changed;
}

bool beta_delta_normalize(expr_t& expr)
{
    bool changed = beta_normalize(expr);
    while (delta_unfold(expr))
    {
        changed = true;
        beta_normalize(expr);
    }
    return changed;
}

} // namespace dep0::typecheck
