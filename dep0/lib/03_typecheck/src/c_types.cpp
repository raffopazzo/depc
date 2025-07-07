/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/c_types.hpp"

#include "dep0/ast/views.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

dep0::expected<std::true_type> is_c_type(parser::expr_t const& x)
{
    auto const no = [&] (std::vector<error_t> reasons = {}) -> dep0::expected<std::true_type>
    {
        return dep0::error_t("not a valid C-type", x.properties, std::move(reasons));
    };
    auto const yes = dep0::expected<std::true_type>{};
    return match(
        x.value,
        [&] (parser::expr_t::typename_t) { return no(); },
        [&] (parser::expr_t::true_t) { return no(); },
        [&] (parser::expr_t::auto_t) { return no(); },
        [&] (parser::expr_t::bool_t) { return yes; },
        [&] (parser::expr_t::cstr_t) { return yes; },
        [&] (parser::expr_t::unit_t) { return no(); },
        [&] (parser::expr_t::i8_t) { return yes; },
        [&] (parser::expr_t::i16_t) { return yes; },
        [&] (parser::expr_t::i32_t) { return yes; },
        [&] (parser::expr_t::i64_t) { return yes; },
        [&] (parser::expr_t::u8_t) { return yes; },
        [&] (parser::expr_t::u16_t) { return yes; },
        [&] (parser::expr_t::u32_t) { return yes; },
        [&] (parser::expr_t::u64_t) { return yes; },
        [&] (parser::expr_t::boolean_constant_t) { return no(); },
        [&] (parser::expr_t::numeric_constant_t) { return no(); },
        [&] (parser::expr_t::string_literal_t) { return no(); },
        [&] (parser::expr_t::boolean_expr_t const&) { return no(); },
        [&] (parser::expr_t::relation_expr_t const&) { return no(); },
        [&] (parser::expr_t::arith_expr_t const&) { return no(); },
        [&] (parser::expr_t::var_t const&) { return no(); },
        [&] (parser::expr_t::global_t const&) { return no(); }, // TODO might be yes for some struct and integer defs
        [&] (parser::expr_t::app_t const&)
        {
            if (auto const arr = get_if_array(x))
                return is_c_type(arr->element_type.get());
            if (auto const ref = get_if_ref(x))
                return is_c_type(ref->element_type.get());
            return no();
        },
        [&] (parser::expr_t::abs_t const&) { return no(); },
        [&] (parser::expr_t::pi_t const& pi) { return is_c_func_type(pi, x.properties); },
        [&] (parser::expr_t::sigma_t const& sigma)
        {
            std::vector<error_t> reasons;
            for (auto const& arg: sigma.args)
                if (auto result = is_c_type(arg.type); not result)
                    reasons.push_back(std::move(result.error()));
            return reasons.empty() ? yes : no(std::move(reasons));
        },
        [&] (parser::expr_t::ref_t) { return no(); },
        [&] (parser::expr_t::scope_t) { return no(); },
        [&] (parser::expr_t::addressof_t const&) { return no(); },
        [&] (parser::expr_t::deref_t const&) { return no(); },
        [&] (parser::expr_t::scopeof_t const&) { return no(); },
        [&] (parser::expr_t::array_t const&) { return no(); },
        [&] (parser::expr_t::init_list_t const&) { return no(); },
        [&] (parser::expr_t::member_t const&) { return no(); },
        [&] (parser::expr_t::subscript_t const&) { return no(); },
        [&] (parser::expr_t::because_t const& x) { return is_c_type(x.value.get()); });
}

dep0::expected<std::true_type> is_c_func_type(parser::expr_t::pi_t const& pi, source_loc_t const origin)
{
    std::vector<error_t> reasons;
    for (auto const& arg: pi.args)
        // TODO allow 0 quantity? eg `f(0 typename t, 0 u64_t n, array_t(t, n) p) -> i32_t` could make `p` a `void*`?
        if (auto result = is_c_type(arg.type); not result)
            reasons.push_back(std::move(result.error()));
    // TODO what about C-functions that return void? shall we allow unit_t for the return type?
    if (auto result = is_c_type(pi.ret_type.get()); not result)
        reasons.push_back(std::move(result.error()));
    if (reasons.empty())
        return {};
    else
        return dep0::error_t("not a valid C-function", origin, std::move(reasons));
}

} // namespace dep0::typecheck

