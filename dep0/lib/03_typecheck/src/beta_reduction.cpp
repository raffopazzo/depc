#include "private/beta_reduction.hpp"

#include "dep0/ast/substitute.hpp"

#include "dep0/match.hpp"

#include <boost/hana.hpp>

#include <algorithm>
#include <cassert>
#include <ranges>

namespace dep0::typecheck {

namespace impl {

template <typename T>
void destructive_self_assign(T& x, T&& y)
{
    auto tmp = std::move(y);
    x = std::move(tmp);
}

static bool beta_normalize(func_decl_t&);
static bool beta_normalize(func_def_t&);
static bool beta_normalize(stmt_t&);
static bool beta_normalize(stmt_t::if_else_t&);
static bool beta_normalize(stmt_t::return_t&);
static bool beta_normalize(expr_t::typename_t&);
static bool beta_normalize(expr_t::bool_t&);
static bool beta_normalize(expr_t::unit_t&);
static bool beta_normalize(expr_t::i8_t&);
static bool beta_normalize(expr_t::i16_t&);
static bool beta_normalize(expr_t::i32_t&);
static bool beta_normalize(expr_t::i64_t&);
static bool beta_normalize(expr_t::u8_t&);
static bool beta_normalize(expr_t::u16_t&);
static bool beta_normalize(expr_t::u32_t&);
static bool beta_normalize(expr_t::u64_t&);
static bool beta_normalize(expr_t::boolean_constant_t&);
static bool beta_normalize(expr_t::numeric_constant_t&);
static bool beta_normalize(expr_t::var_t&);
static bool beta_normalize(expr_t::global_t&);
static bool beta_normalize(expr_t::app_t&);
static bool beta_normalize(expr_t::abs_t&);
static bool beta_normalize(expr_t::pi_t&);
static bool beta_normalize(expr_t::array_t&);
static bool beta_normalize(expr_t::init_list_t&);

bool beta_normalize(func_decl_t& decl)
{
    return impl::beta_normalize(decl.signature);
}

bool beta_normalize(func_def_t& def)
{
    return impl::beta_normalize(def.value);
}

bool beta_normalize(stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return impl::beta_normalize(x); });
}

bool beta_normalize(stmt_t::if_else_t& if_)
{
    bool changed = beta_normalize(if_.cond);
    changed |= beta_normalize(if_.true_branch);
    if (if_.false_branch)
        changed |= beta_normalize(*if_.false_branch);
    return changed;
}

bool beta_normalize(stmt_t::return_t& ret)
{
    return ret.expr and beta_normalize(*ret.expr);
}

bool beta_normalize(expr_t::typename_t&) { return false; }
bool beta_normalize(expr_t::bool_t&) { return false; }
bool beta_normalize(expr_t::unit_t&) { return false; }
bool beta_normalize(expr_t::i8_t&) { return false; }
bool beta_normalize(expr_t::i16_t&) { return false; }
bool beta_normalize(expr_t::i32_t&) { return false; }
bool beta_normalize(expr_t::i64_t&) { return false; }
bool beta_normalize(expr_t::u8_t&) { return false; }
bool beta_normalize(expr_t::u16_t&) { return false; }
bool beta_normalize(expr_t::u32_t&) { return false; }
bool beta_normalize(expr_t::u64_t&) { return false; }
bool beta_normalize(expr_t::boolean_constant_t&) { return false; }
bool beta_normalize(expr_t::numeric_constant_t&) { return false; }
bool beta_normalize(expr_t::var_t&) { return false; }
bool beta_normalize(expr_t::global_t&) { return false; }

