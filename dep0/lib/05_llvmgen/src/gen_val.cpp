/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/gen_val.hpp"

#include "private/first_order_types.hpp"
#include "private/gen_alloca.hpp"
#include "private/gen_array.hpp"
#include "private/gen_attrs.hpp"
#include "private/gen_body.hpp"
#include "private/gen_builtin.hpp"
#include "private/gen_func.hpp"
#include "private/gen_loop.hpp"
#include "private/gen_type.hpp"
#include "private/proto.hpp"

#include "dep0/ast/find_member_field.hpp"
#include "dep0/typecheck/list_initialization.hpp"
#include "dep0/typecheck/subscript_access.hpp"

#include "dep0/match.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <boost/hana.hpp>

#include <algorithm>
#include <ranges>

namespace dep0::llvmgen {

/** Return true if the given type is a signed integral; false otherwise. */
static bool is_signed_integral(global_ctx_t const& ctx, typecheck::expr_t const& type)
{
    return match(
        type.value,
        [] (typecheck::expr_t::i8_t const&) { return true; },
        [] (typecheck::expr_t::i16_t const&) { return true; },
        [] (typecheck::expr_t::i32_t const&) { return true; },
        [] (typecheck::expr_t::i64_t const&) { return true; },
        [&] (typecheck::expr_t::global_t const& g)
        {
            if (auto const type_def = std::get_if<global_ctx_t::type_def_t>(ctx[g]))
                return match(
                    type_def->def.value,
                    [] (typecheck::type_def_t::integer_t const& integer)
                    {
                        return integer.sign == ast::sign_t::signed_v;
                    },
                    [] (typecheck::type_def_t::struct_t const&) { return false; });
            else
                return false;
        },
        [] (auto const&) { return false; });
}

static bool needs_escaping(std::string_view const s)
{
    return s.find('\\') != s.npos;
}

static std::string escape(std::string_view const in)
{
    std::string out;
    auto const n = in.size();
    out.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        out.push_back([&]
        {
            auto const c = in[i];
            if (c == '\\' and i+1 < n)
                switch (in[i+1])
                {
                    case 'b': ++i; return '\b';
                    case 'f': ++i; return '\f';
                    case 'n': ++i; return '\n';
                    case 'r': ++i; return '\r';
                    case 't': ++i; return '\t';
                    case 'v': ++i; return '\v';
                    case '"': ++i; return '"';
                    case '\\': ++i; return '\\';
                }
            return c;
        }());
    return out;
}

static allocator_t select_allocator(value_category_t const value_category)
{
    return value_category == value_category_t::result ? allocator_t::heap : allocator_t::stack;
}

llvm::Value* gen_val_unit(global_ctx_t& global)
{
    return llvm::ConstantAggregateZero::get(llvm::StructType::get(global.llvm_ctx));
}

llvm::Value* gen_val(llvm::IntegerType* const type, boost::multiprecision::cpp_int const& x)
{
    return llvm::ConstantInt::get(type, x.str(), 10);
}

llvm::Value* gen_val(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& expr,
    value_category_t const value_category,
    llvm::Value* const dest)
{
    auto const& type = std::get<typecheck::expr_t>(expr.properties.sort.get());
    auto const maybe_store = [&] (llvm::Value* const value)
    {
        if (dest)
            gen_store(global, local, builder, value_category, value, dest, type);
        return value;
    };
    return match(
        expr.value,
        [] (typecheck::expr_t::typename_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a typename");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::true_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for true_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::auto_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for auto expression");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::bool_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for bool_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::cstr_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for cstr_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::unit_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for unit_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i8_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i8_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i16_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i16_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i32_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i32_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::i64_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for i64_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u8_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u8_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u16_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u16_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u32_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u32_t");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::u64_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for u64_t");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::boolean_constant_t const& x) -> llvm::Value*
        {
            return maybe_store(llvm::ConstantInt::getBool(global.llvm_ctx, x.value));
        },
        [&] (typecheck::expr_t::numeric_constant_t const& x) -> llvm::Value*
        {
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, type));
            assert(llvm_type);
            return maybe_store(gen_val(llvm_type, x.value));
        },
        [&] (typecheck::expr_t::string_literal_t const& x) -> llvm::Value*
        {
            std::optional<std::string> escaped;
            auto const& raw = x.value.view();
            if (needs_escaping(raw))
                escaped.emplace(escape(raw));
            auto const& s = escaped ? *escaped : raw;
            auto val = global.get_string_literal(s);
            if (not val)
            {
                val = builder.CreateGlobalStringPtr(s, "$_str_");
                global.store_string_literal(escaped ? std::move(*escaped) : std::string(s), val);
            }
            return maybe_store(val);
        },
        [&] (typecheck::expr_t::boolean_expr_t const& x) -> llvm::Value*
        {
            return maybe_store(match(
                x.value,
                [&] (typecheck::expr_t::boolean_expr_t::not_t const& x) -> llvm::Value*
                {
                    return builder.CreateNot(gen_temporary_val(global, local, builder, x.expr.get()));
                },
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    auto const lhs_val = gen_temporary_val(global, local, builder, x.lhs.get());
                    auto const rhs_val = gen_temporary_val(global, local, builder, x.rhs.get());
                    return boost::hana::overload(
                        [&] (boost::hana::type<typecheck::expr_t::boolean_expr_t::and_t>)
                        {
                            return builder.CreateAnd(lhs_val, rhs_val);
                        },
                        [&] (boost::hana::type<typecheck::expr_t::boolean_expr_t::or_t>)
                        {
                            return builder.CreateOr(lhs_val, rhs_val);
                        })(boost::hana::type_c<T>);
                }));
        },
        [&] (typecheck::expr_t::relation_expr_t const& x) -> llvm::Value*
        {
            return maybe_store(match(
                x.value,
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    using enum dep0::ast::sign_t;
                    using enum llvm::CmpInst::Predicate;
                    auto const lhs_val = gen_temporary_val(global, local, builder, x.lhs.get());
                    auto const rhs_val = gen_temporary_val(global, local, builder, x.rhs.get());
                    auto const sign =
                        match(
                            std::get<typecheck::expr_t>(x.lhs.get().properties.sort.get()).value,
                            [] (typecheck::expr_t::bool_t const&) { return dep0::ast::sign_t::unsigned_v; },
                            [] (typecheck::expr_t::u8_t const&) { return dep0::ast::sign_t::unsigned_v; },
                            [] (typecheck::expr_t::u16_t const&) { return dep0::ast::sign_t::unsigned_v; },
                            [] (typecheck::expr_t::u32_t const&) { return dep0::ast::sign_t::unsigned_v; },
                            [] (typecheck::expr_t::u64_t const&) { return dep0::ast::sign_t::unsigned_v; },
                            [&global] (typecheck::expr_t::global_t const& g)
                            {
                                auto const t = std::get_if<global_ctx_t::type_def_t>(global[g]);
                                assert(t and "unknown global or not a typedef");
                                auto const integer = std::get_if<typecheck::type_def_t::integer_t>(&t->def.value);
                                assert(integer and "relation expr can only be applied to integer types");
                                return integer->sign;
                            },
                            [] (auto const&) { return dep0::ast::sign_t::signed_v; });
                    auto const op =
                        boost::hana::overload(
                            [] (boost::hana::type<typecheck::expr_t::relation_expr_t::eq_t>)
                            {
                                return ICMP_EQ;
                            },
                            [] (boost::hana::type<typecheck::expr_t::relation_expr_t::neq_t>)
                            {
                                return ICMP_NE;
                            },
                            [sign] (boost::hana::type<typecheck::expr_t::relation_expr_t::gt_t>)
                            {
                                return sign == signed_v ? ICMP_SGT : ICMP_UGT;
                            },
                            [sign] (boost::hana::type<typecheck::expr_t::relation_expr_t::gte_t>)
                            {
                                return sign == signed_v ? ICMP_SGE : ICMP_UGE;
                            },
                            [sign] (boost::hana::type<typecheck::expr_t::relation_expr_t::lt_t>)
                            {
                                return sign == signed_v ? ICMP_SLT : ICMP_ULT;
                            },
                            [sign] (boost::hana::type<typecheck::expr_t::relation_expr_t::lte_t>)
                            {
                                return sign == signed_v ? ICMP_SLE : ICMP_ULE;
                            })(boost::hana::type_c<T>);
                    return builder.CreateCmp(op, lhs_val, rhs_val);
                }));
        },
        [&] (typecheck::expr_t::arith_expr_t const& x) -> llvm::Value*
        {
            return maybe_store(match(
                x.value,
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    auto const lhs = gen_temporary_val(global, local, builder, x.lhs.get());
                    auto const rhs = gen_temporary_val(global, local, builder, x.rhs.get());
                    return boost::hana::overload(
                        [&] (boost::hana::type<typecheck::expr_t::arith_expr_t::plus_t>)
                        {
                            return builder.CreateAdd(lhs, rhs);
                        },
                        [&] (boost::hana::type<typecheck::expr_t::arith_expr_t::minus_t>)
                        {
                            return builder.CreateSub(lhs, rhs);
                        },
                        [&] (boost::hana::type<typecheck::expr_t::arith_expr_t::mult_t>)
                        {
                            return builder.CreateMul(lhs, rhs);
                        },
                        [&] (boost::hana::type<typecheck::expr_t::arith_expr_t::div_t>)
                        {
                            return is_signed_integral(global, type)
                                ? builder.CreateSDiv(lhs, rhs)
                                : builder.CreateUDiv(lhs, rhs);
                        })(boost::hana::type_c<T>);
                }));
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var];
            assert(val and "unknown variable");
            return maybe_store(match(
                *val,
                [] (llvm::Value* const p) { return p; },
                [] (llvm_func_t const& c) { return c.func; }));
        },
        [&] (typecheck::expr_t::global_t const& g) -> llvm::Value*
        {
            auto const val = global[g];
            assert(val and "unknown global");
            return maybe_store(match(
                *val,
                [] (llvm_func_t const& c) { return c.func; },
                [] (global_ctx_t::type_def_t const&) -> llvm::Value*
                {
                    assert(false and "found a typedef but was expecting a value");
                    __builtin_unreachable();
                }));
        },
        [&] (typecheck::expr_t::app_t const& x) -> llvm::Value*
        {
            if (auto const abs = std::get_if<typecheck::expr_t::abs_t>(&x.func.get().value))
                if (x.args.empty())
                {
                    // found an immediately-invoked-lambda: inline it!
                    if (abs->body.stmts.empty())
                        // if body is empty, gen_body produces an open block, which breaks MergeBlockIntoPredecessor;
                        // but this is only possible if return type is unit_t, so just generate the unit value
                        return maybe_store(gen_val_unit(global));
                    auto const current_func = builder.GetInsertBlock()->getParent();
                    auto const gen_inlined_body = [&] (inlined_result_t const inlined_result)
                    {
                        auto snippet = gen_body(global, local, abs->body, "inlined", current_func, inlined_result);
                        builder.CreateBr(snippet.entry_block);
                        // MergeBlockIntoPredecessor requires a valid block, so seal everything before calling it
                        auto const next_block = llvm::BasicBlock::Create(global.llvm_ctx, "cont", current_func);
                        snippet.seal_open_blocks(
                            builder,
                            [&] (llvm::IRBuilder<>& builder)
                            {
                                builder.CreateBr(next_block);
                            });
                        assert(not snippet.entry_block->empty() and snippet.entry_block->back().isTerminator());
                        bool const merged = llvm::MergeBlockIntoPredecessor(snippet.entry_block);
                        assert(merged and "llvm could not merge inlined entry block");
                        builder.SetInsertPoint(next_block);
                    };
                    auto const& ret_type = abs->ret_type.get();
                    auto const allocator = select_allocator(value_category);
                    auto const dest2 =
                        dest ? dest
                        : is_pass_by_val(global, ret_type) ? nullptr
                        : gen_alloca(global, local, builder, allocator, ret_type);
                    if (dest2)
                    {
                        gen_inlined_body({value_category, dest2});
                        return dest2;
                    }
                    else
                    {
                        // if we get here return type is pass-by-value
                        assert(is_pass_by_val(global, ret_type));
                        auto const inlined_type = gen_type(global, ret_type);
                        auto const inlined_result = builder.CreateAlloca(inlined_type, builder.getInt32(1));
                        gen_inlined_body({value_category, inlined_result});
                        return builder.CreateLoad(inlined_type, inlined_result);
                    }
                }
            return gen_func_call(global, local, builder, x, value_category, dest);
        },
        [&] (typecheck::expr_t::abs_t const& abs) -> llvm::Value*
        {
            auto const proto = llvm_func_proto_t::from_abs(abs);
            assert(proto and "can only generate a value for a 1st order function type");
            return maybe_store(gen_func(global, *proto, abs));
        },
        [&] (typecheck::expr_t::pi_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a pi-type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::sigma_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a sigma-type");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::array_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for an array_t expression");
            __builtin_unreachable();
        },
        [&] (typecheck::expr_t::init_list_t const& x) -> llvm::Value*
        {
            return match(
                typecheck::is_list_initializable(global.env(), type),
                [&] (typecheck::is_list_initializable_result::no_t) -> llvm::Value*
                {
                    // We found an initializer list whose type is not trivially initializiable from a list.
                    // This either means that typechecking is broken or, more optimistically,
                    // that some reasoning was required to prove that the type was initializiable from a list.
                    // One such case is `true_t(cond)` for some complicated condition.
                    // We are not going to prove again that the condition was actually true,
                    // so for now just construct a value of the empty struct.
                    // If later on we add more cases like this, we might have to inspect the type.
                    return llvm::ConstantAggregateZero::get(gen_type(global, type));
                },
                [&] (typecheck::is_list_initializable_result::unit_t) -> llvm::Value*
                {
                    return gen_val_unit(global);
                },
                [&] (typecheck::is_list_initializable_result::true_t) -> llvm::Value*
                {
                    return llvm::ConstantAggregateZero::get(gen_type(global, type));
                },
                [&] (typecheck::is_list_initializable_result::struct_t s) -> llvm::Value*
                {
                    auto const llvm_type = gen_type(global, type);
                    auto const allocator = select_allocator(value_category);
                    auto const dest2 = dest ? dest : gen_alloca(global, local, builder, allocator, type);
                    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
                    auto const zero = llvm::ConstantInt::get(int32, 0);
                    auto ctx = local.extend();
                    for (auto const i: std::views::iota(0ul, x.values.size()))
                    {
                        auto const index = llvm::ConstantInt::get(int32, i);
                        auto const element_ptr = builder.CreateGEP(llvm_type, dest2, {zero, index});
                        if (is_boxed(s.fields[i].type))
                        {
                            auto const p = gen_alloca(global, ctx, builder, allocator, s.fields[i].type);
                            gen_val(global, ctx, builder, x.values[i], value_category, p);
                            builder.CreateStore(p, element_ptr);
                            ctx.try_emplace(s.fields[i].var, p);
                        }
                        else
                        {
                            auto const val = gen_val(global, ctx, builder, x.values[i], value_category, element_ptr);
                            ctx.try_emplace(s.fields[i].var, val);
                        }
                    }
                    std::ranges::copy(ctx.destructors, std::back_inserter(local.destructors));
                    ctx.destructors.clear();
                    return dest2;
                },
                [&] (typecheck::is_list_initializable_result::sigma_const_t sigma) -> llvm::Value*
                {
                    auto const llvm_type = gen_type(global, type);
                    auto const allocator = select_allocator(value_category);
                    auto const dest2 = dest ? dest : gen_alloca(global, local, builder, allocator, type);
                    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
                    auto const zero = llvm::ConstantInt::get(int32, 0);
                    auto sigma_ctx = local.extend();
                    for (auto const i: std::views::iota(0ul, x.values.size()))
                    {
                        auto const index = llvm::ConstantInt::get(int32, i);
                        auto const element_ptr = builder.CreateGEP(llvm_type, dest2, {zero, index});
                        if (is_boxed(sigma.args[i].type))
                        {
                            auto const p = gen_alloca(global, sigma_ctx, builder, allocator, sigma.args[i].type);
                            gen_val(global, sigma_ctx, builder, x.values[i], value_category, p);
                            builder.CreateStore(p, element_ptr);
                            if (sigma.args[i].var)
                                sigma_ctx.try_emplace(*sigma.args[i].var, p);
                        }
                        else
                        {
                            auto const val = gen_val(global, sigma_ctx, builder, x.values[i], value_category, element_ptr);
                            if (sigma.args[i].var)
                                sigma_ctx.try_emplace(*sigma.args[i].var, val);
                        }
                    }
                    std::ranges::copy(sigma_ctx.destructors, std::back_inserter(local.destructors));
                    sigma_ctx.destructors.clear();
                    return dest2;
                },
                [&] (typecheck::is_list_initializable_result::array_const_t) -> llvm::Value*
                {
                    auto const properties = get_array_properties(type);
                    auto const allocator = select_allocator(value_category);
                    auto const dest2 = dest ? dest : gen_alloca(global, local, builder, allocator, type);
                    auto const element_type = gen_type(global, properties.element_type);
                    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
                    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx); // TODO we use u64_t to typecheck arrays
                    for (auto const i: std::views::iota(0ul, x.values.size()))
                    {
                        auto const index = llvm::ConstantInt::get(int32, i);
                        auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
                        auto const element_ptr = builder.CreateGEP(element_type, dest2, offset);
                        // TODO currently only arrays are boxed and arrays of arrays are still arrays;
                        // but when we add more boxed types (eg closures) we will need to allocate their box
                        gen_val(global, local, builder, x.values[i], value_category, element_ptr);
                    }
                    return dest2;
                });
        },
        [&] (typecheck::expr_t::member_t const& member) -> llvm::Value*
        {
            auto const& object_type = std::get<typecheck::expr_t>(member.object.get().properties.sort.get());
            auto const& g = std::get<typecheck::expr_t::global_t>(object_type.value);
            auto const type_def = std::get_if<global_ctx_t::type_def_t>(global[g]);
            assert(type_def and "only global structs can have member access");
            auto const& s = std::get<typecheck::type_def_t::struct_t>(type_def->def.value);
            auto const i = ast::find_member_index<typecheck::properties_t>(member.field, s);
            assert(i.has_value());
            auto const struct_type = gen_type(global, object_type);
            auto const base = gen_temporary_val(global, local, builder, member.object.get());
            auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
            auto const zero = llvm::ConstantInt::get(int32, 0);
            auto const index = llvm::ConstantInt::get(int32, *i);
            auto const ptr = builder.CreateGEP(struct_type, base, {zero, index});
            auto const element_type = gen_type(global, s.fields[*i].type); // TODO this is wrong; needs substitutions!
            return maybe_store(
                is_boxed(type) or is_pass_by_val(global, type)
                ? builder.CreateLoad(element_type, ptr)
                : ptr);
        },
        [&] (typecheck::expr_t::subscript_t const& subscript) -> llvm::Value*
        {
            auto const& object_type = std::get<typecheck::expr_t>(subscript.object.get().properties.sort.get());
            return match(
                typecheck::has_subscript_access(object_type),
                [] (typecheck::has_subscript_access_result::no_t) -> llvm::Value*
                {
                    assert(false and "unexpected subscript expression; typechecking must be broken");
                    __builtin_unreachable();
                },
                [&] (typecheck::has_subscript_access_result::sigma_t const& sigma) -> llvm::Value*
                {
                    auto const tuple_type = gen_type(global, object_type);
                    auto const base = gen_temporary_val(global, local, builder, subscript.object.get());
                    auto const index = std::get_if<typecheck::expr_t::numeric_constant_t>(&subscript.index.get().value);
                    assert(index and "subscript operand on tuples must be a numeric literal");
                    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx);
                    auto const zero = llvm::ConstantInt::get(int32, 0);
                    auto const i = index->value.convert_to<std::int32_t>();
                    auto const index_val = llvm::ConstantInt::get(int32, i);
                    auto const ptr = builder.CreateGEP(tuple_type, base, {zero, index_val});
                    auto const element_type = gen_type(global, sigma.args[i].type);
                    return maybe_store(
                        is_boxed(type) or is_pass_by_val(global, type)
                        ? builder.CreateLoad(element_type, ptr)
                        : ptr);
                },
                [&] (typecheck::has_subscript_access_result::array_t const&) -> llvm::Value*
                {
                    auto const& array = subscript.object.get();
                    auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
                    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
                    auto const element_type = gen_type(global, properties.element_type);
                    auto const base = gen_temporary_val(global, local, builder, array);
                    auto const index = gen_temporary_val(global, local, builder, subscript.index.get());
                    auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
                    auto const ptr = builder.CreateGEP(element_type, base, offset);
                    return maybe_store(is_pass_by_ptr(global, type) ? ptr : builder.CreateLoad(element_type, ptr));
                });
        },
        [&] (typecheck::expr_t::because_t const& x) -> llvm::Value*
        {
            return gen_val(global, local, builder, x.value.get(), value_category, dest);
        });
}

