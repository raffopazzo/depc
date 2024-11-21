/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/tactics/search_app.hpp"

#include "private/derivation_rules.hpp"
#include "private/proof_search.hpp"
#include "private/substitute.hpp"
#include "private/unification.hpp"

#include "dep0/ast/occurs_in.hpp"
#include "dep0/ast/pretty_print.hpp"

#include "dep0/fmap.hpp"
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
    // unification returns a map from an argument's name to the value that we must substitute,
    // but substitution might rename later arguments, so we have to remember the old names
    auto const old_names = fmap(app_type.args, [] (func_arg_t const& arg) { return arg.var; });
    auto const [args_begin, args_end] = std::pair{app_type.args.begin(), app_type.args.end()};
    for (
        auto [arg_it, old_name_it] = std::pair{args_begin, old_names.begin()};
        std::pair{arg_it, old_name_it} != std::pair{args_end, old_names.end()};
        ++arg_it, ++old_name_it
    )
    {
        using node_type = decltype(substitutions->extract(*arg_it->var));
        auto const next_arg = std::next(arg_it);
        auto const& old_name = *old_name_it;
        if (auto const node = old_name ? substitutions->extract(*old_name) : node_type{})
        {
            if (not task.usage->try_add(task.ctx, node.mapped(), task.usage_multiplier))
                return task.set_failed();
            substitute(
                *arg_it->var, // if `old_name` is set, `arg_it->var` is also set (might be renamed but not removed)
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
                not arg_it->var or
                not ast::occurs_in<properties_t>(*arg_it->var, next_arg, args_end, ast::occurrence_style::free);
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
        std::vector<std::shared_ptr<search_task_t>> sub_tasks;
        std::vector<std::size_t> indices; // tracks where exactly the results go in `args`
        auto temp_usage = std::make_shared<usage_t>(task.usage->extend());
        for (auto const idx: std::views::iota(0ul, args.size()))
            if (not args[idx])
            {
                // some search paths might be infinite, for example you can keep applying double-negation-elimination
                // to obtain the chain `true_t(x) => true_t(not not x) => true_t(not not not not x) => ...`;
                // we want to avoid this, so if a path is possibly infinite we only run a quick search
                // just in case there was already a value for, say `true_t(not not x)`, in the context
                bool const possibly_infinite_path = unify(pi.ret_type.get(), app_type.args[idx].type).has_value();
                indices.push_back(idx);
                sub_tasks.push_back(search_task_t::create(
                    possibly_infinite_path ? "quick_search" : "proof_search",
                    task.weak_from_this(),
                    task.state,
                    task.depth,
                    std::make_shared<expr_t>(app_type.args[idx].type),
                    task.is_mutable_allowed,
                    temp_usage,
                    task.usage_multiplier,
                    possibly_infinite_path ? quick_search : proof_search
                ));
            }
        task.when_all(
            std::move(sub_tasks),
            std::move(temp_usage),
            [name, func_type, target=task.target, args=std::move(args), indices=std::move(indices)]
            (std::vector<expr_t> sub_task_results)
            mutable -> expr_t
            {
                for (
                    auto [it_task, it_idx] = std::pair{sub_task_results.begin(), indices.begin()};
                    std::pair{it_task, it_idx} != std::pair{sub_task_results.end(), indices.end()};
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

/**
 * Return true if the given axiom states an absurdity.
 *
 * @remarks
 *      The canonical absurd type is `(typename t) -> t` but this can be generalised.
 *      For example `(typename t, i32_t) -> t` is equally absurd.
 *      However `(typename t, is_integer(t)) -> t` is not absurd because
 *      the second argument introduces a constraint on `t` that can be satisfied.
 *      So, more generally, a pi-type is not absurd if the return type is bound to
 *      a typename argument that appears free in some later argument, i.e. a contraint.
 *      But this only works on terms in normal form, for example `(typename t, f(t)) -> t`
 *      is absurd if you define `func f(typename) -> typename { return i32_t; }`.
 *      Finally, this function only needs to test axioms because absurdity is unprovable.
 */
static bool is_absurd(axiom_t const& axiom)
{
    // TODO normalize the pi-type
    auto const& pi = std::get<expr_t::pi_t>(std::get<expr_t>(axiom.properties.sort.get()).value);
    if (auto const var = std::get_if<expr_t::var_t>(&pi.ret_type.get().value))
        for (auto it = pi.args.begin(); it != pi.args.end(); ++it)
            if (it->var == *var)
                return not ast::occurs_in<properties_t>(*var, std::next(it), pi.args.end(), ast::occurrence_style::free);
    return false;
}

void search_app(search_task_t& task)
{
    std::vector<std::shared_ptr<search_task_t>> sub_tasks;
    auto const unifies_with = [&] (auto const& f)
    {
        auto const& pi = std::get<expr_t::pi_t>(std::get<expr_t>(f.properties.sort.get()).value);
        return unify(pi.ret_type.get(), *task.target).has_value();
    };
    for (auto const& name: task.env.globals())
    {
        bool const viable = match(
            *task.env[name],
            [] (type_def_t const&) { return false; },
            [&] (axiom_t const& axiom)
            {
                // axioms are only viable in an erased context
                return task.usage_multiplier == ast::qty_t::zero and unifies_with(axiom) and not is_absurd(axiom);
            },
            [&] (extern_decl_t const& decl)
            {
                using enum ast::is_mutable_t;
                return task.is_mutable_allowed == yes and unifies_with(decl);
            },
            [&] (func_decl_t const& decl)
            {
                using enum ast::is_mutable_t;
                return (decl.signature.is_mutable == no or task.is_mutable_allowed == yes) and unifies_with(decl);
            },
            [&] (func_def_t const& def)
            {
                using enum ast::is_mutable_t;
                return (def.value.is_mutable == no or task.is_mutable_allowed == yes) and unifies_with(def);
            });
        if (viable)
            sub_tasks.push_back(search_task_t::create(
                [&] { std::ostringstream os; ast::pretty_print<properties_t>(os, name); return os.str(); }(),
                task.weak_from_this(),
                task.state,
                task.depth,
                task.target,
                task.is_mutable_allowed,
                task.usage,
                task.usage_multiplier,
                [name] (search_task_t& t)
                {
                    match(
                        *t.env[name],
                        [] (type_def_t const&) { },
                        [&] (auto const& f)
                        {
                            // TODO pass result of unification to avoid duplicating work
                            try_apply(t, name, f.properties.sort.get());
                        });
                }));
    }
    task.when_any(std::move(sub_tasks));
}

} // namespace dep0::typecheck
