/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_attrs.hpp"

#include "dep0/match.hpp"

namespace dep0::llvmgen {

llvm::Attribute::AttrKind get_sign_ext_attribute(global_ctx_t const& global, typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::true_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::auto_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::ref_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::scope_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::addressof_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::deref_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::scopeof_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::bool_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::cstr_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::unit_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::i8_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i16_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i32_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::i64_t const&) { return llvm::Attribute::SExt; },
        [] (typecheck::expr_t::u8_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u16_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u32_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::u64_t const&) { return llvm::Attribute::ZExt; },
        [] (typecheck::expr_t::boolean_constant_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::numeric_constant_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::string_literal_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::boolean_expr_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::relation_expr_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::arith_expr_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::var_t const&) { return llvm::Attribute::None; },
        [&] (typecheck::expr_t::global_t const& g)
        {
            auto const val = global[g];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm_func_t const&) -> llvm::Attribute::AttrKind
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [&] (global_ctx_t::type_def_t const& t)
                {
                    return match(
                        t.def.value,
                        [&] (typecheck::type_def_t::integer_t const& x)
                        {
                            return x.sign == ast::sign_t::signed_v ? llvm::Attribute::SExt : llvm::Attribute::ZExt;
                        },
                        [] (typecheck::type_def_t::struct_t const&) { return llvm::Attribute::None; });
                });
        },
        [] (typecheck::expr_t::app_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::abs_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::pi_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::sigma_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::array_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::init_list_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::member_t const&) { return llvm::Attribute::None; },
        [] (typecheck::expr_t::subscript_t const&) { return llvm::Attribute::None; },
        [&] (typecheck::expr_t::because_t const& x) { return get_sign_ext_attribute(global, x.value.get()); });
}

} // namespace dep0::llvmgen
