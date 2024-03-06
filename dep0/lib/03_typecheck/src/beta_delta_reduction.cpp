#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "private/cpp_int_add.hpp"
#include "private/delta_unfold.hpp"
#include "private/derivation_rules.hpp"
#include "private/substitute.hpp"

#include "dep0/match.hpp"

#include <boost/core/ignore_unused.hpp>
#include <boost/hana.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>

namespace dep0::typecheck {

namespace impl {

template <typename T>
void destructive_self_assign(T& x, T&& y)
{
    auto tmp = std::move(y);
    x = std::move(tmp);
}

static bool beta_delta_normalize(environment_t const&, func_decl_t&);
static bool beta_delta_normalize(environment_t const&, func_def_t&);

static bool beta_delta_normalize(environment_t const&, context_t const&, sort_t&);

static bool beta_delta_normalize(environment_t const&, context_t const&, body_t&);

static bool beta_delta_normalize(environment_t const&, context_t const&, stmt_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, stmt_t::if_else_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, stmt_t::return_t&);

static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::typename_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::bool_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::unit_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i8_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i16_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i32_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i64_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u8_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u16_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u32_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u64_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::boolean_constant_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::numeric_constant_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::var_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::global_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::app_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::abs_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::pi_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::array_t&);
static bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::init_list_t&);

