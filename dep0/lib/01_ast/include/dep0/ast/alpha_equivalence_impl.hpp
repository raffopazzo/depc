/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Imlpementation details of @ref alpha_equivalence.hpp
 */
#pragma once

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/occurs_in.hpp"
#include "dep0/ast/pretty_print.hpp"
#include "dep0/ast/replace.hpp"
#include "dep0/ast/rename.hpp"

#include "dep0/match.hpp"

#include <boost/hana.hpp>

#include <sstream>

namespace dep0::ast {

namespace impl {

// Internally we use an implementation that modifies a copy of the original arguments when renaming is necessary.
template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(expr_t<P>&, expr_t<P>&);

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(typename expr_t<P>::app_t&, typename expr_t<P>::app_t&);

/**
 * @brief Check whether two Pi-Types, Sigma-Types or Lambda-Abstractions are alpha-equivalent.
 *
 * @param x_ret_type,y_ret_type     Pass `nullptr` to check two Sigma-Types.
 * @param x_body,y_body             Pass `nullptr` to check two Pi-Types or two Sigma-Types.
 */
template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(
    is_mutable_t x_mutable, std::vector<func_arg_t<P>>& x_args, expr_t<P>* x_ret_type, body_t<P>* x_body,
    is_mutable_t y_mutable, std::vector<func_arg_t<P>>& y_args, expr_t<P>* y_ret_type, body_t<P>* y_body);

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(body_t<P>&, body_t<P>&);

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(stmt_t<P>&, stmt_t<P>&);

/**
 * @brief Visit two expressions to test whether they are alpha-equivalent or not.
 * @see alpha_equivalence
 */
template <Properties P>
struct alpha_equivalence_visitor
{
    /** @brief All functions succeed if the two expressions are equivalent, or fail with the reason why they are not. */
    using result_t = dep0::expected<std::true_type>;

    template <typename T, typename U>
    static result_t not_alpha_equivalent(T const& x, U const& y)
    {
        std::ostringstream err;
        pretty_print<P>(err << '`', x) << "` is not alpha-equivalent to ";
        pretty_print<P>(err << '`', y) << '`';
        return dep0::error_t(err.str());
    }

    template <typename T, typename U>
    requires (not std::is_same_v<T, U>)
    result_t operator()(T const& x, U const& y) const
    {
        return not_alpha_equivalent(x, y);
    }

    result_t operator()(typename expr_t<P>::typename_t, typename expr_t<P>::typename_t) const { return {}; }
    result_t operator()(typename expr_t<P>::true_t, typename expr_t<P>::true_t) const { return {}; }
    result_t operator()(typename expr_t<P>::auto_t, typename expr_t<P>::auto_t) const { return {}; }
    result_t operator()(typename expr_t<P>::ref_t, typename expr_t<P>::ref_t) const { return {}; }
    result_t operator()(typename expr_t<P>::scope_t, typename expr_t<P>::scope_t) const { return {}; }
    result_t operator()(typename expr_t<P>::addressof_t const& x, typename expr_t<P>::addressof_t const& y) const
    {
        if (x.var == y.var)
            return {};
        else
            return not_alpha_equivalent(x, y);
    }
    result_t operator()(typename expr_t<P>::deref_t& x, typename expr_t<P>::deref_t& y) const
    {
        return is_alpha_equivalent_impl(x.ref.get(), y.ref.get());
    }
    result_t operator()(typename expr_t<P>::scopeof_t, typename expr_t<P>::scopeof_t) const { return {}; }
    result_t operator()(typename expr_t<P>::bool_t, typename expr_t<P>::bool_t) const { return {}; }
    result_t operator()(typename expr_t<P>::cstr_t, typename expr_t<P>::cstr_t) const { return {}; }
    result_t operator()(typename expr_t<P>::unit_t, typename expr_t<P>::unit_t) const { return {}; }
    result_t operator()(typename expr_t<P>::i8_t, typename expr_t<P>::i8_t) const { return {}; }
    result_t operator()(typename expr_t<P>::i16_t, typename expr_t<P>::i16_t) const { return {}; }
    result_t operator()(typename expr_t<P>::i32_t, typename expr_t<P>::i32_t) const { return {}; }
    result_t operator()(typename expr_t<P>::i64_t, typename expr_t<P>::i64_t) const { return {}; }
    result_t operator()(typename expr_t<P>::u8_t, typename expr_t<P>::u8_t) const { return {}; }
    result_t operator()(typename expr_t<P>::u16_t, typename expr_t<P>::u16_t) const { return {}; }
    result_t operator()(typename expr_t<P>::u32_t, typename expr_t<P>::u32_t) const { return {}; }
    result_t operator()(typename expr_t<P>::u64_t, typename expr_t<P>::u64_t) const { return {}; }

