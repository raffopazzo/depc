#include "private/usage.hpp"

namespace dep0::typecheck {

usage_t::usage_t(scope_map<expr_t::var_t, ast::qty_t> count)
    : count(std::move(count))
{
}

usage_t usage_t::extend() const
{
    return usage_t{count.extend()};
}

ast::qty_t usage_t::add(expr_t::var_t const& var, ast::qty_t const qty)
{
    ast::qty_t const* const old = count[var]; // the old value might come from the parent scope
    auto const [it, inserted] = count.try_emplace(var, qty);
    if (not inserted)
        it->second = it->second + qty;
    else if (old)
        it->second = *old + qty;
    return it->second;
}

ast::qty_t usage_t::operator[](expr_t::var_t const& var) const
{
    ast::qty_t const* const old = count[var];
    return old ? *old : ast::qty_t::zero;
}

} // namespace dep0::typecheck
