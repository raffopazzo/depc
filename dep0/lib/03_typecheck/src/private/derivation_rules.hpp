#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/derivation.hpp"

namespace dep0::typecheck {

struct derivation_rules
{
    // At the moment there is no requirements to make any derivation; but could add some, if useful.
    template <typename T>
    static derivation_t<T> make_derivation();

    static expr_t make_typename();
    static expr_t make_bool();
    static expr_t make_unit();
    static expr_t make_i8();
    static expr_t make_i16();
    static expr_t make_i32();
    static expr_t make_i64();
    static expr_t make_u8();
    static expr_t make_u16();
    static expr_t make_u32();
    static expr_t make_u64();

    static expr_t make_true();
    static expr_t make_false();
};

template <typename... Args>
module_t make_legal_module(Args&&... args)
{
    return module_t{derivation_rules::make_derivation<module_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
type_def_t make_legal_type_def(Args&&... args)
{
    return type_def_t{derivation_rules::make_derivation<type_def_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
func_def_t make_legal_func_def(sort_t sort, Args&&... args)
{
    return func_def_t{derivation_rules::make_derivation<func_def_t>(), std::move(sort), std::forward<Args>(args)...};
}

template <typename... Args>
func_arg_t make_legal_func_arg(Args&&... args)
{
    return func_arg_t{derivation_rules::make_derivation<func_arg_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
body_t make_legal_body(Args&&... args)
{
    return body_t{derivation_rules::make_derivation<body_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
stmt_t make_legal_stmt(Args&&... args)
{
    return stmt_t{derivation_rules::make_derivation<stmt_t>(), std::forward<Args>(args)...};
}

template <typename... Args>
expr_t make_legal_expr(sort_t sort, Args&&... args)
{
    return expr_t{derivation_rules::make_derivation<expr_t>(), std::move(sort), std::forward<Args>(args)...};
}

template <typename T>
derivation_t<T> derivation_rules::make_derivation()
{
    return derivation_t<T>{};
}

expr_t derivation_rules::make_typename() { return make_legal_expr(kind_t{}, expr_t::typename_t{}); }
expr_t derivation_rules::make_bool() { return make_legal_expr(make_typename(), expr_t::bool_t{}); }
expr_t derivation_rules::make_unit() { return make_legal_expr(make_typename(), expr_t::unit_t{}); }
expr_t derivation_rules::make_i8() { return make_legal_expr(make_typename(), expr_t::i8_t{}); }
expr_t derivation_rules::make_i16() { return make_legal_expr(make_typename(), expr_t::i16_t{}); }
expr_t derivation_rules::make_i32() { return make_legal_expr(make_typename(), expr_t::i32_t{}); }
expr_t derivation_rules::make_i64() { return make_legal_expr(make_typename(), expr_t::i64_t{}); }
expr_t derivation_rules::make_u8() { return make_legal_expr(make_typename(), expr_t::u8_t{}); }
expr_t derivation_rules::make_u16() { return make_legal_expr(make_typename(), expr_t::u16_t{}); }
expr_t derivation_rules::make_u32() { return make_legal_expr(make_typename(), expr_t::u32_t{}); }
expr_t derivation_rules::make_u64() { return make_legal_expr(make_typename(), expr_t::u64_t{}); }

expr_t derivation_rules::make_true() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{true}); }
expr_t derivation_rules::make_false() { return make_legal_expr(make_bool(), expr_t::boolean_constant_t{false}); }

} // namespace dep0::typecheck
