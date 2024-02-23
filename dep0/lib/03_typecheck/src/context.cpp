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

context_t::context_t(scope_map<expr_t::var_t, value_type> values) :
    m_values(std::move(values))
{ }

// const member functions

context_t context_t::extend() const
{
    return context_t(m_values.extend());
}

context_t context_t::rewrite(expr_t const& from, expr_t const& to) const
{
    auto result = extend();
    for (auto const& var: vars())
    {
        auto const val = (*this)[var];
        assert(val);
        expr_t const& expr = val->value;
        if (auto new_sort = typecheck::rewrite(from, to, expr.properties.sort.get()))
        {
            auto new_expr = expr;
            new_expr.properties.sort = std::move(*new_sort);
            auto const inserted = result.try_emplace(var, val->origin, std::move(new_expr));
            assert(inserted.has_value());
        }
    }
    return result;
}

std::set<expr_t::var_t> context_t::vars() const
{
    std::set<expr_t::var_t> result;
    for (auto x = std::optional{m_values}; x.has_value(); x = x->parent())
        std::ranges::copy(std::views::keys(*x), std::inserter(result, result.end()));
    return result;
}

context_t::value_type const* context_t::operator[](expr_t::var_t const& name) const
{
    return m_values[name];
}

// non-const member functions

dep0::expected<context_t::const_iterator>
context_t::try_emplace(expr_t::var_t name, source_loc_t const loc, expr_t v)
{
    auto const res = m_values.try_emplace(std::move(name), loc, std::move(v));
    if (res.second)
        return dep0::expected<const_iterator>(res.first);
    else
    {
        auto const& prev = res.first->second;
        std::ostringstream err;
        pretty_print<properties_t>(err << "cannot redefine `", name) << '`';
        err << ", previously defined at " << prev.origin.line << ':' << prev.origin.col;
        pretty_print(err << ", as `", prev.value.properties.sort.get()) << '`';
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

static std::vector<expr_t::var_t> topologically_ordered_vars(context_t const& ctx);

std::ostream& pretty_print(std::ostream& os, context_t const& ctx)
{
    auto const length_of = [] (expr_t::var_t const& var)
    {
        // length of the name, plus possibly the length of the index with the colon separator
        return var.name.size() + (var.idx == 0ul ? 0ul : 2ul + static_cast<std::size_t>(std::log10(var.idx)));
    };
    auto const& vars = ctx.vars();
    auto const longest =
        std::accumulate(
            vars.begin(), vars.end(),
            0ul,
            [&] (std::size_t const acc, expr_t::var_t const& var)
            {
                return std::max(acc, length_of(var));
            });
    auto const indent = (longest / 4ul) + 1ul;
    auto const alignment = indent * 4ul;
    for_each_line(
        os,
        topologically_ordered_vars(ctx),
        [&, padding=std::string()] (expr_t::var_t const& var) mutable
        {
            auto const val = ctx[var];
            assert(val);
            pretty_print<properties_t>(os, var);
            padding.resize(alignment - length_of(var), ' ');
            os << padding << ": ";
            match(
                val->value.properties.sort.get(),
                [&] (expr_t const& type)
                {
                    auto copy = type;
                    bool const changed = beta_delta_normalize(environment_t{}, ctx, copy);
                    pretty_print(os, changed and ast::size(copy) < ast::size(type) ? copy : type, indent);
                },
                [&] (kind_t) { pretty_print(os, kind_t{}, indent); });
        });
    return os;
}

std::ostream& pretty_print(std::ostream& os, context_t::value_type const& v)
{
    return pretty_print<properties_t>(os, v.value);
}

std::vector<expr_t::var_t> topologically_ordered_vars(context_t const& ctx)
{
    std::vector<expr_t::var_t> result;
    std::ranges::copy(ctx.vars(), std::back_inserter(result));
    std::stable_sort( // if two vars are equivalent, keep them in alphabetical order
        result.begin(), result.end(),
        [&] (expr_t::var_t const& var_x, expr_t::var_t const& var_y)
        {
            expr_t const& x = ctx[var_x]->value;
            expr_t const& y = ctx[var_y]->value;
            return match(
                x.properties.sort.get(),
                [&] (expr_t const& type_x)
                {
                    return match(
                        y.properties.sort.get(),
                        [&] (expr_t const& type_y)
                        {
                            return ast::occurs_in(var_x, type_y, ast::occurrence_style::free)
                                and not ast::occurs_in(var_y, type_x, ast::occurrence_style::free);
                        },
                        [] (kind_t) { return false; });
                },
                [&] (kind_t)
                {
                    return match(
                        y.properties.sort.get(),
                        [] (expr_t const&) { return true; },
                        [] (kind_t) { return false; });
                });
        });
    return result;
}

} // namespace dep0::typecheck

