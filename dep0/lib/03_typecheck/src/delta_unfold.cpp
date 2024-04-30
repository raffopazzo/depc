#include "private/delta_unfold.hpp"

#include "private/cpp_int_add.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/destructive_self_assign.hpp"
#include "dep0/match.hpp"

#include <boost/core/ignore_unused.hpp>
#include <boost/hana.hpp>

#include <cassert>
#include <limits>
#include <optional>

namespace dep0::typecheck {

namespace impl {

static bool delta_unfold(env_t const&, ctx_t const&, stmt_t&);
static bool delta_unfold(env_t const&, ctx_t const&, stmt_t::if_else_t&);
static bool delta_unfold(env_t const&, ctx_t const&, stmt_t::return_t&);

static bool delta_unfold(env_t const&, ctx_t const&, expr_t::typename_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::true_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::auto_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::bool_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::cstr_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::unit_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i8_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i16_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i32_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::i64_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u8_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u16_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u32_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::u64_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::boolean_constant_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::numeric_constant_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::string_literal_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::boolean_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::relation_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::arith_expr_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::var_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::global_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::app_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::abs_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::pi_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::array_t&) { return false; }
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::init_list_t&);
static bool delta_unfold(env_t const&, ctx_t const&, expr_t::subscript_t&);

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return delta_unfold(env, ctx, x); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t::if_else_t& if_)
{
    return delta_unfold(env, ctx, if_.cond)
        or delta_unfold(env, ctx, if_.true_branch)
        or if_.false_branch and delta_unfold(env, ctx, *if_.false_branch);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, stmt_t::return_t& ret)
{
    return ret.expr and delta_unfold(env, ctx, *ret.expr);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::boolean_expr_t& x)
{
    return match(
        x.value,
        [&] (expr_t::boolean_expr_t::not_t& x) { return delta_unfold(env, ctx, x.expr.get()); },
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::relation_expr_t& x)
{
    return match(
        x.value,
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::arith_expr_t& x)
{
    return match(
        x.value,
        [&] (auto& x) { return delta_unfold(env, ctx, x.lhs.get()) or delta_unfold(env, ctx, x.rhs.get()); });
}

bool delta_unfold(env_t const&, ctx_t const&, expr_t::global_t&)
{
    // We only perform delta-unfolding inside a direct application,
    // eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
    // The reason for this is that we are interested in delta-unfolding only as
    // a way to extend beta normalization as far as possible.
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::app_t& app)
{
    if (auto const global = std::get_if<expr_t::global_t>(&app.func.get().value))
        if (auto const func_def = std::get_if<func_def_t>(env[*global]))
        {
            app.func.get().value = func_def->value;
            return true;
        }
    if (delta_unfold(env, ctx, app.func.get()))
        return true;
    for (auto& arg: app.args)
        if (delta_unfold(env, ctx, arg))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: abs.args)
    {
        if (delta_unfold(env, ctx2, arg.type))
            return true;
        auto const inserted = ctx2.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(inserted.has_value());
    }
    if (delta_unfold(env, ctx2, abs.ret_type.get()))
        return true;
    return delta_unfold(env, ctx2, abs.body);
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::pi_t& pi)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: pi.args)
    {
        if (delta_unfold(env, ctx2, arg.type))
            return true;
        auto const inserted = ctx2.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(inserted.has_value());
    }
    return delta_unfold(env, ctx2, pi.ret_type.get());
}


bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::init_list_t& init_list)
{
    for (auto& v: init_list.values)
        if (delta_unfold(env, ctx, v))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t::subscript_t& subscript)
{
    return delta_unfold(env, ctx, subscript.array.get()) or delta_unfold(env, ctx, subscript.index.get());
}

} // namespace impl

bool delta_unfold(env_t const& env, ctx_t const& ctx, body_t& body)
{
    for (auto& s: body.stmts)
        if (impl::delta_unfold(env, ctx, s))
            return true;
    return false;
}

bool delta_unfold(env_t const& env, ctx_t const& ctx, expr_t& expr)
{
    if (auto const type = std::get_if<expr_t>(&expr.properties.sort.get()))
        if (delta_unfold(env, ctx, *type))
            return true;
    // for boolean_expr_t, relation_expr_t,  etc, prefer primitive reduction over further unfolding
    return match(
        expr.value,
        [&] (expr_t::boolean_expr_t& x)
        {
            bool changed = match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t& x)
                {
                    if (auto const c = std::get_if<expr_t::boolean_constant_t>(&x.expr.get().value))
                    {
                        bool const value = not c->value; // compute result before destructive self assignment
                        expr.value.template emplace<expr_t::boolean_constant_t>(value);
                        return true;
                    }
                    return false;
                },
                [&] <typename T> (T& x)
                {
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
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
                            return true;
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::relation_expr_t& x)
        {
            bool changed = match(
                x.value,
                [&] <typename T> (T& x)
                {
                    if (auto const a = std::get_if<expr_t::boolean_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::boolean_constant_t>(&x.rhs.get().value))
                        {
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
                            return true;
                        }
                    if (auto const a = std::get_if<expr_t::numeric_constant_t>(&x.lhs.get().value))
                        if (auto const b = std::get_if<expr_t::numeric_constant_t>(&x.rhs.get().value))
                        {
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
                            return true;
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::arith_expr_t& x)
        {
            bool changed = match(
                x.value,
                [&] (expr_t::arith_expr_t::plus_t& x)
                {
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
                                    assert(false and "type of numeric constant was not normalized");
                                });
                            if (result)
                            {
                                expr.value = expr_t::numeric_constant_t{std::move(*result)};
                                return true;
                            }
                        }
                    return false;
                });
            return changed or impl::delta_unfold(env, ctx, x);
        },
        [&] (expr_t::subscript_t& subscript)
        {
            bool changed = false;
            if (auto const init_list = std::get_if<expr_t::init_list_t>(&subscript.array.get().value))
                if (auto const i = std::get_if<expr_t::numeric_constant_t>(&subscript.index.get().value))
                    if (i->value <= std::numeric_limits<std::size_t>::max())
                    {
                        changed = true;
                        auto const i_ = i->value.template convert_to<std::size_t>();
                        destructive_self_assign(expr.value, std::move(init_list->values[i_].value));
                    }
            return changed or impl::delta_unfold(env, ctx, subscript);
        },
        [&] (auto& x) { return impl::delta_unfold(env, ctx, x); });
}

} // namespace dep0::typecheck