void gen_store(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    value_category_t const value_category,
    llvm::Value* const value,
    llvm::Value* const dest,
    typecheck::expr_t const& type)
{
    assert(dest and "gen_store() destination cannot be nullptr");
    assert(dest->getType()->isPointerTy() and "gen_store() destination must be a pointer");
    match(
        pass_by_ptr(global, type),
        [&] (pass_by_ptr_result::no_t)
        {
            builder.CreateStore(value, dest);
        },
        [&] (pass_by_ptr_result::struct_t const& s)
        {
            if (is_trivially_copyable(global, type))
            {
                auto const& data_layout = global.llvm_module.getDataLayout();
                auto const pointed_type = gen_type(global, type);
                auto const align = data_layout.getPrefTypeAlign(pointed_type);
                auto const bytes = data_layout.getTypeAllocSize(pointed_type);
                builder.CreateMemCpy(dest, align, value, align, builder.getInt64(bytes.getFixedSize()));
            }
            else
            {
                auto const gep =
                    [&, llvm_type=gen_type(global, type), zero=builder.getInt32(0)]
                    (llvm::Value* const p, std::size_t const i)
                    {
                        return builder.CreateGEP(llvm_type, p, {zero, builder.getInt32(i)});
                    };
                auto struct_ctx = local.extend();
                for (auto const i: std::views::iota(0ul, s.fields.size()))
                {
                    auto const& element_type = s.fields[i].type;
                    auto const element_ptr = gep(value, i);
                    auto const dest_element_ptr = gep(dest, i);
                    if (is_boxed(element_type))
                    {
                        auto const allocator = select_allocator(value_category);
                        auto const alloca = gen_alloca(global, struct_ctx, builder, allocator, element_type);
                        builder.CreateStore(alloca, dest_element_ptr);
                        auto const element_value = builder.CreateLoad(gen_type(global, element_type), element_ptr);
                        gen_store(global, struct_ctx, builder, value_category, element_value, alloca, element_type);
                        struct_ctx.try_emplace(s.fields[i].var, alloca);
                    }
                    else if (is_pass_by_ptr(global, element_type))
                    {
                        gen_store(global, struct_ctx, builder, value_category, element_ptr, dest_element_ptr, element_type);
                        struct_ctx.try_emplace(s.fields[i].var, dest_element_ptr);
                    }
                    else
                    {
                        auto const element_value = builder.CreateLoad(gen_type(global, element_type), element_ptr);
                        builder.CreateStore(element_value, dest_element_ptr);
                        struct_ctx.try_emplace(s.fields[i].var, element_value);
                    }
                }
                std::ranges::copy(struct_ctx.destructors, std::back_inserter(local.destructors));
                struct_ctx.destructors.clear();
            }
        },
        [&] (pass_by_ptr_result::sigma_t const& sigma)
        {
            if (is_trivially_copyable(global, type))
            {
                auto const& data_layout = global.llvm_module.getDataLayout();
                auto const pointed_type = gen_type(global, type);
                auto const align = data_layout.getPrefTypeAlign(pointed_type);
                auto const bytes = data_layout.getTypeAllocSize(pointed_type);
                builder.CreateMemCpy(dest, align, value, align, builder.getInt64(bytes.getFixedSize()));
            }
            else
            {
                auto const gep =
                    [&, llvm_type=gen_type(global, type), zero=builder.getInt32(0)]
                    (llvm::Value* const p, std::size_t const i)
                    {
                        return builder.CreateGEP(llvm_type, p, {zero, builder.getInt32(i)});
                    };
                auto sigma_ctx = local.extend();
                for (auto const i: std::views::iota(0ul, sigma.args.size()))
                {
                    auto const& element_type = sigma.args[i].type;
                    auto const element_ptr = gep(value, i);
                    auto const dest_element_ptr = gep(dest, i);
                    if (is_boxed(element_type))
                    {
                        auto const allocator = select_allocator(value_category);
                        auto const alloca = gen_alloca(global, sigma_ctx, builder, allocator, element_type);
                        builder.CreateStore(alloca, dest_element_ptr);
                        auto const element_value = builder.CreateLoad(gen_type(global, element_type), element_ptr);
                        gen_store(global, sigma_ctx, builder, value_category, element_value, alloca, element_type);
                        if (sigma.args[i].var)
                            sigma_ctx.try_emplace(*sigma.args[i].var, alloca);
                    }
                    else if (is_pass_by_ptr(global, element_type))
                    {
                        gen_store(global, sigma_ctx, builder, value_category, element_ptr, dest_element_ptr, element_type);
                        if (sigma.args[i].var)
                            sigma_ctx.try_emplace(*sigma.args[i].var, dest_element_ptr);
                    }
                    else
                    {
                        auto const element_value = builder.CreateLoad(gen_type(global, element_type), element_ptr);
                        builder.CreateStore(element_value, dest_element_ptr);
                        if (sigma.args[i].var)
                            sigma_ctx.try_emplace(*sigma.args[i].var, element_value);
                    }
                }
                std::ranges::copy(sigma_ctx.destructors, std::back_inserter(local.destructors));
                sigma_ctx.destructors.clear();
            }
        },
        [&] (pass_by_ptr_result::array_t const& array)
        {
            // TODO do not crash if the array size is erased at runtime
            auto const size = gen_array_total_size(global, local, builder, array.properties);
            if (is_trivially_copyable(global, type))
            {
                auto const& data_layout = global.llvm_module.getDataLayout();
                auto const pointed_type = gen_type(global, array.properties.element_type);
                auto const align = data_layout.getPrefTypeAlign(pointed_type);
                auto const bytes = data_layout.getTypeAllocSize(pointed_type);
                auto const total_bytes = builder.CreateMul(size, builder.getInt64(bytes.getFixedSize()));
                builder.CreateMemCpy(dest, align, value, align, total_bytes);
            }
            else
            {
                gen_for_loop_upward(
                    global, local, builder,
                    ast::sign_t::unsigned_v,
                    builder.getInt64(0),
                    size,
                    [
                        &element_type=array.properties.element_type,
                        llvm_type=gen_type(global, array.properties.element_type),
                        value_category, value, dest
                    ]
                    (global_ctx_t& global, local_ctx_t& local, llvm::IRBuilder<>& builder, llvm::Value* const i)
                    {
                        auto const element_ptr = builder.CreateGEP(llvm_type, value, i);
                        auto const dest_element_ptr = builder.CreateGEP(llvm_type, dest, i);
                        gen_store(global, local, builder, value_category, element_ptr, dest_element_ptr, element_type);
                    });
            }
        });
};

