#include "private/tactics/search_app.hpp"

#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/substitute.hpp"
#include "private/unification.hpp"

#include "dep0/ast/occurs_in.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <optional>
#include <ranges>
#include <vector>

namespace dep0::typecheck {

/**
 * Extract all expressions from the given vector keeping the initial order;
 * if any is missing return nothing and leave the input vector unchanged.
 */
static std::optional<std::vector<expr_t>> all_or_nothing(std::vector<std::optional<expr_t>>& xs)
{
    std::optional<std::vector<expr_t>> result;
    if (std::ranges::all_of(xs, [] (auto const& x) { return x.has_value(); }))
    {
        result.emplace().reserve(xs.size());
        for (auto& x: xs)
            result->push_back(std::move(*x));
    }
    return result;
}

static void try_apply(search_task_t& task, expr_t::global_t const& name, sort_t const& func_type)
{
    auto const& target = *task.target;
    auto const& pi = std::get<expr_t::pi_t>(std::get<expr_t>(func_type).value);
    auto substitutions = unify(pi.ret_type.get(), target);
    if (not substitutions)
        return;
    auto app_type = pi;
    std::vector<std::optional<expr_t>> args;
    args.reserve(app_type.args.size());
    auto const [args_begin, args_end] = std::pair{app_type.args.begin(), app_type.args.end()};
    for (auto arg_it = args_begin; arg_it != args_end; ++arg_it)
    {
        func_arg_t const& func_arg = *arg_it;
        using node_type = decltype(substitutions->extract(*func_arg.var));
        auto const next_arg = std::next(arg_it);
        if (auto const node = func_arg.var ? substitutions->extract(*func_arg.var) : node_type{})
        {
            substitute(
                *func_arg.var,
                node.mapped(),
                next_arg,
                args_end,
                app_type.ret_type.get(),
                nullptr);
            args.emplace_back(std::move(node.mapped()));
        }
        else
        {
            // We do not know what the "correct" substitution is for this argument.
            // But if later arguments do not depend on it, then its exact value is irrelevant.
            // This is normally known as proof-irrelevance.
            // For proof-irrelevant arguments any "random" value will be fine.
            // On the contrary, if an argument `b` depends on `a`, then the exact value of `a` actually matters;
            // for example, in `f(bool a, true_t(a))` you cannot just use any boolean value for `a` because,
            // if you choose the wrong one, you may not find a value for `true_t(a)`.
            bool const irrelevant =
                not func_arg.var or
                not ast::occurs_in<properties_t>(*func_arg.var, next_arg, args_end, ast::occurrence_style::free);
            if (irrelevant)
                // TODO for irrelevant arguments of primitive types, we could use any random value, eg 0 for i32_t
                args.push_back(std::nullopt);
            else
                // could not find some relevant argument, so this search failed
                // TODO try unify arguments with context from right to left, then take intersection
                return;
        }
    }
    if (auto all_args = all_or_nothing(args))
        task.set_result(make_legal_expr(
            target,
            expr_t::app_t{make_legal_expr(func_type, name), std::move(*all_args)}));
    else
    {
        // Some arguments are still unresolved.
        // They must be proof-irrelevant, otherwise we would have failed earlier.
        // So schedule some sub-tasks to find some suitable value.
        std::vector<search_task_t> sub_tasks;
        std::vector<std::size_t> indices; // tracks where exactly the results go in `args`
        for (auto const idx: std::views::iota(0ul, args.size()))
            if (not args[idx])
            {
                indices.push_back(idx);
                sub_tasks.push_back(search_task_t{
                    task.depth + 1,
                    task.state,
                    std::make_shared<expr_t>(app_type.args[idx].type),
                    task.is_mutable_allowed,
                    task.usage,
                    task.usage_multiplier,
                    proof_search
                });
            }
        task.when_all(
            std::move(sub_tasks),
            [name, func_type, target=task.target, args=std::move(args), indices=std::move(indices)]
            (std::vector<expr_t> sub_task_results)
            mutable -> expr_t
            {
                for (
                    auto [it_task, it_idx] = std::pair{sub_task_results.begin(), indices.begin()};
                    it_task != sub_task_results.end() and it_idx != indices.end();
                    ++it_task, ++it_idx
                )
                {
                    auto& arg = args[*it_idx];
                    assert(not arg.has_value());
                    arg.emplace(std::move(*it_task));
                }
                auto all_args = all_or_nothing(args);
                assert(all_args and "when_all thinks it succeeded but some arguments are still unresolved!");
                return make_legal_expr(*target, expr_t::app_t{make_legal_expr(func_type, name), std::move(*all_args)});
            });
    }
}

void search_app(search_task_t& task)
{
    std::vector<search_task_t> sub_tasks;
    for (auto const& name: task.env.globals())
        sub_tasks.push_back(search_task_t(
            task.depth + 1,
            task.state,
            task.target,
            task.is_mutable_allowed,
            task.usage,
            task.usage_multiplier,
            [name, p=task.env[name]] (search_task_t& t)
            {
                using enum ast::is_mutable_t;
                match(
                    *p,
                    [] (type_def_t const&) { },
                    [&] (axiom_t const& axiom)
                    {
                        // axioms are only viable in an erased context
                        if (t.usage_multiplier == ast::qty_t::zero)
                            try_apply(t, name, axiom.properties.sort.get());
                    },
                    [&] (extern_decl_t const& decl)
                    {
                        if (t.is_mutable_allowed == yes)
                            try_apply(t, name, decl.properties.sort.get());
                    },
                    [&] (func_decl_t const& decl)
                    {
                        if (decl.signature.is_mutable == no or t.is_mutable_allowed == yes)
                            try_apply(t, name, decl.properties.sort.get());
                    },
                    [&] (func_def_t const& def)
                    {
                        if (def.value.is_mutable == no or t.is_mutable_allowed == yes)
                            try_apply(t, name, def.properties.sort.get());
                    });
            }
        ));
    task.when_any(std::move(sub_tasks));
}

} // namespace dep0::typecheck