bool beta_normalize(expr_t::app_t& app)
{
    // Technically, this classifies as applicative-order-reduction;
    // as such, if the abstraction discards some/all arguments we might waste time reducing those arguments.
    // Currently it doesn't matter; we can reassess in future.
    bool changed = beta_normalize(app.func.get());
    for (auto& arg: app.args)
        changed |= beta_normalize(arg);
    if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
    {
        if (abs->args.size() > 0ul)
        {
            assert(abs->args.size() == app.args.size()); // always true when beta-normalizing legal terms
            for (auto const i: std::views::iota(0ul, abs->args.size()))
            {
                auto const& arg = abs->args[i];
                if (arg.var)
                    substitute(
                        *arg.var,
                        app.args[i],
                        abs->args.begin() + i + 1,
                        abs->args.end(),
                        abs->ret_type.get(),
                        &abs->body);
            }
            // at this point all arguments of the abstraction have been substituted,
            // so we can remove them and normalize the new body;
            // also note that now the function type becomes a pi-type with no arguments
            std::get<expr_t::pi_t>(std::get<expr_t>(app.func.get().properties.sort.get()).value).args.clear();
            app.args.clear();
            abs->args.clear();
            changed = true;
            beta_normalize(abs->body);
        }
    }
    return changed;
}

bool beta_normalize(expr_t::abs_t& abs)
{
    bool changed = false;
    for (auto& arg: abs.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(abs.ret_type.get());
    changed |= beta_normalize(abs.body);
    return changed;
}

bool beta_normalize(expr_t::pi_t& pi)
{
    bool changed = false;
    for (auto& arg: pi.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(pi.ret_type.get());
    return changed;
}

bool beta_normalize(expr_t::array_t&) { return false; }

bool beta_normalize(expr_t::init_list_t& init_list)
{
    bool changed = false;
    for (auto& v: init_list.values)
        changed |= beta_normalize(v);
    return changed;
}

} // namespace impl

bool beta_normalize(module_t& m)
{
    bool changed = false;
    for (auto& decl: m.func_decls)
        changed |= impl::beta_normalize(decl);
    for (auto& def: m.func_defs)
        changed |= impl::beta_normalize(def);
    return changed;
}

bool beta_normalize(body_t& body)
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
        changed |= impl::beta_normalize(*it);
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

bool beta_normalize(expr_t& expr)
{
    return match(
        expr.value,
        [&] (expr_t::app_t& app)
        {
            bool changed = impl::beta_normalize(app);
            // if this reduced to a parameterless single return statement, we can extract the returned expression
            if (app.args.empty())
                if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
                    if (abs->body.stmts.size() == 1ul)
                        if (auto* const ret = std::get_if<stmt_t::return_t>(&abs->body.stmts[0].value))
                            if (ret->expr)
                            {
                                changed = true;
                                impl::destructive_self_assign(expr, std::move(*ret->expr));
                            }
            return changed;
        },
        [&] (expr_t::boolean_expr_t& x)
        {
            return match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t& x)
                {
                    bool changed = beta_normalize(x.expr.get());
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
                    bool changed = beta_normalize(x.lhs.get());
                    changed |= beta_normalize(x.rhs.get());
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
                    bool changed = beta_normalize(x.lhs.get());
                    changed |= beta_normalize(x.rhs.get());
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
                    bool changed = beta_normalize(x.lhs.get());
                    changed |= beta_normalize(x.rhs.get());
                    if (auto const n = std::get_if<expr_t::numeric_constant_t>(&x.lhs.get().value))
                        if (auto const m = std::get_if<expr_t::numeric_constant_t>(&x.rhs.get().value))
                        {
                            changed = true;
                            impl::destructive_self_assign(
                                expr.value,
                                expr_t::value_t{
                                    expr_t::numeric_constant_t{n->value + m->value}
                                });
                        }
                    return changed;
                });
        },
        [&] (expr_t::subscript_t& subscript)
        {
            bool changed = beta_normalize(subscript.array.get());
            changed |= beta_normalize(subscript.index.get());
            if (auto const init_list = std::get_if<expr_t::init_list_t>(&subscript.array.get().value))
                if (auto const i = std::get_if<expr_t::numeric_constant_t>(&subscript.index.get().value))
                    if (i->value <= std::numeric_limits<std::size_t>::max())
                    {
                        changed = true;
                        auto const i_ = i->value.template convert_to<std::size_t>();
                        impl::destructive_self_assign(expr, std::move(init_list->values[i_]));
                    }
            return changed;
        },
        [&] (auto& x) { return impl::beta_normalize(x); });
}

} // namespace dep0::typecheck

