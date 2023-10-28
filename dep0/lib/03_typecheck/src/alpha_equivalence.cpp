#include "dep0/typecheck/alpha_equivalence.hpp"
#include "dep0/typecheck/rename.hpp"
#include "dep0/typecheck/replace.hpp"

#include "dep0/ast/pretty_print.hpp"

#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <ranges>
#include <sstream>

namespace dep0::typecheck {

static std::string_view ordinal_suffix(std::size_t i);

// Internally we use an implementation that modifies a copy of the original arguments when renaming is necessary.
static dep0::expected<std::true_type> is_alpha_equivalent_impl(type_t&, type_t&);

struct alpha_equivalence_visitor
{
    template <typename T, typename U>
    requires (not std::is_same_v<T, U>)
    dep0::expected<std::true_type> operator()(T const& x, U const& y) const
    {
        std::ostringstream err;
        pretty_print<properties_t>(err << '`', x) << '`';
        pretty_print<properties_t>(err << "is not alpha-equivalent to `", y) << '`';
        return dep0::error_t(err.str());
    }

    dep0::expected<std::true_type> operator()(type_t::bool_t const&, type_t::bool_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::unit_t const&, type_t::unit_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::i8_t const&, type_t::i8_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::i16_t const&, type_t::i16_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::i32_t const&, type_t::i32_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::i64_t const&, type_t::i64_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::u8_t const&, type_t::u8_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::u16_t const&, type_t::u16_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::u32_t const&, type_t::u32_t const&) const { return {}; }
    dep0::expected<std::true_type> operator()(type_t::u64_t const&, type_t::u64_t const&) const { return {}; }

    dep0::expected<std::true_type> operator()(type_t::var_t const& x, type_t::var_t const& y) const
    {
        if (x == y)
            return std::true_type{};
        else
        {
            std::ostringstream err;
            pretty_print<properties_t>(err << '`', x) << '`';
            pretty_print<properties_t>(err << " is not alpha-equivalent to `", y) << '`';
            return dep0::error_t(err.str());
        }
    }

    dep0::expected<std::true_type> operator()(type_t::arr_t& x, type_t::arr_t& y) const
    {
        if (x.args.size() != y.args.size())
        {
            std::ostringstream err;
            err << "a function with " << x.args.size() << " arguments is not alpha-equivalent to ";
            err << "a function with " << y.args.size();
            return dep0::error_t(err.str());
        }
        auto const not_alpha_equivalent = [&] (std::size_t const i)
        {
            auto const print_ordinal = [] (std::ostream& os, std::size_t const i) -> std::ostream&
            {
                return os << i << ordinal_suffix(i);
            };
            std::ostringstream err;
            pretty_print(print_ordinal(err, i+1) << " argument `", x.args[i]) << '`';
            pretty_print(err << " is not alpha-equivalent to `", y.args[i]) << '`';
            return dep0::error_t(err.str());
        };
        for (auto const i: std::views::iota(0ul, x.args.size()))
        {
            auto const ok =
                match(
                    x.args[i].value,
                    [&] (func_arg_t::type_arg_t& x_arg) -> dep0::expected<std::true_type>
                    {
                        auto* const y_arg = std::get_if<func_arg_t::type_arg_t>(&y.args[i].value);
                        if (not y_arg)
                            return not_alpha_equivalent(i);
                        if (x_arg.var.has_value() xor y_arg->var.has_value())
                        {
                            // TODO: x and y could be alpha-equivalent if the named argument is never used
                            std::ostringstream err;
                            err << "alpha-equivalence between named and anonymous type-binders is not yet supported";
                            auto not_ok = not_alpha_equivalent(i);
                            not_ok.reasons.push_back(dep0::error_t(err.str()));
                            return not_ok;
                        }
                        if (not x_arg.var and not y_arg->var)
                            return std::true_type{};
                        auto& x_var = *x_arg.var;
                        auto& y_var = *y_arg->var;
                        if (x_var == y_var)
                            return std::true_type{};
                        x_var = rename(x_var, x.args.begin() + i + 1, x.args.end(), x.ret_type.get());
                        y_var = rename(y_var, y.args.begin() + i + 1, y.args.end(), y.ret_type.get());
                        if (x_var == y_var)
                            // by pure luck renaming assigned the same name to both
                            return std::true_type{};
                        // renaming assigns the next unused index number counting upwards;
                        // so if index of x_var is greather than that of y_var, we know that `x_var` does not occur
                        // in the renamed y, and viceversa; we can therefore safely replace x_var in y (or viceversa);
                        // if x and y are alpha-equivalent, they will now compare equal in all other args and ret type
                        if (x_var.name.idx > y_var.name.idx)
                            replace(y_var, x_var, y.args.begin() + i + 1, y.args.end(), y.ret_type.get());
                        else
                            replace(x_var, y_var, x.args.begin() + i + 1, x.args.end(), x.ret_type.get());
                        return std::true_type{};
                    },
                    [&] (func_arg_t::term_arg_t& x_arg) -> dep0::expected<std::true_type>
                    {
                        auto* const y_arg = std::get_if<func_arg_t::term_arg_t>(&y.args[i].value);
                        if (not y_arg)
                            return not_alpha_equivalent(i);
                        auto ok = is_alpha_equivalent_impl(x_arg.type, y_arg->type);
                        if (not ok)
                        {
                            auto not_ok = not_alpha_equivalent(i);
                            not_ok.reasons.push_back(std::move(ok.error()));
                            return not_ok;
                        }
                        return ok;
                    });
            if (not ok)
                return std::move(ok.error());
        }
        auto const ok = is_alpha_equivalent_impl(x.ret_type.get(), y.ret_type.get());
        if (ok)
            return ok;
        else
        {
            std::ostringstream err;
            pretty_print(err << "return type `", x.ret_type.get()) << '`';
            pretty_print(err << " is not alpha-equivalent to `", y.ret_type.get()) << '`';
            return dep0::error_t(err.str(), {std::move(ok.error())});
        }
    }
};

dep0::expected<std::true_type> is_alpha_equivalent_impl(type_t& x, type_t& y)
{
    return std::visit(alpha_equivalence_visitor{}, x.value, y.value);
}

dep0::expected<std::true_type> is_alpha_equivalent(type_t const& x, type_t const& y)
{
    // TODO we should make a lazy copy somehow, because renaming might not be often unnecessary
    auto x2 = x;
    auto y2 = y;
    return is_alpha_equivalent_impl(x2, y2);
}

std::string_view ordinal_suffix(std::size_t i)
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
