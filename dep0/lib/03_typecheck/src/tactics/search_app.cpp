#include "private/tactics/search_app.hpp"

#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/substitute.hpp"
#include "private/unification.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::optional<expr_t> search_app(environment_t const& env, context_t const& ctx, expr_t const& type)
{
    std::optional<expr_t> result;
    auto const try_application = [&] (expr_t::global_t const& func_name, sort_t const& func_type)
    {
        auto const& pi = std::get<expr_t::pi_t>(std::get<expr_t>(func_type).value);
        if (auto substitutions = unify(pi.ret_type.get(), type))
        {
            auto app_type = pi;
            std::vector<expr_t> args;
            for (auto arg_it = app_type.args.begin(); arg_it != app_type.args.end(); ++arg_it)
            {
                func_arg_t const& arg = *arg_it;
                auto const it = arg.var ? substitutions->find(*arg.var) : substitutions->end();
                if (it != substitutions->end())
                {
                    substitute(
                        it->first,
                        it->second,
                        std::next(arg_it),
                        app_type.args.end(),
                        app_type.ret_type.get(),
                        nullptr);
                    args.push_back(std::move(it->second));
                }
                else if (auto val = proof_search(env, ctx, arg.type))
                    // TODO if arg has a name, we should substitute() in later arguments and return type (needs a test)
                    args.push_back(std::move(*val));
                else
                    return;
            }
            result =
                make_legal_expr(
                    std::move(app_type.ret_type.get()),
                    expr_t::app_t{
                        make_legal_expr(func_type, func_name),
                        std::move(args)});
        }
    };
    for (auto const& name: env.globals())
    {
        match(
            *env[name],
            [] (type_def_t const&) {},
            [&] (axiom_t const& axiom) { try_application(name, axiom.properties.sort.get()); },
            [&] (func_decl_t const& decl) { try_application(name, decl.properties.sort.get()); },
            [&] (func_def_t const& def) { try_application(name, def.properties.sort.get()); });
        if (result)
            break;
    }
    return result;
}

} // namespace dep0::typecheck
