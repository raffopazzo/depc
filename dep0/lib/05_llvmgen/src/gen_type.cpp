/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_type.hpp"

#include "private/gen_array.hpp"

#include "dep0/fmap.hpp"
#include "dep0/match.hpp"

#include <algorithm>
#include <vector>

namespace dep0::llvmgen {

llvm::FunctionType* gen_func_type(global_ctx_t const& global, llvm_func_proto_t const& proto)
{
    bool constexpr is_var_arg = false;
    std::vector<llvm::Type*> arg_types; // might need to contain a return argument
    auto const ret_type =
        match(
            pass_by_ptr(global, proto.ret_type()),
            [&] (pass_by_ptr_result::no_t)
            {
                return gen_type(global, proto.ret_type());
            },
            [&] (pass_by_ptr_result::struct_t const&)
            {
                arg_types.push_back(gen_type(global, proto.ret_type())->getPointerTo());
                return llvm::Type::getVoidTy(global.llvm_ctx);
            },
            [&] (pass_by_ptr_result::sigma_t const&)
            {
                arg_types.push_back(gen_type(global, proto.ret_type())->getPointerTo());
                return llvm::Type::getVoidTy(global.llvm_ctx);
            },
            [&] (pass_by_ptr_result::array_t const& array)
            {
                arg_types.push_back(gen_type(global, array.properties.element_type)->getPointerTo());
                return llvm::Type::getVoidTy(global.llvm_ctx);
            });
    arg_types.reserve(arg_types.size() + proto.runtime_args().size());
    for (typecheck::func_arg_t const& arg: proto.runtime_args())
        arg_types.push_back(
            match(
                pass_by_ptr(global, arg.type),
                [&] (pass_by_ptr_result::no_t) -> llvm::Type*
                {
                    return gen_type(global, arg.type);
                },
                [&] (pass_by_ptr_result::struct_t const&) -> llvm::Type*
                {
                    return gen_type(global, arg.type)->getPointerTo();
                },
                [&] (pass_by_ptr_result::sigma_t const&) -> llvm::Type*
                {
                    return gen_type(global, arg.type)->getPointerTo();
                },
                [&] (pass_by_ptr_result::array_t const& array) -> llvm::Type*
                {
                    return gen_type(global, array.properties.element_type)->getPointerTo();
                }));
    return llvm::FunctionType::get(ret_type, std::move(arg_types), is_var_arg);
}

llvm::IntegerType* gen_type(global_ctx_t const& global, ast::width_t const width)
{
    switch (width)
    {
    case ast::width_t::_8: return llvm::Type::getInt8Ty(global.llvm_ctx);
    case ast::width_t::_16: return llvm::Type::getInt16Ty(global.llvm_ctx);
    case ast::width_t::_32: return llvm::Type::getInt32Ty(global.llvm_ctx);
    case ast::width_t::_64: return llvm::Type::getInt64Ty(global.llvm_ctx);
    default: __builtin_unreachable();
    }
}

llvm::Type* gen_type(global_ctx_t const& global, typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for typename");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::true_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for true_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::auto_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for auto expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::bool_t const&) -> llvm::Type*
        {
            return llvm::Type::getInt1Ty(global.llvm_ctx);
        },
        [&] (typecheck::expr_t::cstr_t const&) -> llvm::Type*
        {
            return llvm::Type::getInt8Ty(global.llvm_ctx)->getPointerTo();
        },
        [&] (typecheck::expr_t::unit_t const&) -> llvm::Type*
        {
            return llvm::StructType::get(global.llvm_ctx);
        },
        [&] (typecheck::expr_t::i8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::i64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u8_t const&) -> llvm::Type* { return llvm::Type::getInt8Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u16_t const&) -> llvm::Type* { return llvm::Type::getInt16Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u32_t const&) -> llvm::Type* { return llvm::Type::getInt32Ty(global.llvm_ctx); },
        [&] (typecheck::expr_t::u64_t const&) -> llvm::Type* { return llvm::Type::getInt64Ty(global.llvm_ctx); },
        [] (typecheck::expr_t::boolean_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean constant");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::numeric_constant_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a numeric constant");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::string_literal_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a string literal");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::boolean_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a boolean expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::relation_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a relation expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::arith_expr_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an arithmetic expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::var_t const&) -> llvm::Type*
        {
            assert(false and "found a value but was expecting a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::global_t const& g) -> llvm::Type*
        {
            auto const val = global[g];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm_func_t const&) -> llvm::Type*
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [&] (global_ctx_t::type_def_t const& t)
                {
                    return t.llvm_type;
                });
        },
        [&] (typecheck::expr_t::app_t const& app) -> llvm::Type*
        {
            return match(
                app.func.get().value,
                [&] (typecheck::expr_t::true_t const&) -> llvm::Type*
                {
                    return llvm::StructType::get(global.llvm_ctx);
                },
                [&] (typecheck::expr_t::ref_t const&) -> llvm::Type*
                {
                    return gen_type(global, app.args[0])->getPointerTo();
                },
                [&] (typecheck::expr_t::array_t const&) -> llvm::Type*
                {
                    auto const properties = get_array_properties(type);
                    return gen_type(global, properties.element_type)->getPointerTo();
                },
                [] (auto const&) -> llvm::Type*
                {
                    assert(false and "cannot generate a type for application of anything except true_t and array_t");
                    __builtin_unreachable();
                });
        },
        [] (typecheck::expr_t::abs_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for a function abstraction");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::pi_t const& x) -> llvm::Type*
        {
            auto proto = llvm_func_proto_t::from_pi(x);
            assert(proto and "can only generate an llvm type for 1st order function types");
            return gen_func_type(global, *proto)->getPointerTo();
        },
        [&] (typecheck::expr_t::sigma_t const& x) -> llvm::Type*
        {
            auto const& element_types = fmap(x.args, [&] (auto const& arg) { return gen_type(global, arg.type); });
            return llvm::StructType::get(global.llvm_ctx, element_types);
        },
        [] (typecheck::expr_t::ref_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for ref_t expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::scope_t const&) -> llvm::Type*
        {
            return llvm::StructType::get(global.llvm_ctx);
        },
        [] (typecheck::expr_t::addressof_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for addressof_t expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::deref_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for deref_t expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::scopeof_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for scopeof_t expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::array_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an array_t expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::init_list_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for an initializer list");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::member_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for member expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::subscript_t const&) -> llvm::Type*
        {
            assert(false and "cannot generate a type for subscript expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::because_t const& x) -> llvm::Type*
        {
            return gen_type(global, x.value.get());
        });
}

pass_by_ptr_result_t pass_by_ptr(global_ctx_t const& global, typecheck::expr_t const& type)
{
    using namespace pass_by_ptr_result;
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::true_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::auto_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::bool_t const&) -> pass_by_ptr_result_t
        {
            return no_t{};
        },
        [] (typecheck::expr_t::cstr_t const&) -> pass_by_ptr_result_t
        {
            // `gen_type()` returns a pointer to i8 but the pointer *is* the value!
            return no_t{};
        },
        [] (typecheck::expr_t::unit_t const&) -> pass_by_ptr_result_t
        {
            return no_t{};
        },
        [] (typecheck::expr_t::i8_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::i16_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::i32_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::i64_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::u8_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::u16_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::u32_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::u64_t const&) -> pass_by_ptr_result_t { return no_t{}; },
        [] (typecheck::expr_t::boolean_constant_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::numeric_constant_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::string_literal_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::boolean_expr_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::relation_expr_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::arith_expr_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::var_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::global_t const& g) -> pass_by_ptr_result_t
        {
            auto const val = global[g];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm_func_t const&) -> pass_by_ptr_result_t
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [] (global_ctx_t::type_def_t const& t) -> pass_by_ptr_result_t
                {
                    return match(
                        t.def.value,
                        [] (typecheck::type_def_t::integer_t const& integer) -> pass_by_ptr_result_t
                        {
                            return no_t{};
                        },
                        [] (typecheck::type_def_t::struct_t const& x) -> pass_by_ptr_result_t
                        {
                            // TODO should it be pass-by-value if empty struct? same would apply for tuples
                            return pass_by_ptr_result::struct_t{x.fields};
                        });
                });
        },
        [&] (typecheck::expr_t::app_t const& app) -> pass_by_ptr_result_t
        {
            return match(
                app.func.get().value,
                [] (typecheck::expr_t::true_t const&) -> pass_by_ptr_result_t
                {
                    return no_t{};
                },
                [] (typecheck::expr_t::ref_t const&) -> pass_by_ptr_result_t
                {
                    return no_t{}; // the pointer *is* the value!
                },
                [&] (typecheck::expr_t::array_t const&) -> pass_by_ptr_result_t
                {
                    return pass_by_ptr_result::array_t{get_array_properties(type)};
                },
                [] (auto const&) -> pass_by_ptr_result_t
                {
                    assert(false and "expression is not a type");
                    __builtin_unreachable();
                });
        },
        [] (typecheck::expr_t::abs_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::pi_t const& x) -> pass_by_ptr_result_t
        {
            auto proto = llvm_func_proto_t::from_pi(x);
            assert(proto and "can only pass 1st order function types");
            // `gen_type()` returns a function pointer but the pointer *is* the value!
            return no_t{};
        },
        [&] (typecheck::expr_t::sigma_t const& x) -> pass_by_ptr_result_t
        {
            return pass_by_ptr_result::sigma_t{x.args};
        },
        [] (typecheck::expr_t::ref_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::scope_t const&) -> pass_by_ptr_result_t
        {
            return no_t{};
        },
        [] (typecheck::expr_t::addressof_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::deref_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::scopeof_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::array_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::init_list_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::member_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::subscript_t const&) -> pass_by_ptr_result_t
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::because_t const& x) -> pass_by_ptr_result_t
        {
            return pass_by_ptr(global, x.value.get());
        });
}

