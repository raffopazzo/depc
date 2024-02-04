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

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(
    std::vector<func_arg_t<P>>& x_args, expr_t<P>& x_ret_type, body_t<P>* x_body,
    std::vector<func_arg_t<P>>& y_args, expr_t<P>& y_ret_type, body_t<P>* y_body);

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(body_t<P>&, body_t<P>&);

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(stmt_t<P>&, stmt_t<P>&);

template <Properties P>
struct alpha_equivalence_visitor
{
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
    result_t operator()(typename expr_t<P>::bool_t, typename expr_t<P>::bool_t) const { return {}; }
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

    result_t operator()(typename expr_t<P>::boolean_expr_t& x, typename expr_t<P>::boolean_expr_t& y) const
    {
        struct visitor
        {
            result_t operator()(
                typename expr_t<P>::boolean_expr_t::negation_t& x,
                typename expr_t<P>::boolean_expr_t::negation_t& y) const
            {
                return is_alpha_equivalent_impl(x.expr.get(), y.expr.get());
            }
        };
        return std::visit(visitor{}, x.value, y.value);
    };

    result_t operator()(typename expr_t<P>::relation_expr_t& x, typename expr_t<P>::relation_expr_t& y) const
    {
        auto eq =
            x.relation == y.relation
                ? is_alpha_equivalent_impl(x.lhs.get(), y.lhs.get())
                : not_alpha_equivalent(x, y);
        if (eq)
            eq = is_alpha_equivalent_impl(x.rhs.get(), y.rhs.get());
        return eq;
    };

    result_t operator()(typename expr_t<P>::arith_expr_t& x, typename expr_t<P>::arith_expr_t& y) const
    {
        struct visitor
        {
            result_t operator()(
                typename expr_t<P>::arith_expr_t::plus_t& x,
                typename expr_t<P>::arith_expr_t::plus_t& y) const
            {
                auto eq = is_alpha_equivalent_impl(x.lhs.get(), y.lhs.get());
                if (eq)
                    eq = is_alpha_equivalent_impl(x.rhs.get(), y.rhs.get());
                return eq;
            }
        };
        return std::visit(visitor{}, x.value, y.value);
    };

    result_t operator()(typename expr_t<P>::var_t const& x, typename expr_t<P>::var_t const& y) const
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
        return is_alpha_equivalent_impl(x.args, x.ret_type.get(), &x.body, y.args, y.ret_type.get(), &y.body);
    }

    result_t operator()(typename expr_t<P>::pi_t& x, typename expr_t<P>::pi_t& y) const
    {
        return is_alpha_equivalent_impl<P>(x.args, x.ret_type.get(), nullptr, y.args, y.ret_type.get(), nullptr);
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

    result_t operator()(typename expr_t<P>::subscript_t& x, typename expr_t<P>::subscript_t& y) const
    {
        auto eq = is_alpha_equivalent_impl(x.array.get(), y.array.get());
        if (eq)
            eq = is_alpha_equivalent_impl(x.index.get(), y.index.get());
        return eq;
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
};

template <Properties P>
dep0::expected<std::true_type> is_alpha_equivalent_impl(expr_t<P>& x, expr_t<P>& y)
{
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
    std::vector<func_arg_t<P>>& x_args, expr_t<P>& x_ret_type, body_t<P>* x_body,
    std::vector<func_arg_t<P>>& y_args, expr_t<P>& y_ret_type, body_t<P>* y_body)
{
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
            x_var = rename(x_var, x_args.begin() + i + 1, x_args.end(), x_ret_type, x_body);
            y_var = rename(y_var, y_args.begin() + i + 1, y_args.end(), y_ret_type, y_body);
            if (x_var == y_var)
                // by pure luck renaming assigned the same name to both
                continue;
            // renaming assigns the next unused index number counting upwards;
            // so if index of x_var is greater than that of y_var, we know that `x_var` does not occur
            // in the renamed y, and viceversa; we can therefore safely replace x_var in y (or viceversa);
            if (x_var.idx > y_var.idx)
            {
                replace(y_var, x_var, y_args.begin() + i + 1, y_args.end(), y_ret_type, y_body);
                y_var = x_var; // just in case; otherwise error messages might look odd and even confusing
            }
            else
            {
                replace(x_var, y_var, x_args.begin() + i + 1, x_args.end(), x_ret_type, x_body);
                x_var = y_var; // ditto
            }
        }
        else if (x_arg.var.has_value() xor y_arg.var.has_value())
        {
            // One function argument is named but the other is anonymous.
            // The two functions might still be alpha-equivalent if the named argument
            // does not occur free inside the rest of the signature or in the body.
            auto const occurs_somewhere = [&]
            {
                auto err = not_alpha_equivalent(i);
                err.reasons.push_back([&]
                {
                    std::ostringstream err;
                    pretty_print<P>(err << '`', x_arg.var ? *x_arg.var : *y_arg.var) << '`';
                    err << " occurs free somewhere";
                    return dep0::error_t(err.str());
                }());
                return err;
            };
            if (x_arg.var)
            {
                if (occurs_in(*x_arg.var, x_args.begin() + i + 1, x_args.end(), x_ret_type, x_body, occurrence_style::free))
                    return occurs_somewhere();
            }
            else
            {
                if (occurs_in(*y_arg.var, y_args.begin() + i + 1, y_args.end(), y_ret_type, y_body, occurrence_style::free))
                    return occurs_somewhere();
            }
        }
    }
    if (auto eq = is_alpha_equivalent_impl(x_ret_type, y_ret_type); not eq)
    {
        std::ostringstream err;
        pretty_print(err << "return type `", x_ret_type) << '`';
        pretty_print(err << " is not alpha-equivalent to `", y_ret_type) << '`';
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
