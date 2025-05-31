/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/unification.hpp"

#include "private/beta_delta_equivalence.hpp"

#include "dep0/ast/alpha_equivalence.hpp"

#include <boost/hana.hpp>

#include <ranges>

namespace dep0::typecheck {

namespace impl {
bool unify(expr_t const& from, expr_t const& to, std::map<expr_t::var_t, expr_t>& result)
{
    auto constexpr fail_if_different_types =
        [] <typename T, typename U> (T const&, U const&)
        requires (not std::is_same_v<T, U>)
        {
            return false;
        };
    return std::visit(
        boost::hana::overload(
            fail_if_different_types,
            [] (expr_t::typename_t, expr_t::typename_t) { return true; },
            [] (expr_t::true_t, expr_t::true_t) { return true; },
            [] (expr_t::auto_t, expr_t::auto_t) { return true; },
            [] (expr_t::bool_t, expr_t::bool_t) { return true; },
            [] (expr_t::cstr_t, expr_t::cstr_t) { return true; },
            [] (expr_t::unit_t, expr_t::unit_t) { return true; },
            [] (expr_t::i8_t, expr_t::i8_t) { return true; },
            [] (expr_t::i16_t, expr_t::i16_t) { return true; },
            [] (expr_t::i32_t, expr_t::i32_t) { return true; },
            [] (expr_t::i64_t, expr_t::i64_t) { return true; },
            [] (expr_t::u8_t, expr_t::u8_t) { return true; },
            [] (expr_t::u16_t, expr_t::u16_t) { return true; },
            [] (expr_t::u32_t, expr_t::u32_t) { return true; },
            [] (expr_t::u64_t, expr_t::u64_t) { return true; },
            [] (expr_t::boolean_constant_t const& x, expr_t::boolean_constant_t const& y)
            {
                return x.value == y.value;
            },
            [] (expr_t::numeric_constant_t const& x, expr_t::numeric_constant_t const& y)
            {
                // TOOD should we compare the two types here? if so, add a test; if not, explain why.
                return x.value == y.value;
            },
            [] (expr_t::string_literal_t const& x, expr_t::string_literal_t const& y)
            {
                return x.value == y.value;
            },
            [&] (expr_t::boolean_expr_t const& x, expr_t::boolean_expr_t const& y)
            {
                return std::visit(
                    boost::hana::overload(
                        fail_if_different_types,
                        [&] (expr_t::boolean_expr_t::not_t const& x, expr_t::boolean_expr_t::not_t const& y)
                        {
                            return unify(x.expr.get(), y.expr.get(), result);
                        },
                        [&] <typename T> (T const& x, T const& y)
                        {
                            return unify(x.lhs.get(), y.lhs.get(), result) and unify(x.rhs.get(), y.rhs.get(), result);
                        }),
                    x.value, y.value);
            },
            [&] (expr_t::relation_expr_t const& x, expr_t::relation_expr_t const& y)
            {
                return std::visit(
                    boost::hana::overload(
                        fail_if_different_types,
                        [&] <typename T> (T const& x, T const& y)
                        {
                            return unify(x.lhs.get(), y.lhs.get(), result) and unify(x.rhs.get(), y.rhs.get(), result);
                        }),
                    x.value, y.value);
            },
            [&] (expr_t::arith_expr_t const& x, expr_t::arith_expr_t const& y)
            {
                return std::visit(
                    boost::hana::overload(
                        fail_if_different_types,
                        [&] <typename T> (T const& x, T const& y)
                        {
                            return unify(x.lhs.get(), y.lhs.get(), result) and unify(x.rhs.get(), y.rhs.get(), result);
                        }),
                    x.value, y.value);
            },
            [&] (expr_t::var_t const& x, auto const&)
            {
                // TODO should also consider non-empty environments and contexts
                if (not is_beta_delta_equivalent({}, {}, from.properties.sort.get(), to.properties.sort.get()))
                    return false;
                auto const [it, inserted] = result.try_emplace(x, to);
                return inserted or ast::is_alpha_equivalent(to, it->second).has_value();
            },
            [] (expr_t::global_t const& x, expr_t::global_t const& y)
            {
                // TODO should probably check that the two globals refer to the same environment (needs a test)
                return x == y;
            },
            [&] (expr_t::app_t const& x, expr_t::app_t const& y)
            {
                if (x.args.size() != y.args.size())
                    return false;
                if (not unify(x.func.get(), y.func.get(), result))
                    return false;
                for (auto const i: std::views::iota(0ul, x.args.size()))
                    if (not unify(x.args[i], y.args[i], result))
                        return false;
                return true;
            },
            [] (expr_t::abs_t const&, expr_t::abs_t const&) { return false; }, // TODO
            [] (expr_t::pi_t const&, expr_t::pi_t const&) { return false; }, // TODO
            [] (expr_t::sigma_t const&, expr_t::sigma_t const&) { return false; }, // TODO
            [] (expr_t::ref_t, expr_t::ref_t) { return true; },
            [] (expr_t::scope_t, expr_t::scope_t) { return true; },
            [] (expr_t::addressof_t const& x, expr_t::addressof_t const& y) { return x.var == y.var; },
            [&] (expr_t::deref_t const& x, expr_t::deref_t const& y)
            {
                return unify(x.ref.get(), y.ref.get(), result);
            },
            [] (expr_t::scopeof_t const& x, expr_t::scopeof_t const& y) { return x.var == y.var; },
            [] (expr_t::array_t, expr_t::array_t) { return true; },
            [&] (expr_t::init_list_t const& x, expr_t::init_list_t const& y)
            {
                if (x.values.size() != y.values.size())
                    return false;
                for (auto const i: std::views::iota(0ul, x.values.size()))
                    if (not unify(x.values[i], y.values[i], result))
                        return false;
                return true;
            },
            [&] (expr_t::member_t const& x, expr_t::member_t const& y)
            {
                return x.field == y.field and unify(x.object.get(), y.object.get(), result);
            },
            [&] (expr_t::subscript_t const& x, expr_t::subscript_t const& y)
            {
                return unify(x.object.get(), y.object.get(), result) and unify(x.index.get(), y.index.get(), result);
            },
            [&] (expr_t::because_t const& x, expr_t::because_t const& y)
            {
                // the reason why two expressions are legal is irrelevant to
                // whether they can be unified and what the required substitution is,
                // so we only unify inside the values
                return unify(x.value.get(), y.value.get(), result);
            }),
        from.value, to.value);
}
} // namespace impl

std::optional<std::map<expr_t::var_t, expr_t>> unify(expr_t const& from, expr_t const& to)
{
    std::optional<std::map<expr_t::var_t, expr_t>> result;
    if (not impl::unify(from, to, result.emplace()))
        result.reset();
    return result;
}

} // namespace dep0::typecheck