    result_t operator()(
        typename expr_t<P>::boolean_constant_t const& x,
        typename expr_t<P>::boolean_constant_t const& y) const
    {
        if (x.value == y.value)
            return {};
        else
            return not_alpha_equivalent(x, y);
    }

    result_t operator()(
        typename expr_t<P>::numeric_constant_t const& x,
        typename expr_t<P>::numeric_constant_t const& y) const
    {
        if (x.value == y.value)
            return {};
        else
            return not_alpha_equivalent(x, y);
    }

    result_t operator()(
        typename expr_t<P>::string_literal_t const& x,
        typename expr_t<P>::string_literal_t const& y) const
    {
        if (x.value == y.value)
            return {};
        else
            return not_alpha_equivalent(x, y);
    }

    result_t operator()(typename expr_t<P>::boolean_expr_t& x, typename expr_t<P>::boolean_expr_t& y) const
    {
        return std::visit(
            boost::hana::overload(
                [] (expr_t<P>::boolean_expr_t::not_t& x, expr_t<P>::boolean_expr_t::not_t& y)
                {
                    return is_alpha_equivalent_impl(x.expr.get(), y.expr.get());
                },
                [] <typename T> (T& x, T& y)
                {
                    auto eq = is_alpha_equivalent_impl(x.lhs.get(), y.lhs.get());
                    if (eq)
                        eq = is_alpha_equivalent_impl(x.rhs.get(), y.rhs.get());
                    return eq;
                },
                [&] <typename T, typename U> (T const&, U const&) requires (not std::is_same_v<T, U>)
                {
                    return not_alpha_equivalent(x, y);
                }),
            x.value, y.value);
    };

    result_t operator()(typename expr_t<P>::relation_expr_t& x, typename expr_t<P>::relation_expr_t& y) const
    {
        return std::visit(
            boost::hana::overload(
                [] <typename T> (T& x, T& y)
                {
                    auto eq = is_alpha_equivalent_impl(x.lhs.get(), y.lhs.get());
                    if (eq)
                        eq = is_alpha_equivalent_impl(x.rhs.get(), y.rhs.get());
                    return eq;
                },
                [&] <typename T, typename U> (T const&, U const&) requires (not std::is_same_v<T, U>)
                {
                    return not_alpha_equivalent(x, y);
                }),
            x.value, y.value);
    };

    result_t operator()(typename expr_t<P>::arith_expr_t& x, typename expr_t<P>::arith_expr_t& y) const
    {
        return std::visit(
            boost::hana::overload(
                [] <typename T> (T& x, T& y)
                {
                    auto eq = is_alpha_equivalent_impl(x.lhs.get(), y.lhs.get());
                    if (eq)
                        eq = is_alpha_equivalent_impl(x.rhs.get(), y.rhs.get());
                    return eq;
                },
                [&] <typename T, typename U> (T const&, U const&) requires (not std::is_same_v<T, U>)
                {
                    return not_alpha_equivalent(x, y);
                }),
            x.value, y.value);
    };

    result_t operator()(typename expr_t<P>::var_t const& x, typename expr_t<P>::var_t const& y) const
    {
        if (x == y)
            return std::true_type{};
        else
            return not_alpha_equivalent(x, y);
    }

    result_t operator()(typename expr_t<P>::global_t const& x, typename expr_t<P>::global_t const& y) const
    {
        if (x == y)
            return std::true_type{};
        else
            return not_alpha_equivalent(x, y);
    }

