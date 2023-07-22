#include "dep0/typecheck/tt/context.hpp"

#include <sstream>

namespace dep0::typecheck::tt {

static std::ostream& pretty_print(std::ostream&, context_t::type_decl_t const&);
static std::ostream& pretty_print(std::ostream&, context_t::var_decl_t const&);

struct context_t::state_t
{
    std::shared_ptr<state_t const> parent;
    std::map<source_text, decl_t> decls; // type and term variables must have unique names; TODO use sorted vector?

    explicit state_t(std::shared_ptr<state_t const> p) :
        parent(std::move(p))
    { }

    static std::shared_ptr<state_t> inherit(std::shared_ptr<state_t const> parent)
    {
        return std::make_shared<state_t>(std::move(parent));
    }
};

context_t::context_t() :
    state(std::make_shared<state_t>(nullptr))
{ }

context_t::context_t(std::shared_ptr<state_t const> parent) :
    state(std::make_shared<state_t>(parent))
{ }

context_t context_t::extend() const
{
    return context_t(state);
}

expected<std::true_type> context_t::add(type_t::var_t var)
{
    // we don't check if `var` is declared in `parent` to allow shadowing in a new scope
    auto const [it, inserted] = state->decls.try_emplace(var.name, std::in_place_type<type_decl_t>, var);
    if (not inserted)
    {
        std::ostringstream err;
        std::visit(
            [&] (auto const& x)
            {
                pretty_print(err << "Conflicting declaration of `" << var.name << "` previously `", x) << '`';
            },
            it->second);
        return error_t{err.str()};
    }
    return expected<std::true_type>{};
}

expected<std::true_type> context_t::add(term_t::var_t var, type_t ty)
{
    // we don't check if `var` is declared in `parent` to allow shadowing in a new scope
    auto const [it, inserted] = state->decls.try_emplace(var.name, std::in_place_type<var_decl_t>, var, std::move(ty));
    if (not inserted)
    {
        std::ostringstream err;
        std::visit(
            [&] (auto const& x)
            {
                pretty_print(err << "Conflicting declaration of `" << var.name << "` previously `", x) << '`';
            },
            it->second);
        return error_t{err.str()};
    }
    return expected<std::true_type>{};
}

std::optional<context_t::type_decl_t> context_t::operator[](type_t::var_t const& var) const
{
    auto const *s = state.get();
    do
    {
        if (auto const it = s->decls.find(var.name); it != s->decls.end())
            // TODO should we return nullopt if it's a var declaration? but it would be inconsistent with shadowing...
            if (auto const* const p = std::get_if<type_decl_t>(&it->second))
                return *p;
        s = s->parent.get();
    }
    while (s);
    return std::nullopt;
}

std::optional<context_t::var_decl_t> context_t::operator[](term_t::var_t const& var) const
{
    auto const *s = state.get();
    do
    {
        if (auto const it = s->decls.find(var.name); it != s->decls.end())
            // TODO should we return nullopt if it's a type declaration? but it would be inconsistent with shadowing...
            if (auto const* const p = std::get_if<var_decl_t>(&it->second))
                return *p;
        s = s->parent.get();
    }
    while (s);
    return std::nullopt;
}

std::ostream& pretty_print(std::ostream& os, context_t const& ctx)
{
    bool first = true;
    for (auto const& [_, decl]: ctx.state->decls)
        std::visit(
            [&] (auto const& x) { pretty_print((std::exchange(first, false) ? os : os << std::endl), x); },
            decl);
    return os;
}

std::ostream& pretty_print(std::ostream& os, context_t::type_decl_t const& x)
{
    return os << x.subject.name << ": typename";
}

std::ostream& pretty_print(std::ostream& os, context_t::var_decl_t const& x)
{
    return pretty_print(os << x.subject.name << ": ", x.type);
}


} // namespace dep0::typecheck
