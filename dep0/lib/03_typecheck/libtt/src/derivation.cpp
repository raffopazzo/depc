#include "dep0/typecheck/tt/derivation.hpp"

#include "dep0/match.hpp"

#include <sstream>

namespace dep0::typecheck::tt {

struct derivation_rules
{
    static derivation_t form(/*context_t ctx,*/ type_t ty)
    {
        return derivation_t(derivation_t::form_t(std::move(ty)));
    }

    static derivation_t var(context_t ctx, term_t::var_t x, type_t t)
    {
        return derivation_t(derivation_t::var_t(std::move(ctx), std::move(x), std::move(t)));
    }

    static derivation_t app(context_t ctx, derivation_t fun, derivation_t arg)
    {
        auto const fun_type = type_of(fun);
        auto img_type = std::get<type_t::arr_t>(fun_type.value).img.get();
        return derivation_t(derivation_t::app_t(std::move(ctx), std::move(fun), std::move(arg), std::move(img_type)));
    }

    static derivation_t abs(context_t ctx, term_t::var_t var, type_t var_type, derivation_t body)
    {
        auto ty = type_t::arr(var_type, type_of(body));
        return derivation_t(derivation_t::abs_t(std::move(ctx), std::move(var), std::move(var_type), std::move(body), ty));
    }
};

derivation_t derivation_t::bool_t() { return derivation_rules::form(type_t::var(source_text::from_literal("bool"))); }
derivation_t derivation_t::unit_t() { return derivation_rules::form(type_t::var(source_text::from_literal("unit_t"))); }
derivation_t derivation_t::i8_t() { return derivation_rules::form(type_t::var(source_text::from_literal("i8_t"))); }
derivation_t derivation_t::i16_t() { return derivation_rules::form(type_t::var(source_text::from_literal("i16_t"))); }
derivation_t derivation_t::i32_t() { return derivation_rules::form(type_t::var(source_text::from_literal("i32_t"))); }
derivation_t derivation_t::i64_t() { return derivation_rules::form(type_t::var(source_text::from_literal("i64_t"))); }
derivation_t derivation_t::u8_t() { return derivation_rules::form(type_t::var(source_text::from_literal("u8_t"))); }
derivation_t derivation_t::u16_t() { return derivation_rules::form(type_t::var(source_text::from_literal("u16_t"))); }
derivation_t derivation_t::u32_t() { return derivation_rules::form(type_t::var(source_text::from_literal("u32_t"))); }
derivation_t derivation_t::u64_t() { return derivation_rules::form(type_t::var(source_text::from_literal("u64_t"))); }

derivation_t::form_t::form_t(type_t ty) :
    m_ty(std::move(ty))
{ }

derivation_t::var_t::var_t(context_t ctx, term_t::var_t var, type_t ty) :
    m_ctx(std::move(ctx)),
    m_var(std::move(var)),
    m_ty(std::move(ty))
{ }

derivation_t::app_t::app_t(context_t ctx, derivation_t fun, derivation_t arg, type_t ty) :
    m_ctx(std::move(ctx)),
    m_fun(std::move(fun)),
    m_arg(std::move(arg)),
    m_ty(std::move(ty))
{ }

derivation_t::abs_t::abs_t(context_t ctx, term_t::var_t var, type_t var_type, derivation_t body, type_t ty) :
    m_ctx(std::move(ctx)),
    m_var(std::move(var)),
    m_var_type(std::move(var_type)),
    m_body(std::move(body)),
    m_ty(std::move(ty))
{ }

derivation_t::derivation_t(form_t x) : value(std::move(x)) {}
derivation_t::derivation_t(var_t x) : value(std::move(x)) {}
derivation_t::derivation_t(app_t x) : value(std::move(x)) {}
derivation_t::derivation_t(abs_t x) : value(std::move(x)) {}

type_t const& type_of(derivation_t const& d)
{
    return match(d.value, [] (auto const& d) -> auto const& { return d.ty(); });
}

expected<derivation_t> type_assign(context_t const& ctx, type_t::var_t const& x)
{
    if (auto decl = ctx[x])
        return derivation_rules::form(type_t(x));
    else
    {
        std::ostringstream err;
        err << "Unknown type `" << x.name << '`';
        return error_t{err.str()};
    }
}

expected<derivation_t> type_assign(context_t const& ctx, term_t const& x)
{
    return match(
        x.value,
        [&] (term_t::var_t const& x) -> expected<derivation_t>
        {
            if (auto ty = ctx[x])
                return derivation_rules::var(ctx, x, ty->type);
            else
            {
                std::ostringstream err;
                err << "Variable/Function name `"<< x.name <<"` not found in current context";
                return error_t{err.str()};
            }
        },
        [&] (term_t::app_t const& x) -> expected<derivation_t>
        {
            auto left_d = type_assign(ctx, x.left.get());
            if (not left_d)
                return left_d.error();
            auto const left_type = type_of(*left_d); is_arr(left_type);
            if (not is_arr(left_type))
            {
                std::ostringstream err;
                pretty_print(err << "Cannot apply non-function type `", left_type) << '`';
                return error_t{err.str()};
            }
            auto right_d = type_assign(ctx, x.right.get());
            if (not right_d)
                return right_d.error();
            auto const& dom = std::get<type_t::arr_t>(left_type.value).dom.get();
            if (dom != type_of(*right_d))
            {
                std::ostringstream err;
                pretty_print(err << "Type mismatch between function argument of type `", dom);
                pretty_print(err << "` and expression of type `", type_of(*right_d)) << '`';
                return error_t{err.str()};
            }
            return derivation_rules::app(ctx, std::move(*left_d), std::move(*right_d));
        },
        [&] (term_t::abs_t const& x) -> expected<derivation_t>
        {
            auto ext_ctx = ctx.extend();
            if (auto ok = ext_ctx.add(x.var, x.var_type))
                return std::move(ok.error());
            if (auto body_derivation = type_assign(ext_ctx, x.body.get()))
                return derivation_rules::abs(ctx, x.var, x.var_type, std::move(*body_derivation));
            else
                return body_derivation.error();
        });
}

} // namespace dep0::typecheck::tt
