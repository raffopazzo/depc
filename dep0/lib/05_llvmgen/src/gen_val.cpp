/*
 * Copyright Raffaele Rossi 2023 - 2024.
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
#include "private/gen_type.hpp"
#include "private/proto.hpp"

#include "dep0/typecheck/list_initialization.hpp"

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
            if (auto const type_def = std::get_if<typecheck::type_def_t>(ctx[g]))
                return match(
                    type_def->value,
                    [] (typecheck::type_def_t::integer_t const& integer)
                    {
                        return integer.sign == ast::sign_t::signed_v;
                    });
            else
                return false;
        },
        [] (auto const&) { return false; });
}

/**
 * Move the given instruction to the entry block of the given function.
 * If the instruction is already in its entry block, this function does nothing.
 */
static void move_to_entry_block(llvm::Instruction* const i, llvm::Function* const f)
{
    auto& bb = f->getEntryBlock();
    if (&bb == i->getParent())
        // already in entry block
        return;
    if (auto const t = f->getEntryBlock().getTerminator())
        i->moveBefore(t);
    else
        i->moveAfter(&bb.back());
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
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& expr,
    llvm::Value* const dest)
{
    auto const& expr_type = std::get<typecheck::expr_t>(expr.properties.sort.get());
    auto const storeOrReturn = [&] (llvm::Value* const value)
    {
        return maybe_gen_store(global, local, builder, value, dest, expr_type);
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
            return storeOrReturn(llvm::ConstantInt::getBool(global.llvm_ctx, x.value));
        },
        [&] (typecheck::expr_t::numeric_constant_t const& x) -> llvm::Value*
        {
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, expr_type));
            assert(llvm_type);
            return storeOrReturn(gen_val(llvm_type, x.value));
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
            return storeOrReturn(val);
        },
        [&] (typecheck::expr_t::boolean_expr_t const& x) -> llvm::Value*
        {
            return storeOrReturn(match(
                x.value,
                [&] (typecheck::expr_t::boolean_expr_t::not_t const& x) -> llvm::Value*
                {
                    return builder.CreateNot(gen_val(global, local, builder, x.expr.get(), nullptr));
                },
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    auto const lhs_val = gen_val(global, local, builder, x.lhs.get(), nullptr);
                    auto const rhs_val = gen_val(global, local, builder, x.rhs.get(), nullptr);
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
            return storeOrReturn(match(
                x.value,
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    using enum dep0::ast::sign_t;
                    using enum llvm::CmpInst::Predicate;
                    auto const lhs_val = gen_val(global, local, builder, x.lhs.get(), nullptr);
                    auto const rhs_val = gen_val(global, local, builder, x.rhs.get(), nullptr);
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
                                auto const type_def = std::get_if<typecheck::type_def_t>(global[g]);
                                assert(type_def and "unknown global or not a typedef");
                                return match(
                                    type_def->value,
                                    [] (typecheck::type_def_t::integer_t const& integer) { return integer.sign; });
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
            return storeOrReturn(match(
                x.value,
                [&] <typename T> (T const& x) -> llvm::Value*
                {
                    auto const lhs = gen_val(global, local, builder, x.lhs.get(), nullptr);
                    auto const rhs = gen_val(global, local, builder, x.rhs.get(), nullptr);
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
                            return is_signed_integral(global, expr_type)
                                ? builder.CreateSDiv(lhs, rhs)
                                : builder.CreateUDiv(lhs, rhs);
                        })(boost::hana::type_c<T>);
                }));
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var];
            assert(val and "unknown variable");
            return storeOrReturn(match(
                *val,
                [] (llvm::Value* const p) { return p; },
                [] (llvm_func_t const& c) { return c.func; }));
        },
        [&] (typecheck::expr_t::global_t const& g) -> llvm::Value*
        {
            auto const val = global[g];
            assert(val and "unknown global");
            return storeOrReturn(match(
                *val,
                [] (llvm_func_t const& c) { return c.func; },
                [] (typecheck::type_def_t const&) -> llvm::Value*
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
                        return storeOrReturn(gen_val_unit(global));
                    auto const current_func = builder.GetInsertBlock()->getParent();
                    auto const gen_inlined_body = [&] (llvm::Value* const inlined_result)
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
                        bool const merged = llvm::MergeBlockIntoPredecessor(snippet.entry_block);
                        assert(merged and "llvm could not merge inlined entry block");
                        builder.SetInsertPoint(next_block);
                    };
                    auto const dest2 = dest ? dest : gen_alloca_if_needed(global, local, builder, abs->ret_type.get());
                    // Note, if we generated an alloca here,
                    // there is no need to move it to the entry block,
                    // because mem2reg will not be able to optimize it.
                    if (dest2)
                    {
                        gen_inlined_body(dest2);
                        return dest2;
                    }
                    else
                    {
                        auto const inlined_type = gen_type(global, abs->ret_type.get());
                        auto const inlined_result = builder.CreateAlloca(inlined_type, builder.getInt32(1));
                        move_to_entry_block(inlined_result, current_func);
                        gen_inlined_body(inlined_result);
                        return builder.CreateLoad(inlined_type, inlined_result);
                    }
                }
            return gen_func_call(global, local, builder, x, dest);
        },
        [&] (typecheck::expr_t::abs_t const& abs) -> llvm::Value*
        {
            auto const proto = llvm_func_proto_t::from_abs(abs);
            assert(proto and "can only generate a value for a 1st order function type");
            return storeOrReturn(gen_func(global, *proto, abs));
        },
        [&] (typecheck::expr_t::pi_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a pi-type");
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
                typecheck::is_list_initializable(expr_type),
                [&] (typecheck::is_list_initializable_result::no_t) -> llvm::Value*
                {
                    // We found an initializer list whose type is not trivially initializiable from a list.
                    // This either means that typechecking is broken or, more optimistically,
                    // that some reasoning was required to prove that the type was initializiable from a list.
                    // One such case is `true_t(cond)` for some complicated condition.
                    // We are not going to prove again that the condition was actually true,
                    // so for now just construct a value of the empty struct.
                    // If later on we add more cases like this, we might have to inspect the type.
                    return llvm::ConstantAggregateZero::get(gen_type(global, expr_type));
                },
                [&] (typecheck::is_list_initializable_result::unit_t) -> llvm::Value*
                {
                    return gen_val_unit(global);
                },
                [&] (typecheck::is_list_initializable_result::true_t) -> llvm::Value*
                {
                    return llvm::ConstantAggregateZero::get(gen_type(global, expr_type));
                },
                [&] (typecheck::is_list_initializable_result::array_t const&) -> llvm::Value*
                {
                    auto const properties = get_array_properties(expr_type);
                    auto const dest2 = dest ? dest : [&]
                    {
                        auto const total_size = gen_array_total_size(global, local, builder, properties);
                        auto const element_type = gen_type(global, properties.element_type);
                        return builder.CreateAlloca(element_type, total_size);
                    }();
                    auto const type = dest2->getType()->getPointerElementType();
                    auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
                    auto const int32 = llvm::Type::getInt32Ty(global.llvm_ctx); // TODO we use u64_t to typecheck arrays
                    for (auto const i: std::views::iota(0ul, x.values.size()))
                    {
                        auto const index = llvm::ConstantInt::get(int32, i);
                        auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
                        auto const p = builder.CreateGEP(type, dest2, offset);
                        gen_val(global, local, builder, x.values[i], p);
                    }
                    return dest2;
                });
        },
        [&] (typecheck::expr_t::subscript_t const& subscript) -> llvm::Value*
        {
            auto const& array = subscript.array.get();
            auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
            auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
            auto const type = gen_type(global, properties.element_type);
            auto const base = gen_val(global, local, builder, subscript.array.get(), nullptr);
            auto const index = gen_val(global, local, builder, subscript.index.get(), nullptr);
            auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
            auto const ptr = builder.CreateGEP(type, base, offset);
            return storeOrReturn(
                is_array(std::get<typecheck::expr_t>(expr.properties.sort.get()))
                    ? ptr
                    : builder.CreateLoad(type, ptr));
        },
        [&] (typecheck::expr_t::because_t const& x) -> llvm::Value*
        {
            return gen_val(global, local, builder, x.value.get(), dest);
        });
}