    result_t operator()(typename expr_t<P>::app_t& x, typename expr_t<P>::app_t& y) const
    {
        return is_alpha_equivalent_impl<P>(x, y);
    }

    result_t operator()(typename expr_t<P>::abs_t& x, typename expr_t<P>::abs_t& y) const
    {
        return is_alpha_equivalent_impl(
            x.is_mutable, x.args, &x.ret_type.get(), &x.body,
            y.is_mutable, y.args, &y.ret_type.get(), &y.body);
    }

    result_t operator()(typename expr_t<P>::pi_t& x, typename expr_t<P>::pi_t& y) const
    {
        return is_alpha_equivalent_impl<P>(
            x.is_mutable, x.args, &x.ret_type.get(), nullptr,
            y.is_mutable, y.args, &y.ret_type.get(), nullptr);
    }

    result_t operator()(typename expr_t<P>::sigma_t& x, typename expr_t<P>::sigma_t& y) const
    {
        return is_alpha_equivalent_impl<P>(
            ast::is_mutable_t::no, x.args, nullptr, nullptr,
            ast::is_mutable_t::no, y.args, nullptr, nullptr);
    }

    result_t operator()(typename expr_t<P>::array_t&, typename expr_t<P>::array_t&) const
    {
        return std::true_type{};
    }

    result_t operator()(typename expr_t<P>::init_list_t& x, typename expr_t<P>::init_list_t& y) const
    {
        if (x.values.size() != y.values.size())
        {
            std::ostringstream err;
            err << "initializer list with " << x.values.size() << " values is not alpha-equivalent to ";
            err << "initializer list with " << y.values.size();
            return dep0::error_t(err.str());
        }
        for (auto const i: std::views::iota(0ul, x.values.size()))
            if (auto eq = is_alpha_equivalent_impl(x.values[i], y.values[i]); not eq)
                return eq;
        return {};
    }

    result_t operator()(typename expr_t<P>::member_t& x, typename expr_t<P>::member_t& y) const
    {
        auto eq = is_alpha_equivalent_impl(x.object.get(), y.object.get());
        if (eq and x.field != y.field)
        {
            std::ostringstream err;
            err << "member `" << x.field << "` is not alpha-equivalent to ";
            err << "member `" << y.field << '`';
            eq = dep0::error_t(err.str());
        }
        return eq;
    }

    result_t operator()(typename expr_t<P>::subscript_t& x, typename expr_t<P>::subscript_t& y) const
    {
        auto eq = is_alpha_equivalent_impl(x.object.get(), y.object.get());
        if (eq)
            eq = is_alpha_equivalent_impl(x.index.get(), y.index.get());
        return eq;
    }

    result_t operator()(typename expr_t<P>::because_t& x, typename expr_t<P>::because_t& y) const
    {
        return is_alpha_equivalent_impl(x.value.get(), y.value.get());
    }

    result_t operator()(typename stmt_t<P>::if_else_t& x, typename stmt_t<P>::if_else_t& y) const
    {
        auto eq = is_alpha_equivalent_impl(x.cond, y.cond);
        if (eq)
            eq = is_alpha_equivalent_impl(x.true_branch, y.true_branch);
        if (eq)
        {
            if (x.false_branch.has_value() xor y.false_branch.has_value())
                return dep0::error_t("if-statement with an else branch is not alpha-equivalent to one without");
            if (x.false_branch)
                eq = is_alpha_equivalent_impl(*x.false_branch, *y.false_branch);
        }
        return eq;
    }

    result_t operator()(typename stmt_t<P>::return_t& x, typename stmt_t<P>::return_t& y) const
    {
        if (x.expr.has_value() xor y.expr.has_value())
            return dep0::error_t("return statement with expression is not alpha-equivalent to one without");
        if (x.expr)
            return is_alpha_equivalent_impl(*x.expr, *y.expr);
        return {};
    }

