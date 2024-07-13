#include "private/tactics/search_app.hpp"

#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/substitute.hpp"
#include "private/unification.hpp"

#include "dep0/ast/occurs_in.hpp"

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
    auto const impl =
        [&env, &ctx, &type, &st, usage_multiplier]
        (expr_t::global_t const& name, sort_t const& func_type, usage_t& tmp_usage)
        -> std::optional<expr_t>
    {
        auto const& pi = std::get<expr_t::pi_t>(std::get<expr_t>(func_type).value);
        auto substitutions = unify(pi.ret_type.get(), type);
        if (not substitutions)
            return std::nullopt;
        auto app_type = pi;
        std::vector<expr_t> args;
        auto const [args_begin, args_end] = std::pair{app_type.args.begin(), app_type.args.end()};
        for (auto arg_it = args_begin; arg_it != args_end; ++arg_it)
        {
            auto arg_val = [&] () -> std::optional<expr_t>
            {
                func_arg_t const& arg = *arg_it;
                using node_type = decltype(substitutions->extract(*arg.var));
                auto const next_arg = std::next(arg_it);
                if (auto const node = arg.var ? substitutions->extract(*arg.var) : node_type{})
                {
                    substitute(
                        *arg.var,
                        node.mapped(),
                        next_arg,
                        args_end,
                        app_type.ret_type.get(),
                        nullptr);
                    return std::move(node.mapped());
                }
                // We do not know what the "correct" substitution is for this argument,
                // but if later arguments do not depend on it,
                // then its value is irrelevant and any "random" value will be fine.
                bool const irrelevant =
                    not arg.var or
                    not ast::occurs_in<properties_t>(*arg.var, next_arg, args_end, ast::occurrence_style::free);
                if (irrelevant)
                    // TODO for irrelevant arguments of primitive types, we could use any random value, eg 0 for i32_t
                    return continue_proof_search(env, ctx, arg.type, st, tmp_usage, usage_multiplier);
                return std::nullopt;
            }();
            if (arg_val)
                args.push_back(std::move(*arg_val));
            else
                return std::nullopt;
        }
        return make_legal_expr(
            std::move(app_type.ret_type.get()),
            expr_t::app_t{
                make_legal_expr(func_type, name),
                std::move(args)});
    };
    std::optional<expr_t> result;
    for (auto const& name: env.globals())
    {
        auto tmp_usage = usage.extend();
        result = match(
            *env[name],
            [] (type_def_t const&) -> std::optional<expr_t> { return std::nullopt; },
            [&] (axiom_t const& axiom) -> std::optional<expr_t>
            {
                // axioms are only viable in an erased context
                if (usage_multiplier == ast::qty_t::zero)
                    return impl(name, axiom.properties.sort.get(), tmp_usage);
                else
                    return std::nullopt;
            },
            [&] (extern_decl_t const& decl) -> std::optional<expr_t>
            {
                if (is_mutable_allowed(st))
                    return impl(name, decl.properties.sort.get(), tmp_usage);
                else
                    return std::nullopt;
            },
            [&] (func_decl_t const& decl) -> std::optional<expr_t>
            {
                if (decl.signature.is_mutable == ast::is_mutable_t::no or is_mutable_allowed(st))
                    return impl(name, decl.properties.sort.get(), tmp_usage);
                else
                    return std::nullopt;
            },
            [&] (func_def_t const& def) -> std::optional<expr_t>
            {
                if (def.value.is_mutable == ast::is_mutable_t::no or is_mutable_allowed(st))
                    return impl(name, def.properties.sort.get(), tmp_usage);
                else
                    return std::nullopt;
            });
        if (result)
        {
            usage += tmp_usage;
            break;
        }
    }
    return result;
}

} // namespace dep0::typecheck