llvm::Value* maybe_gen_store(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    llvm::Value* const value,
    llvm::Value* const dest,
    typecheck::expr_t const& type)
{
    if (not dest)
        return value;
    if (auto const pty = get_properties_if_array(type))
    {
        assert(dest->getType()->isPointerTy() and "memcpy destination must be a pointer");
        auto const& data_layout = global.llvm_module.getDataLayout();
        auto const pointed_type = dest->getType()->getPointerElementType();
        auto const align = data_layout.getPrefTypeAlign(pointed_type);
        auto const bytes = data_layout.getTypeAllocSize(pointed_type);
        auto const size = gen_array_total_size(global, local, builder, *pty);
        auto const total_bytes = builder.CreateMul(size, builder.getInt64(bytes.getFixedSize()));
        builder.CreateMemCpy(dest, align, value, align, total_bytes);
    }
    else
        builder.CreateStore(value, dest);
    return dest;
};

llvm::Value* gen_func_call(
    global_ctx_t& global,
    local_ctx_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
    if (auto const result = try_gen_builtin(global, local, builder, app, dest))
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
    auto const llvm_func = gen_val(global, local, builder, app.func.get(), nullptr);
    std::vector<llvm::Value*> llvm_args; // modifiable before generating the call
    auto const has_ret_arg = is_alloca_needed(proto.ret_type());
    auto const gen_call = [&, arg_offset = has_ret_arg ? 1ul : 0ul]
    {
        llvm_args.reserve(llvm_args.size() + proto.runtime_args().size());
        for (auto const i: std::views::iota(0ul, pi_type.args.size()))
            if (pi_type.args[i].qty > ast::qty_t::zero)
                llvm_args.push_back(gen_val(global, local, builder, app.args[i], nullptr));
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
    if (has_ret_arg)
    {
        auto const dest2 = dest ? dest : gen_alloca_if_needed(global, local, builder, proto.ret_type());
        assert(dest2 and "function call needs an alloca for the return value but does not have one");
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
}

} // namespace dep0::llvmgen