    result_t operator()(typename stmt_t<P>::impossible_t&, typename stmt_t<P>::impossible_t&) const
    {
        // whether an explicit proof of false is supplied or not is irrelevant,
        // both forms are alpha-equivalent to each other
        return {};
    }
};

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(expr_t<P>& x, expr_t<P>& y)
{
    auto const because_x = std::get_if<typename expr_t<P>::because_t>(&x.value);
    auto const because_y = std::get_if<typename expr_t<P>::because_t>(&y.value);
    if (because_x and because_y)
        return is_alpha_equivalent_impl(because_x->value.get(), because_y->value.get());
    else if (because_x or because_y)
        return because_x
            ? is_alpha_equivalent_impl(because_x->value.get(), y)
            : is_alpha_equivalent_impl(x, because_y->value.get());
    else
        return std::visit(alpha_equivalence_visitor<P>{}, x.value, y.value);
}

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(typename expr_t<P>::app_t& x, typename expr_t<P>::app_t& y)
{
    if (x.args.size() != y.args.size())
    {
        std::ostringstream err;
        err << "application with " << x.args.size() << " arguments is not alpha-equivalent to ";
        err << "application with " << y.args.size();
        return dep0::error_t(err.str());
    }
    if (auto eq = is_alpha_equivalent_impl(x.func.get(), y.func.get()); not eq)
        return eq;
    for (auto const i: std::views::iota(0ul, x.args.size()))
        if (auto eq = is_alpha_equivalent_impl(x.args[i], y.args[i]); not eq)
            return eq;
    return {};
}

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(
    is_mutable_t const x_mutable, std::vector<func_arg_t<P>>& x_args, expr_t<P>* x_ret_type, body_t<P>* x_body,
    is_mutable_t const y_mutable, std::vector<func_arg_t<P>>& y_args, expr_t<P>* y_ret_type, body_t<P>* y_body)
{
    if (x_mutable != y_mutable)
        return dep0::error_t(
            x_mutable == is_mutable_t::yes
            ? "a mutable function is not alpha-equivalent to an immutable one"
            : "an immutable function is not alpha-equivalent to a mutable one");
    if (x_args.size() != y_args.size())
    {
        std::ostringstream err;
        err << "a function with " << x_args.size() << " arguments is not alpha-equivalent to ";
        err << "a function with " << y_args.size();
        return dep0::error_t(err.str());
    }
    auto const not_alpha_equivalent = [&] (std::size_t const i)
    {
        auto const print_ordinal = [] (std::ostream& os, std::size_t const i) -> std::ostream&
        {
            return os << i << [&]
            {
                switch (i) { case 11: case 12: case 13: return "th"; }
                switch (i % 10)
                {
                case 1: return "st";
                case 2: return "nd";
                case 3: return "rd";
                }
                return "th";
            }();
        };
        std::ostringstream err;
        pretty_print(print_ordinal(err, i+1) << " argument `", x_args[i]) << '`';
        pretty_print(err << " is not alpha-equivalent to `", y_args[i]) << '`';
        return dep0::error_t(err.str());
    };
    for (auto const i: std::views::iota(0ul, x_args.size()))
    {
        auto& x_arg = x_args[i];
        auto& y_arg = y_args[i];
        if (auto eq = is_alpha_equivalent_impl(x_arg.type, y_arg.type); not eq)
        {
            auto err = not_alpha_equivalent(i);
            err.reasons.push_back(std::move(eq.error()));
            return err;
        }
        if (x_arg.var and y_arg.var)
        {
            auto& x_var = *x_arg.var;
            auto& y_var = *y_arg.var;
            if (x_var == y_var)
                continue;
            x_var = x_ret_type
                ? rename(x_var, x_args.begin() + i + 1, x_args.end(), *x_ret_type, x_body)
                : rename<P>(x_var, x_args.begin() + i + 1, x_args.end());
            y_var = y_ret_type
                ? rename(y_var, y_args.begin() + i + 1, y_args.end(), *y_ret_type, y_body)
                : rename<P>(y_var, y_args.begin() + i + 1, y_args.end());

            if (x_var == y_var)
                // by pure luck renaming assigned the same name to both
                continue;
            // renaming assigns the next unused index number counting upwards;
            // so if index of x_var is greater than that of y_var, we know that `x_var` does not occur
            // in the renamed y, and viceversa; we can therefore safely replace x_var in y (or viceversa);
            if (x_var.idx > y_var.idx)
            {
                if (y_ret_type)
                    replace(y_var, x_var, y_args.begin() + i + 1, y_args.end(), *y_ret_type, y_body);
                else
                    replace<P>(y_var, x_var, y_args.begin() + i + 1, y_args.end());
                y_var = x_var; // just in case; otherwise error messages might look odd and even confusing
            }
            else
            {
                if (x_ret_type)
                    replace(x_var, y_var, x_args.begin() + i + 1, x_args.end(), *x_ret_type, x_body);
                else
                    replace<P>(x_var, y_var, x_args.begin() + i + 1, x_args.end());
                x_var = y_var; // ditto
            }
        }
        else if (x_arg.var.has_value() xor y_arg.var.has_value())
        {
            auto const occurs_free = [] <typename... Args> (Args&&...args)
            {
                return occurs_in<P>(std::forward<Args>(args)..., occurrence_style::free);
            };
            // One function argument is named but the other is anonymous.
            // The two functions might still be alpha-equivalent if the named argument
            // does not occur free inside the rest of the signature or in the body.
            auto const occurs_somewhere = [&]
            {
                std::ostringstream err;
                pretty_print<P>(err << '`', x_arg.var ? *x_arg.var : *y_arg.var) << '`';
                err << " occurs free somewhere";
                return dep0::error_t(err.str());
            };
            if (x_arg.var)
            {
                if (x_ret_type)
                {
                    if (occurs_free(*x_arg.var, x_args.begin() + i + 1, x_args.end(), *x_ret_type, x_body))
                        return occurs_somewhere();
                }
                else if (occurs_free(*x_arg.var, x_args.begin() + i + 1, x_args.end()))
                    return occurs_somewhere();
            }
            else
            {
                if (y_ret_type)
                {
                    if (occurs_free(*y_arg.var, y_args.begin() + i + 1, y_args.end(), *y_ret_type, y_body))
                        return occurs_somewhere();
                }
                else if (occurs_free(*y_arg.var, y_args.begin() + i + 1, y_args.end()))
                    return occurs_somewhere();
            }
        }
    }
    if (x_ret_type and y_ret_type)
        if (auto eq = is_alpha_equivalent_impl(*x_ret_type, *y_ret_type); not eq)
        {
            std::ostringstream err;
            pretty_print(err << "return type `", *x_ret_type) << '`';
            pretty_print(err << " is not alpha-equivalent to `", *y_ret_type) << '`';
            return dep0::error_t(err.str(), {std::move(eq.error())});
        }
    return x_body and y_body ? is_alpha_equivalent_impl(*x_body, *y_body) : dep0::expected<std::true_type>{};
}

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(body_t<P>& x, body_t<P>& y)
{
    if (x.stmts.size() != y.stmts.size())
    {
        std::ostringstream err;
        err << "a body with " << x.stmts.size() << " statements is not alpha-equivalent to ";
        err << "a body with " << y.stmts.size();
        return dep0::error_t(err.str());
    }
    for (auto const i: std::views::iota(0ul, x.stmts.size()))
        if (auto eq = is_alpha_equivalent_impl(x.stmts[i], y.stmts[i]); not eq)
            return eq;
    return {};
}

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(stmt_t<P>& x, stmt_t<P>& y)
{
    return std::visit(alpha_equivalence_visitor<P>{}, x.value, y.value);
}

} // namespace impl

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent(expr_t<P> const& x, expr_t<P> const& y)
{
    // TODO we should make a lazy copy somehow, because renaming might be often unnecessary.
    // Alternatively we could try something like this:
    // 1. make the whole implementation templated over const-ref or mutable-ref, eg via `bool mutable` flag;
    // 2. the immutable version will return an empty optional if renaming is necessary to establish equivalence;
    // 3. only at that point actually make the copy and try again, but the mutable-ref does not return an optional.
    auto x2 = x;
    auto y2 = y;
    return impl::is_alpha_equivalent_impl(x2, y2);
}

} // namespace dep0::ast
