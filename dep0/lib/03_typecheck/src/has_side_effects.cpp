#include "private/has_side_effects.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

namespace impl {

static boost::logic::tribool has_side_effects(body_t const&);

static boost::logic::tribool has_side_effects(stmt_t::if_else_t const&);
static boost::logic::tribool has_side_effects(stmt_t::return_t const&);

static boost::logic::tribool has_side_effects(expr_t::typename_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::bool_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::unit_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::i8_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::i16_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::i32_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::i64_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::u8_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::u16_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::u32_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::u64_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::boolean_constant_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::numeric_constant_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::boolean_expr_t const&);
static boost::logic::tribool has_side_effects(expr_t::relation_expr_t const&);
static boost::logic::tribool has_side_effects(expr_t::arith_expr_t const&);
static boost::logic::tribool has_side_effects(expr_t::var_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::global_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::app_t const&);
static boost::logic::tribool has_side_effects(expr_t::abs_t const&);
static boost::logic::tribool has_side_effects(expr_t::pi_t const&);
static boost::logic::tribool has_side_effects(expr_t::array_t const&) { return false; }
static boost::logic::tribool has_side_effects(expr_t::init_list_t const&);
static boost::logic::tribool has_side_effects(expr_t::subscript_t const&);

static boost::logic::tribool has_side_effects(func_arg_t const&);

template <typename R>
boost::logic::tribool any_has_side_effects(R&& r)
{
    boost::logic::tribool result(false);
    for (auto const& x: std::forward<R>(r))
        if (auto const tmp = has_side_effects(x))
            return true;
        else if (boost::logic::indeterminate(tmp))
            result = tmp;
    return result;
}

boost::logic::tribool has_side_effects(body_t const& body)
{
    return any_has_side_effects(body.stmts);
}

boost::logic::tribool has_side_effects(stmt_t::if_else_t const& if_else)
{
    boost::logic::tribool result = has_side_effects(if_else.cond);
    if (result)
        return true;
    result = result or has_side_effects(if_else.true_branch);
    if (result)
        return true;
    if (if_else.false_branch)
        result = result or has_side_effects(*if_else.false_branch);
    return result;
}

boost::logic::tribool has_side_effects(stmt_t::return_t const& ret)
{
    return ret.expr and has_side_effects(*ret.expr);
}


boost::logic::tribool has_side_effects(expr_t::boolean_expr_t const& x)
{
    return match(
        x.value,
        [] (expr_t::boolean_expr_t::not_t const& x) { return has_side_effects(x.expr.get()); },
        [] (auto const& x) { return has_side_effects(x.lhs.get()) or has_side_effects(x.rhs.get()); });
}

boost::logic::tribool has_side_effects(expr_t::relation_expr_t const& x)
{
    return match(x.value, [](auto const& x) { return has_side_effects(x.lhs.get()) or has_side_effects(x.rhs.get()); });
}

boost::logic::tribool has_side_effects(expr_t::arith_expr_t const& x)
{
    return match(x.value, [](auto const& x) { return has_side_effects(x.lhs.get()) or has_side_effects(x.rhs.get()); });
}

boost::logic::tribool has_side_effects(expr_t::app_t const& x)
{
    // TODO a call to a function declaration might contain side effects
    return has_side_effects(x.func.get()) or any_has_side_effects(x.args);
}

boost::logic::tribool has_side_effects(expr_t::abs_t const& x)
{
    return any_has_side_effects(x.args) or has_side_effects(x.body) or has_side_effects(x.ret_type.get());
}

boost::logic::tribool has_side_effects(expr_t::pi_t const& x)
{
    return any_has_side_effects(x.args) or has_side_effects(x.ret_type.get());
}

boost::logic::tribool has_side_effects(expr_t::init_list_t const& x)
{
    return any_has_side_effects(x.values);
}

boost::logic::tribool has_side_effects(expr_t::subscript_t const& x)
{
    return has_side_effects(x.array.get()) or has_side_effects(x.index.get());
}

boost::logic::tribool has_side_effects(func_arg_t const& arg)
{
    return has_side_effects(arg.type);
}

} // namespace impl

boost::logic::tribool has_side_effects(stmt_t const& stmt)
{
    return match(stmt.value, [] (auto const& x) { return impl::has_side_effects(x); });
}

boost::logic::tribool has_side_effects(expr_t const& expr)
{
    return match(expr.value, [] (auto const& x) { return impl::has_side_effects(x); });
}

} // namespace dep0::typecheck
