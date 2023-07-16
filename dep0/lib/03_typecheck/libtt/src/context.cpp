#include "dep0/typecheck/tt/context.hpp"

#include <sstream>

namespace dep0::typecheck::tt {

context_t::state_t::state_t(std::shared_ptr<state_t const> parent) :
    parent(parent)
{ }

std::shared_ptr<context_t::state_t> context_t::state_t::inherit(std::shared_ptr<state_t const> parent)
{
    return std::make_shared<state_t>(parent);
}

context_t::context_t(std::shared_ptr<state_t const> parent) :
    state(std::make_shared<state_t>(parent))
{ }

context_t context_t::extend() const
{
    return context_t(state);
}

expected<std::true_type> context_t::add(term_t::var_t var, type_t ty)
{
    // we don't check if `var` is declared in `parent` to allow shadowing in a new scope
    auto const [it, inserted] = state->decls.try_emplace(std::move(var), std::move(ty));
    if (not inserted)
    {
        std::ostringstream err;
        pretty_print(err << "Conflicting declaration: `" << var.name << "` already declared as `", it->second) << "`";
        return error_t{err.str()};
    }
    return expected<std::true_type>{};
}

std::optional<type_t> context_t::operator[](term_t::var_t const var) const
{
    auto const *s = state.get();
    do
    {
        if (auto const it = s->decls.find(var); it != s->decls.end())
            return it->second;
        s = s->parent.get();
    }
    while (s);
    return std::nullopt;
}

std::ostream& pretty_print(std::ostream& os, context_t const& ctx)
{
    bool first = true;
    for (auto const& [var, ty]: ctx.state->decls)
        pretty_print((std::exchange(first, false) ? os : os << std::endl) << var.name << ": ", ty);
    return os;
}

} // namespace dep0::typecheck