llvm::Value* gen_func_call(
    global_ctx_t& global,
    local_ctx_t& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    value_category_t const value_category,
    llvm::Value* const dest)
{
    if (auto const result = try_gen_builtin(global, local, builder, app, value_category, dest))
        return result;
    assert(
        std::ranges::all_of(
            app.args,
            [] (typecheck::expr_t const& x)
            {
                return match(
                    x.properties.sort.get(),
                    [] (typecheck::expr_t const& type) { return is_first_order_type(type); },
                    [] (typecheck::kind_t) { return false; });
            })
        and "can only generate function call for 1st order applications");
    auto const pi_type = [&] () -> typecheck::expr_t::pi_t const&
    {
        auto const func_type = std::get_if<typecheck::expr_t>(&app.func.get().properties.sort.get());
        assert(func_type and "functions must be of sort type");
        auto const pi_type = std::get_if<typecheck::expr_t::pi_t>(&func_type->value);
        assert(pi_type and "functions must be pi-types");
        return *pi_type;
    }();
    auto const proto = [&] () -> llvm_func_proto_t
    {
        auto proto = llvm_func_proto_t::from_pi(pi_type);
        assert(proto and "can only generate a function call for 1st order function type");
        return std::move(proto.value());
    }();
    auto const llvm_func_type = gen_func_type(global, proto);
    auto const llvm_func = gen_temporary_val(global, local, builder, app.func.get());
    std::vector<llvm::Value*> llvm_args; // modifiable before generating the call
    auto const has_ret_arg = is_pass_by_ptr(global, proto.ret_type());
    auto const gen_call = [&, arg_offset = has_ret_arg ? 1ul : 0ul]
    {
        llvm_args.reserve(llvm_args.size() + proto.runtime_args().size());
        for (auto const i: std::views::iota(0ul, pi_type.args.size()))
            if (pi_type.args[i].qty > ast::qty_t::zero)
                llvm_args.push_back(gen_temporary_val(global, local, builder, app.args[i]));
        // this could be a direct call to a global function, or an indirect call via a function pointer
        auto const call = builder.CreateCall(llvm_func_type, llvm_func, std::move(llvm_args));
        for (auto const i: std::views::iota(0ul, proto.runtime_args().size()))
        {
            auto const& arg_type = proto.runtime_arg(i).type;
            if (auto const attr = get_sign_ext_attribute(global, arg_type); attr != llvm::Attribute::None)
                call->addParamAttr(i + arg_offset, attr);
        }
        return call;
    };
    auto const allocator = select_allocator(value_category);
    auto const result = [&] () -> llvm::Value*
    {
        if (has_ret_arg)
        {
            auto const dest2 = dest ? dest : gen_alloca(global, local, builder, allocator, proto.ret_type());
            llvm_args.push_back(dest2);
            gen_call();
            return dest2;
        }
        else if (dest)
        {
            builder.CreateStore(gen_call(), dest);
            return dest;
        }
        else
            return gen_call();
    }();
    // If the value constructed by the invoked function is used as a temporary in the enclosing scope,
    // we need to make sure that it is properly destructed when all destructors are invoked.
    // Conversely, if the constructed value is a result, it will be passed to the caller;
    // it is their responsibility to invoke the destructor when the returned value is no longer needed.
    if (value_category == value_category_t::temporary and not is_trivially_destructible(global, proto.ret_type()))
        local.destructors.emplace_back(result, proto.ret_type());
    return result;
}

} // namespace dep0::llvmgen
