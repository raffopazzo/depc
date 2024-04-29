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

ctx_t::ctx_t(scope_map<expr_t::var_t, value_type> values) :
    m_values(std::move(values))
{ }

// const member functions

ctx_t ctx_t::extend() const
{
    return ctx_t(m_values.extend());
}

ctx_t ctx_t::rewrite(expr_t const& from, expr_t const& to) const
{
    auto result = extend();
    for (auto const& var: vars())
    {
        auto const val = (*this)[var];
        assert(val);
        if (auto new_sort = typecheck::rewrite(from, to, val->value.type))
        {
            auto const inserted =
                result.try_emplace(var, val->origin, var_decl_t{val->value.qty, std::move(std::get<expr_t>(*new_sort))});
            assert(inserted.has_value());
        }
    }
    return result;
}

std::set<expr_t::var_t> ctx_t::vars() const
{
    std::set<expr_t::var_t> result;
    for (auto x = std::optional{m_values}; x.has_value(); x = x->parent())
        std::ranges::copy(std::views::keys(*x), std::inserter(result, result.end()));
    return result;
}

ctx_t::value_type const* ctx_t::operator[](expr_t::var_t const& name) const
{
    return m_values[name];
}

// non-const member functions

void ctx_t::add_unnamed(expr_t type)
{
    add_unnamed(var_decl_t{ast::qty_t::zero, std::move(type)});
}

void ctx_t::add_unnamed(var_decl_t decl)
{
    static std::size_t next_id = 1ul;
    static const source_text empty = source_text(make_null_handle(), "auto");
    // an unnamed variable can only be used via proof-search, so it better have zero quantity
    decl.qty = ast::qty_t::zero;
    do
        if (m_values.try_emplace(expr_t::var_t{empty, next_id++}, std::nullopt, std::move(decl)).second)
            return; // should always be true but doesn't harm to try the next id
    while (true);
}

dep0::expected<std::true_type>
ctx_t::try_emplace(std::optional<expr_t::var_t> name, std::optional<source_loc_t> const loc, var_decl_t decl)
{
    if (not name)
    {
        add_unnamed(std::move(decl));
        return std::true_type{};
    }
    auto const [it, inserted] = m_values.try_emplace(std::move(*name), loc, std::move(decl));
    if (inserted)
        return std::true_type{};
    else
    {
        auto const& prev = it->second;
        std::ostringstream err;
        pretty_print<properties_t>(err << "cannot redefine `", *name) << '`';
        pretty_print(err << ", previously defined as `", prev.value.type) << '`';
        if (prev.origin)
            err << " at " << prev.origin->line << ':' << prev.origin->col;
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

static std::vector<expr_t::var_t> topologically_ordered_vars(ctx_t const& ctx);

std::ostream& pretty_print(std::ostream& os, ctx_t const& ctx)
{
    auto const length_of = [] (expr_t::var_t const& var)
    {
        // length of the name, plus:
        //   1. length of its quantity ("0 ", "1 ", "  ")
        //   2. and possibly the length of the index with the colon separator
        return var.name.size() + 2 + (var.idx == 0ul ? 0ul : 2ul + static_cast<std::size_t>(std::log10(var.idx)));
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
            os << (val->value.qty == ast::qty_t::zero ? "0 " : val->value.qty == ast::qty_t::one ? "1 " : "  ");
            pretty_print<properties_t>(os, var);
            padding.resize(alignment - length_of(var), ' ');
            os << padding << ": ";
            expr_t const& type = val->value.type;
            auto copy = type;
            bool const changed = beta_delta_normalize(env_t{}, ctx, copy);
            pretty_print(os, changed and ast::size(copy) < ast::size(type) ? copy : type, indent);
        });
    return os;
}

std::vector<expr_t::var_t> topologically_ordered_vars(ctx_t const& ctx)
{
    std::vector<expr_t::var_t> result;
    std::ranges::copy(ctx.vars(), std::back_inserter(result));
    std::stable_sort( // if two vars are equivalent, keep them in alphabetical order
        result.begin(), result.end(),
        [&] (expr_t::var_t const& var_x, expr_t::var_t const& var_y)
        {
            expr_t const& type_x = ctx[var_x]->value.type;
            expr_t const& type_y = ctx[var_y]->value.type;
            return ast::occurs_in(var_x, type_y, ast::occurrence_style::free)
                and not ast::occurs_in(var_y, type_x, ast::occurrence_style::free);
        });
    return result;
}

} // namespace dep0::typecheck