bool beta_delta_normalize(environment_t const& env, func_decl_t& decl)
{
    context_t ctx;
    bool changed = false;
    for (func_arg_t& arg: decl.signature.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        if (arg.var)
        {
            auto const ok = ctx.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(ok.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx, decl.signature.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, decl.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(environment_t const& env, func_def_t& def)
{
    context_t ctx;
    bool changed = false;
    for (func_arg_t& arg: def.value.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        if (arg.var)
        {
            auto const ok = ctx.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(ok.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx, def.value.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, def.value.body);
    changed |= beta_delta_normalize(env, ctx, def.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, sort_t& sort)
{
    return match(
        sort,
        [&] (expr_t& type) { return beta_delta_normalize(env, ctx, type); },
        [] (kind_t const&) { return false; });
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, body_t& body)
{
    auto const is_return = [] (stmt_t const& x)
    {
        return std::holds_alternative<stmt_t::return_t>(x.value);
    };
    // NB taking `stmts` by value because we are about to perform a destructive self-assignment
    auto replace_with = [&] (std::vector<stmt_t>::iterator it, std::vector<stmt_t> stmts)
    {
        // If there is nothing to replace `it` with, then just remove it;
        // otherwise we need to splice all the replacing statements in place of `it`,
        // which can be done efficiently by replacing `it` with the first statement, and insert the rest after it.
        // Finally, if the new statements contain a return statment, any other statement after that can be dropped.
        if (stmts.empty())
            return body.stmts.erase(it);
        auto const index_of_ret = [&]
        {
            auto const j = std::ranges::find_if(stmts, is_return);
            return j == stmts.end() ? std::nullopt : std::optional{std::distance(stmts.begin(), j)};
        }();
        auto j = stmts.begin();
        *it++ = std::move(*j++);
        it = body.stmts.insert(it, std::make_move_iterator(j), std::make_move_iterator(stmts.end()));
        if (index_of_ret)
            // if you were expecting a `+1` here, the reason it's missing is because we manually incremented `it` once
            body.stmts.erase(it + *index_of_ret, body.stmts.end());
        return it;
    };
    bool changed = false;
    auto it = body.stmts.begin();
    while (it != body.stmts.end())
    {
        changed |= beta_delta_normalize(env, ctx, *it);
        it = match(
            it->value,
            [&] (expr_t::app_t& app)
            {
                // if immediately-invoked lambda we can extract the body,
                // but have to suppress return statements from the invoked lambda,
                // otherwise they would become early returns from the parent function
                // TODO blindly suppressing return of the invoked lambda will be wrong once we have side-effects
                if (app.args.empty())
                    if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
                    {
                        changed = true;
                        // TODO fix this: if body contains 2 return statements, all others in between must be dropped
                        std::erase_if(abs->body.stmts, is_return);
                        return replace_with(it, std::move(abs->body.stmts));
                    }
                return std::next(it);
            },
            [&] (stmt_t::if_else_t& if_)
            {
                // this step is arguably iota-reduction
                if (auto const c = std::get_if<expr_t::boolean_constant_t>(&if_.cond.value))
                {
                    changed = true;
                    if (c->value)
                        return replace_with(it, std::move(if_.true_branch.stmts));
                    else if (if_.false_branch)
                        return replace_with(it, std::move(if_.false_branch->stmts));
                    else
                        return body.stmts.erase(it);
                }
                else
                    return std::next(it);
            },
            [&] (stmt_t::return_t const&) { return std::next(it); });
    }
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return beta_delta_normalize(env, ctx, x); });
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, stmt_t::if_else_t& if_)
{
    bool changed = beta_delta_normalize(env, ctx, if_.cond);
    changed |= beta_delta_normalize(env, ctx, if_.true_branch);
    if (if_.false_branch)
        changed |= beta_delta_normalize(env, ctx, *if_.false_branch);
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, stmt_t::return_t& ret)
{
    return ret.expr and beta_delta_normalize(env, ctx, *ret.expr);
}

bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::typename_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::bool_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::unit_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i8_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i16_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i32_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::i64_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u8_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u16_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u32_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::u64_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::boolean_constant_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::numeric_constant_t&) { return false; }
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::var_t&) { return false; }

bool beta_delta_normalize(environment_t const&, context_t const&, expr_t::global_t&)
{
    // We only perform delta-unfolding inside a direct application,
    // eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
    // The reason for this is that we are interested in delta-unfolding only as
    // a way to extend beta normalization as far as possible.
    return false;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t::app_t& app)
{
    // Technically, this classifies as applicative-order-reduction;
    // as such, if the abstraction discards some/all arguments we might waste time reducing those arguments.
    // Currently it doesn't matter; we can reassess in future.
    bool changed = beta_delta_normalize(env, ctx, app.func.get());
    for (auto& arg: app.args)
        changed |= beta_delta_normalize(env, ctx, arg);
    changed |= delta_unfold(env, ctx, app.func.get());
    if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
    {
        if (abs->args.size() > 0ul)
        {
            assert(abs->args.size() == app.args.size()); // always true for legal terms
            for (auto const i: std::views::iota(0ul, abs->args.size()))
                if (auto const& arg = abs->args[i]; arg.var)
                    substitute(
                        *arg.var,
                        app.args[i],
                        abs->args.begin() + i + 1,
                        abs->args.end(),
                        abs->ret_type.get(),
                        &abs->body);
            // at this point all arguments of the abstraction have been substituted,
            // so we can remove them and normalize the new body;
            // also note that now the function type becomes a pi-type with no arguments
            // TODO we should also perform substitution inside the function type but we need a test first
            std::get<expr_t::pi_t>(std::get<expr_t>(app.func.get().properties.sort.get()).value).args.clear();
            app.args.clear();
            abs->args.clear();
            changed = true;
            beta_delta_normalize(env, ctx, abs->body);
        }
    }
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    bool changed = false;
    for (auto& arg: abs.args)
    {
        changed |= beta_delta_normalize(env, ctx2, arg.type);
        if (arg.var)
        {
            auto const inserted = ctx2.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(inserted.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx2, abs.ret_type.get());
    changed |= beta_delta_normalize(env, ctx2, abs.body);
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t::pi_t& pi)
{
    auto ctx2 = ctx.extend();
    bool changed = false;
    for (auto& arg: pi.args)
    {
        changed |= beta_delta_normalize(env, ctx2, arg.type);
        if (arg.var)
        {
            auto const inserted = ctx2.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(inserted.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx2, pi.ret_type.get());
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t::array_t&) { return false; }

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t::init_list_t& init_list)
{
    bool changed = false;
    for (auto& v: init_list.values)
        changed |= beta_delta_normalize(env, ctx, v);
    return changed;
}

} // namespace impl

bool beta_delta_normalize(module_t& m)
{
    environment_t env;
    bool changed = false;
    for (auto& x: m.entries)
        // normalize first and then store in env, so future look-ups will find the normalized term
        changed |= match(
            x,
            [&] (type_def_t const& def)
            {
                // nothing to normalize but still need to add the definition in the environment
                auto const& name = match(def.value, [&] (auto const& x) { return x.name; });
                auto const ok = env.try_emplace(expr_t::global_t{name}, def);
                assert(ok.has_value());
                return false;
            },
            [&] (func_decl_t& decl)
            {
                bool const result = impl::beta_delta_normalize(env, decl);
                auto const ok = env.try_emplace(expr_t::global_t{decl.name}, decl);
                assert(ok.has_value());
                return result;
            },
            [&] (func_def_t& def)
            {
                bool const result = impl::beta_delta_normalize(env, def);
                auto const ok = env.try_emplace(expr_t::global_t{def.name}, def);
                assert(ok.has_value());
                return result;
            });
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t& expr)
{
    bool changed = impl::beta_delta_normalize(env, ctx, expr.properties.sort.get());
    changed |= match(
        expr.value,
        [&] (expr_t::app_t& app)
        {
            bool changed = impl::beta_delta_normalize(env, ctx, app);
            // if this reduced to a parameterless single return statement, we can extract the returned expression
            if (app.args.empty())
                if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
                    if (abs->body.stmts.size() == 1ul)
                        if (auto* const ret = std::get_if<stmt_t::return_t>(&abs->body.stmts[0].value))
                            if (ret->expr)
                            {
                                changed = true;
                                impl::destructive_self_assign(expr.value, std::move(ret->expr->value));
                            }
            return changed;
        },
        [&] (expr_t::boolean_expr_t& x)
        {
            return match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t& x)
                {
                    bool changed = beta_delta_normalize(env, ctx, x.expr.get());
                    if (auto const c = std::get_if<expr_t::boolean_constant_t>(&x.expr.get().value))
                    {
                        changed = true;
                        bool const value = not c->value; // compute result before destructive self assignment
                        expr.value.template emplace<expr_t::boolean_constant_t>(value);
                    }
                    return changed;
                },
                [&] <typename T> (T& x)
                {
                    bool changed = beta_delta_normalize(env, ctx, x.lhs.get());
                    changed |= beta_delta_normalize(env, ctx, x.rhs.get());
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
                            changed = true;
                            bool const c =
                                boost::hana::overload(
                                    [&] (boost::hana::type<expr_t::boolean_expr_t::and_t>)
                                    {
                                        return a->value and b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::boolean_expr_t::or_t>)
                                    {
                                        return a->value or b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::boolean_expr_t::xor_t>)
                                    {
                                        return a->value xor b->value;
                                    })(boost::hana::type_c<T>);
                            expr.value.template emplace<expr_t::boolean_constant_t>(c);
                        }
                    return changed;
                });
        },
        [&] (expr_t::relation_expr_t& x)
        {
            return match(
                x.value,
                [&] <typename T> (T& x)
                {
                    bool changed = beta_delta_normalize(env, ctx, x.lhs.get());
                    changed |= beta_delta_normalize(env, ctx, x.rhs.get());
                    // TODO numeric constants too
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
                            changed = true;
                            bool const c =
                                boost::hana::overload(
                                    [&] (boost::hana::type<expr_t::relation_expr_t::eq_t>)
                                    {
                                        return a->value == b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::relation_expr_t::neq_t>)
                                    {
                                        return a->value != b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::relation_expr_t::gt_t>)
                                    {
                                        return a->value > b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::relation_expr_t::gte_t>)
                                    {
                                        return a->value >= b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::relation_expr_t::lt_t>)
                                    {
                                        return a->value < b->value;
                                    },
                                    [&] (boost::hana::type<expr_t::relation_expr_t::lte_t>)
                                    {
                                        return a->value <= b->value;
                                    })(boost::hana::type_c<T>);
                            expr.value.template emplace<expr_t::boolean_constant_t>(c);
                        }
                    return changed;
                });
        },
        [&] (expr_t::arith_expr_t& x)
        {
            return match(
                x.value,
                [&] (expr_t::arith_expr_t::plus_t& x)
                {
                    bool changed = beta_delta_normalize(env, ctx, x.lhs.get());
                    changed |= beta_delta_normalize(env, ctx, x.rhs.get());
                    if (auto const n = std::get_if<expr_t::numeric_constant_t>(&x.lhs.get().value))
                        if (auto const m = std::get_if<expr_t::numeric_constant_t>(&x.rhs.get().value))
                        {
                            std::optional<boost::multiprecision::cpp_int> result;
                            match(
                                std::get<expr_t>(x.lhs.get().properties.sort.get()).value,
                                [&] (expr_t::i8_t) { result = cpp_int_add_signed<8>(n->value, m->value); },
                                [&] (expr_t::i16_t) { result = cpp_int_add_signed<16>(n->value, m->value); },
                                [&] (expr_t::i32_t) { result = cpp_int_add_signed<32>(n->value, m->value); },
                                [&] (expr_t::i64_t) { result = cpp_int_add_signed<64>(n->value, m->value); },
                                [&] (expr_t::u8_t) { result = cpp_int_add_unsigned<8>(n->value, m->value); },
                                [&] (expr_t::u16_t) { result = cpp_int_add_unsigned<16>(n->value, m->value); },
                                [&] (expr_t::u32_t) { result = cpp_int_add_unsigned<32>(n->value, m->value); },
                                [&] (expr_t::u64_t) { result = cpp_int_add_unsigned<64>(n->value, m->value); },
                                [&] (expr_t::global_t const& g)
                                {
                                    if (auto const type_def = std::get_if<type_def_t>(env[g]))
                                        match(
                                            type_def->value,
                                            [&] (type_def_t::integer_t const& integer)
                                            {
                                                auto const& [name, sign, width, max_abs_value] = integer;
                                                boost::ignore_unused(name);
                                                result = cpp_int_add(sign, width, n->value, m->value);
                                                if (max_abs_value and *result > *max_abs_value)
                                                {
                                                    if (sign == ast::sign_t::signed_v)
                                                        result.emplace(-*max_abs_value);
                                                    else
                                                        result.emplace(0);
                                                }
                                            });
                                },
                                [&] (auto const&)
                                {
                                    assert(false and "unexpected type for plus expr");
                                });
                            if (result)
                            {
                                changed = true;
                                expr.value = expr_t::numeric_constant_t{std::move(*result)};
                            }
                        }
                    return changed;
                });
        },
        [&] (expr_t::subscript_t& subscript)
        {
            bool changed = beta_delta_normalize(env, ctx, subscript.array.get());
            changed |= beta_delta_normalize(env, ctx, subscript.index.get());
            if (auto const init_list = std::get_if<expr_t::init_list_t>(&subscript.array.get().value))
                if (auto const i = std::get_if<expr_t::numeric_constant_t>(&subscript.index.get().value))
                    if (i->value <= std::numeric_limits<std::size_t>::max())
                    {
                        changed = true;
                        auto const i_ = i->value.template convert_to<std::size_t>();
                        impl::destructive_self_assign(expr.value, std::move(init_list->values[i_].value));
                    }
            return changed;
        },
        [&] (auto& x) { return impl::beta_delta_normalize(env, ctx, x); });
    return changed;
}

} // namespace dep0::typecheck
