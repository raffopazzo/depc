#include "private/tactics/search_app.hpp"

#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/substitute.hpp"
#include "private/unification.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

std::optional<expr_t>
search_app(
    env_t const& env,
    ctx_t const& ctx,
    expr_t const& type,
    search_state_t& st,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    std::optional<expr_t> result;
    auto const impl =
        [&env, &ctx, &type, &st, &result, usage_multiplier]
        (expr_t::global_t const& name, sort_t const& func_type, usage_t& tmp_usage)
    {
        // TODO what about mutability here?
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
                else if (auto val = continue_proof_search(env, ctx, arg.type, st, tmp_usage, usage_multiplier))
                    // TODO if arg has a name, we should substitute() in later arguments and return type (needs a test)
                    args.push_back(std::move(*val));
                else
                    return;
            }
            result =
                make_legal_expr(
                    std::move(app_type.ret_type.get()),
                    expr_t::app_t{
                        make_legal_expr(func_type, name),
                        std::move(args)});
        }
    };
    for (auto const& name: env.globals())
    {
        auto tmp_usage = usage.extend();
        match(
            *env[name],
            [] (type_def_t const&) {},
            [&] (axiom_t const& axiom)
            {
                // axioms are only viable in an erased context
                if (usage_multiplier == ast::qty_t::zero)
                    impl(name, axiom.properties.sort.get(), tmp_usage);
            },
            [&] (extern_decl_t const& decl) { impl(name, decl.properties.sort.get(), tmp_usage); },
            [&] (func_decl_t const& decl) { impl(name, decl.properties.sort.get(), tmp_usage); },
            [&] (func_def_t const& def) { impl(name, def.properties.sort.get(), tmp_usage); });
        if (result)
        {
            usage += tmp_usage;
            break;
        }
    }
    return result;
}

} // namespace dep0::typecheck