bool is_pass_by_ptr(global_ctx_t const& global, typecheck::expr_t const& x)
{
    return not std::holds_alternative<pass_by_ptr_result::no_t>(pass_by_ptr(global, x));
}

bool is_boxed(typecheck::expr_t const& type)
{
    // Currently only arrays are boxed, but most likely closures will be boxed too.
    return is_array(type);
}

bool is_trivially_destructible(global_ctx_t const& global, typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::typename_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::true_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::auto_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::bool_t const&)
        {
            return true;
        },
        [] (typecheck::expr_t::cstr_t const&)
        {
            return true;
        },
        [] (typecheck::expr_t::unit_t const&)
        {
            return true;
        },
        [] (typecheck::expr_t::i8_t const&) { return true; },
        [] (typecheck::expr_t::i16_t const&) { return true; },
        [] (typecheck::expr_t::i32_t const&) { return true; },
        [] (typecheck::expr_t::i64_t const&) { return true; },
        [] (typecheck::expr_t::u8_t const&) { return true; },
        [] (typecheck::expr_t::u16_t const&) { return true; },
        [] (typecheck::expr_t::u32_t const&) { return true; },
        [] (typecheck::expr_t::u64_t const&) { return true; },
        [] (typecheck::expr_t::boolean_constant_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::numeric_constant_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::string_literal_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::boolean_expr_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::relation_expr_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::arith_expr_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::var_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::global_t const& g)
        {
            auto const val = global[g];
            assert(val and "unknown type");
            return match(
                *val,
                [] (llvm_func_t const&) -> bool
                {
                    assert(false and "found a function but was expecting a type");
                    __builtin_unreachable();
                },
                [&] (global_ctx_t::type_def_t const& t)
                {
                    return match(
                        t.def.value,
                        [] (typecheck::type_def_t::integer_t const& integer)
                        {
                            return true;
                        },
                        [&] (typecheck::type_def_t::struct_t const& s)
                        {
                            return std::ranges::all_of(
                                s.fields,
                                [&] (typecheck::type_def_t::struct_t::field_t const& f)
                                {
                                    return not is_boxed(f.type) and is_trivially_destructible(global, f.type);
                                });
                        });
                });
        },
        [&] (typecheck::expr_t::app_t const& app)
        {
            return match(
                app.func.get().value,
                [] (typecheck::expr_t::true_t const&) { return true; },
                [] (typecheck::expr_t::ref_t const&) { return true; },
                [&] (typecheck::expr_t::array_t const&)
                {
                    // TODO currently only arrays are boxed and arrays of arrays are still arrays,
                    // so for now it is sufficient to check that element_type is trivially destructible;
                    // in other words, currently an array cannot contain a boxed type,
                    // but when we add more boxed types (eg closures) we need to check that element_type is not boxed
                    return is_trivially_destructible(global, get_array_properties(type).element_type);
                },
                [] (auto const&) -> bool
                {
                    assert(false and "expression is not a type");
                    __builtin_unreachable();
                });
        },
        [] (typecheck::expr_t::abs_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::pi_t const& x)
        {
            // TODO this will change once we introduce closures
            return true;
        },
        [&] (typecheck::expr_t::sigma_t const& x)
        {
            return std::ranges::all_of(
                x.args,
                [&] (typecheck::func_arg_t const& arg)
                {
                    return not is_boxed(arg.type) and is_trivially_destructible(global, arg.type);
                });
        },
        [] (typecheck::expr_t::ref_t const&) { return true; },
        [] (typecheck::expr_t::scope_t const&) { return true; },
        [] (typecheck::expr_t::addressof_t const&) { return true; },
        [] (typecheck::expr_t::deref_t const&) { return true; },
        [] (typecheck::expr_t::scopeof_t const&) { return true; },
        [] (typecheck::expr_t::array_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::init_list_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::member_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::subscript_t const&) -> bool
        {
            assert(false and "expression is not a type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::because_t const& x)
        {
            // this is a type expression, eg `i32_t because true`
            return is_trivially_destructible(global, x.value.get());
        });
}

} // namespace dep0::llvmgen
