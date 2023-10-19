#include "dep0/typecheck/alpha_equivalence.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

using ctx_t = scope_map<type_t::var_t, type_t::var_t>;

static dep0::expected<std::true_type> is_alpha_equivalent(ctx_t const&, ctx_t const&, type_t const&, type_t const&);
static std::string_view ordinal(std::size_t i);

dep0::expected<std::true_type> is_alpha_equivalent(type_t const& x, type_t const& y)
{
    ctx_t ctx1, ctx2;
    return is_alpha_equivalent(ctx_t{}, ctx_t{}, x, y);
}

struct alpha_equivalence_visitor
{
    ctx_t const& ctx1;
    ctx_t const& ctx2;

    template <typename T, typename U>
    requires (not std::is_same_v<T, U>)
    dep0::expected<std::true_type> operator()(T const& x, U const& y) const
    {
        std::ostringstream err;
        pretty_print<typecheck::properties_t>(err << '`', x) << '`';
        pretty_print<typecheck::properties_t>(err << "is not alpha-equivalent to `", y) << '`';
        return dep0::error_t(err.str());
    }

    template <typename T>
    dep0::expected<std::true_type> operator()(T const&, T const&) const
    {
        return std::true_type{};
    }

    dep0::expected<std::true_type> operator()(type_t::var_t const& x, type_t::var_t const& y) const
    {
        auto const var_x = ctx1[x];
        auto const var_y = ctx2[y];
        if (var_x and var_y)
        {
            if (*var_x == y and *var_y == x)
                return std::true_type{};
            else
            {
                std::ostringstream err;
                pretty_print<typecheck::properties_t>(err << "in the current context `", x) << '`';
                pretty_print<typecheck::properties_t>(err << " means `", *var_x) << '`';
                pretty_print<typecheck::properties_t>(err << " but `", y) << '`';
                pretty_print<typecheck::properties_t>(err << " means `", *var_y) << '`';
                return dep0::error_t(err.str());
            }
        }
        if (not var_x and not var_y)
        {
            if (x == y)
                return std::true_type{};
            else
            {
                std::ostringstream err;
                pretty_print<typecheck::properties_t>(err << '`', x) << '`';
                pretty_print<typecheck::properties_t>(err << " is not alpha-equivalent to `", y) << '`';
                return dep0::error_t(err.str());
            }
        }
        std::ostringstream err;
        pretty_print<typecheck::properties_t>(err << "in the current context `", x) << '`';
        pretty_print<typecheck::properties_t>(err << " has a different meaning from `", y) << '`';
        return dep0::error_t(err.str());
    }

    dep0::expected<std::true_type> operator()(type_t::arr_t const& x, type_t::arr_t const& y) const
    {
        if (x.arg_kinds.size() != y.arg_kinds.size())
        {
            std::ostringstream err;
            err << "a function with " << x.arg_kinds.size() << " arguments is not alpha-equivalent to ";
            err << "a function with " << y.arg_kinds.size();
            return dep0::error_t(err.str());
        }
        auto const not_alpha_equivalent = [&] (std::size_t const i)
        {
            auto const print =
                [](std::ostream& os, type_t::arr_t::arg_kind_t const& kind) -> std::ostream&
                {
                    match(
                        kind,
                        [&] (type_t::var_t const& var) { pretty_print<typecheck::properties_t>(os << "typename ", var); },
                        [&] (type_t const& type) { pretty_print(os, type); });
                    return os;
                };
            auto const print_ordinal = [] (std::ostream& os, std::size_t const i) -> std::ostream&
            {
                return os << i << ordinal(i);
            };
            std::ostringstream err;
            print(print_ordinal(err, i+1) << " argument of type `", x.arg_kinds[i]) << '`';
            print(err << " is not alpha-equivalent to argument of type `", y.arg_kinds[i]) << '`';
            return dep0::error_t(err.str());
        };
        auto ctx1 = this->ctx1;
        auto ctx2 = this->ctx2;
        for (auto const i: std::views::iota(0ul, x.arg_kinds.size()))
        {
            auto const ok =
                match(
                    x.arg_kinds[i],
                    [&] (type_t::var_t const& x_var) -> dep0::expected<std::true_type>
                    {
                        auto const y_var = std::get_if<type_t::var_t>(&y.arg_kinds[i]);
                        if (not y_var)
                            return not_alpha_equivalent(i);
                        bool const inserted1 = ctx1.try_emplace(x_var, *y_var).second;
                        bool const inserted2 = ctx2.try_emplace(*y_var, x_var).second;
                        if (inserted1 and inserted2)
                            return std::true_type{};
                        else
                            return not_alpha_equivalent(i);
                    },
                    [&] (type_t const& x_type) -> dep0::expected<std::true_type>
                    {
                        auto const y_type = std::get_if<type_t>(&y.arg_kinds[i]);
                        if (not y_type)
                            return not_alpha_equivalent(i);
                        auto ok = is_alpha_equivalent(ctx1, ctx2, x_type, *y_type);
                        if (not ok)
                        {
                            auto not_ok = not_alpha_equivalent(i);
                            not_ok.reasons.push_back(ok.error());
                            return not_ok;
                        }
                        return ok;
                    });
            if (not ok)
                return ok.error();
        }
        auto const ok = is_alpha_equivalent(ctx1, ctx2, x.ret_type.get(), y.ret_type.get());
        if (ok)
            return ok;
        else
        {
            std::ostringstream err;
            pretty_print(err << "return type `", x.ret_type.get()) << '`';
            pretty_print(err << " is not alpha-equivalent to return type `", y.ret_type.get()) << '`';
            return dep0::error_t(err.str(), {std::move(ok.error())});
        }
    }
};

dep0::expected<std::true_type> is_alpha_equivalent(
    ctx_t const& ctx1,
    ctx_t const& ctx2,
    type_t const& x,
    type_t const& y)
{
    return std::visit(alpha_equivalence_visitor{ctx1, ctx2}, x.value, y.value);
}

std::string_view ordinal(std::size_t i)
{
    switch (i) { case 11: case 12: case 13: return "th"; }
    switch (i % 10)
    {
    case 1: return "st";
    case 2: return "nd";
    case 3: return "th";
    }
    return "th";
}

} // namespace dep0::typecheck
