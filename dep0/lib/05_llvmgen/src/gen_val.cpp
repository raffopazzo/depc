#include "private/gen_val.hpp"

#include "private/first_order_types.hpp"
#include "private/gen_alloca.hpp"
#include "private/gen_array.hpp"
#include "private/gen_attrs.hpp"
#include "private/gen_body.hpp"
#include "private/gen_func.hpp"
#include "private/gen_type.hpp"
#include "private/proto.hpp"

#include "dep0/match.hpp"

#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <algorithm>
#include <ranges>

namespace dep0::llvmgen {

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

llvm::Value* gen_val(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t const& expr,
    llvm::Value* const dest)
{
    auto const storeOrReturn = [&] (llvm::Value* const value)
    {
        if (not dest)
            return value;
        if (auto const pty = get_properties_if_array(std::get<typecheck::expr_t>(expr.properties.sort.get())))
        {
            assert(dest->getType()->isPointerTy() and "memcpy destination must be a pointer");
            auto const& data_layout = global.llvm_module.getDataLayout();
            auto const ptr_element_type = dest->getType()->getPointerElementType();
            auto const align = data_layout.getPrefTypeAlign(ptr_element_type);
            auto const bytes = data_layout.getTypeAllocSize(ptr_element_type);
            auto const size = gen_array_total_size(global, local, builder, *pty);
            auto const total_bytes = builder.CreateMul(size, builder.getInt64(bytes.getFixedSize()));
            builder.CreateMemCpy(dest, align, value, align, total_bytes);
        }
        else
            builder.CreateStore(value, dest);
        return dest;
    };
    return match(
        expr.value,
        [] (typecheck::expr_t::typename_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for a typename");
            __builtin_unreachable();
        },
        [] (typecheck::expr_t::bool_t const&) -> llvm::Value*
        {
            assert(false and "cannot generate a value for bool_t");
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
            // TODO currently beta-delta normalization operates on the templated AST,
            // so it does not reduce inside `properties.sort`, which means we might fail to generate a type;
            // we should in fact check all usages of `properties.sort`
            auto const& type = std::get<typecheck::expr_t>(expr.properties.sort.get());
            auto const llvm_type = cast<llvm::IntegerType>(gen_type(global, local, type));
            assert(llvm_type);
            return storeOrReturn(llvm::ConstantInt::get(llvm_type, x.value.str(), 10));
        },
        [&] (typecheck::expr_t::arith_expr_t const& x) -> llvm::Value*
        {
            return storeOrReturn(match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t const& x) -> llvm::Value*
                {
                    // TODO add NUW/NSW but probably depends on decision whether typechecking `plus_t` requires proofs?
                    // use temporaries to make sure that LHS comes before RHS in the emitted IR
                    auto const lhs = gen_val(global, local, builder, x.lhs.get(), nullptr);
                    auto const rhs = gen_val(global, local, builder, x.rhs.get(), nullptr);
                    return builder.CreateAdd(lhs, rhs);
                }));
        },
        [&] (typecheck::expr_t::var_t const& var) -> llvm::Value*
        {
            auto const val = local[var];
            assert(val and "unknown variable");
            return storeOrReturn(match(
                *val,
                [] (llvm::Value* const p) { return p; },
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
                    auto const current_func = builder.GetInsertBlock()->getParent();
                    auto const gen_inlined_body = [&] (llvm::Value* const inlined_result)
                    {
                        auto snippet = gen_body(global, local, abs->body, "inlined", current_func, inlined_result);
                        builder.CreateBr(snippet.entry_block);
                        bool const merged = llvm::MergeBlockIntoPredecessor(snippet.entry_block);
                        assert(merged and "llvm could not merge inlined entry block");
                        auto const next_block = llvm::BasicBlock::Create(global.llvm_ctx, "cont", current_func);
                        snippet.seal_open_blocks(
                            builder,
                            [&] (llvm::IRBuilder<>& builder)
                            {
                                builder.CreateBr(next_block);
                            });
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
                        auto const inlined_type = gen_type(global, local, abs->ret_type.get());
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
            return storeOrReturn(gen_func(global, local, *proto, abs));
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
            auto const properties = get_array_properties(std::get<typecheck::expr_t>(expr.properties.sort.get()));
            auto const dest2 = dest ? dest : [&]
            {
                auto const total_size = gen_array_total_size(global, local, builder, properties);
                auto const element_type = gen_type(global, local, properties.element_type);
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
        },
        [&] (typecheck::expr_t::subscript_t const& subscript) -> llvm::Value*
        {
            auto const& array = subscript.array.get();
            auto const properties = get_array_properties(std::get<typecheck::expr_t>(array.properties.sort.get()));
            auto const stride_size = gen_stride_size_if_needed(global, local, builder, properties);
            auto const type = gen_type(global, local, properties.element_type);
            auto const base = gen_val(global, local, builder, subscript.array.get(), nullptr);
            auto const index = gen_val(global, local, builder, subscript.index.get(), nullptr);
            auto const offset = stride_size ? builder.CreateMul(stride_size, index) : index;
            auto const ptr = builder.CreateGEP(type, base, offset);
            return storeOrReturn(
                is_array(std::get<typecheck::expr_t>(expr.properties.sort.get()))
                    ? ptr
                    : builder.CreateLoad(type, ptr));
        });
}

llvm::Value* gen_func_call(
    global_context_t& global,
    local_context_t const& local,
    llvm::IRBuilder<>& builder,
    typecheck::expr_t::app_t const& app,
    llvm::Value* const dest)
{
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
    auto const proto = [&] () -> llvm_func_proto_t
    {
        auto const func_type = std::get_if<typecheck::expr_t>(&app.func.get().properties.sort.get());
        assert(func_type and "functions must be of sort type");
        auto const pi_type = std::get_if<typecheck::expr_t::pi_t>(&func_type->value);
        assert(pi_type and "functions must be pi-types");
        auto proto = llvm_func_proto_t::from_pi(*pi_type);
        assert(proto and "can only generate a function call for 1st order function type");
        return std::move(proto.value());
    }();
    auto const llvm_func_type = gen_func_type(global, local, proto);
    auto const llvm_func = gen_val(global, local, builder, app.func.get(), nullptr);
    std::vector<llvm::Value*> llvm_args; // modifiable before generating the call
    auto const has_ret_arg = is_alloca_needed(proto.ret_type());
    auto const gen_call = [&, arg_offset = has_ret_arg ? 1ul : 0ul]
    {
        llvm_args.reserve(llvm_args.size() + app.args.size());
        for (typecheck::expr_t const& arg: app.args)
            llvm_args.push_back(gen_val(global, local, builder, arg, nullptr));
        // this could be a direct call to a global function, or an indirect call via a function pointer
        auto const call = builder.CreateCall(llvm_func_type, llvm_func, std::move(llvm_args));
        for (auto const i: std::views::iota(0ul, app.args.size()))
        {
            auto const& arg_type = std::get<typecheck::expr_t>(app.args[i].properties.sort.get());
            if (auto const attr = get_sign_ext_attribute(local, arg_type); attr != llvm::Attribute::None)
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
