#pragma once

#include "dep0/testing/predicate.hpp"
#include "dep0/testing/failure.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include <boost/test/tools/assertion_result.hpp>

#include <optional>
#include <utility>

namespace dep0::llvmgen::testing {

// type predicates
boost::test_tools::predicate_result has_void_return_type(llvm::Function const&);

boost::test_tools::predicate_result is_bool(llvm::Argument const*, std::optional<std::string_view>);
boost::test_tools::predicate_result is_i32(llvm::Argument const*, std::optional<std::string_view>);
boost::test_tools::predicate_result is_u32(llvm::Argument const*, std::optional<std::string_view>);

boost::test_tools::predicate_result is_i32_type(llvm::Type const&);

template <dep0::testing::Predicate<llvm::Type> F_ret, typename... F_args>
boost::test_tools::predicate_result is_pointer_to_function(
    llvm::Type const* const type,
    std::tuple<F_args...> const& f_args,
    F_ret&& f_ret)
{
    if (not type)
        return dep0::testing::failure("type is null");
    if (not type->isPointerTy())
        return dep0::testing::failure("type is not a pointer");
    auto const fn_type = dyn_cast<llvm::FunctionType>(type->getPointerElementType());
    if (not fn_type)
        return dep0::testing::failure("type is not a pointer to function");
    auto constexpr N = sizeof...(F_args);
    if (fn_type->getNumParams() != N)
        return dep0::testing::failure("FunctionType has ", fn_type->getNumParams(), " arguments but was expecting ", N);
    if (auto const result = std::forward<F_ret>(f_ret)(*fn_type->getReturnType()); not result)
        return dep0::testing::failure("predicate failed for return type: ", result.message());
    auto result = boost::test_tools::predicate_result(true);
    [&] <std::size_t... Is> (std::index_sequence<Is...>)
    {
        ([&]
        {
            if (result)
                if (auto const tmp = std::get<Is>(f_args)(*fn_type->getParamType(Is)); not tmp)
                    result = dep0::testing::failure("predicate failed for argument at index ", Is, tmp.message());
        }(), ...);
    } (std::make_index_sequence<N>{});
    return result;
}

// instruction/value predicates and factories; sorted by name, predicate before the corresponding factories

template <dep0::testing::Predicate<llvm::Value> F1, dep0::testing::Predicate<llvm::Value> F2>
boost::test_tools::predicate_result is_add_of(llvm::Value const* const val, F1&& f1, F2&& f2)
{
    if (not val)
        return dep0::testing::failure("val is null");
    auto const op = dyn_cast<llvm::BinaryOperator>(val);
    if (not op)
        return dep0::testing::failure("not a binary operator");
    if (op->getOpcode() != llvm::Instruction::BinaryOps::Add)
        return dep0::testing::failure(
            "binary operator is not Add: ",
            static_cast<int>(llvm::Instruction::BinaryOps::Add),
            " != ",
            static_cast<int>(op->getOpcode()));
    if (op->getNumOperands() != 2)
        return dep0::testing::failure("addition does not have 2 operands but ", op->getNumOperands());
    if (auto const result = std::forward<F1>(f1)(*op->getOperand(0)); not result)
        return dep0::testing::failure("predicate has failed for 1st operand: ", result.message());
    if (auto const result = std::forward<F2>(f2)(*op->getOperand(1)); not result)
        return dep0::testing::failure("predicate has failed for 2nd operand: ", result.message());
    return true;
}

template <dep0::testing::Predicate<llvm::Value> F1, dep0::testing::Predicate<llvm::Value> F2>
inline auto add_of(F1&& f1, F2&& f2)
{
    return [f1=std::forward<F1>(f1), f2=std::forward<F2>(f2)] (llvm::Value const& val)
    {
        return is_add_of(&val, f1, f2);
    };
}

template <dep0::testing::Predicate<llvm::BranchInst> F>
boost::test_tools::predicate_result is_branch_of(llvm::Instruction const* const instr, F&& f)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const br = dyn_cast<llvm::BranchInst>(instr);
    if (not br)
        return dep0::testing::failure("not a branch instruction");
    return std::forward<F>(f)(*br);
}

template <dep0::testing::Predicate<llvm::CallInst> F>
boost::test_tools::predicate_result is_call_inst(llvm::Instruction const& instr, F&& f)
{
    auto const call = dyn_cast<llvm::CallInst>(&instr);
    if (not call)
        return dep0::testing::failure("not a call instruction");
    return std::forward<F>(f)(*call);
}

template <dep0::testing::Predicate<llvm::CallInst> F>
auto call_inst(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
    {
        auto const call = dyn_cast<llvm::CallInst>(&v);
        if (not call)
            return dep0::testing::failure("value is not a function call");
        return f(*call);
    };
}

template <typename T>
auto exactly(T const* const p)
{
    return [p] (T const& x) -> boost::test_tools::predicate_result
    {
        if (p != &x)
            return dep0::testing::failure(p, " != ", &x);
        else
            return true;
    };
}
template <
    dep0::testing::Predicate<llvm::Type> F_type,
    dep0::testing::Predicate<llvm::Value> F_ptr,
    dep0::testing::Predicate<llvm::Value> F_idx>
boost::test_tools::predicate_result is_gep(llvm::Instruction const* v, F_type&& f_type, F_ptr&& f_ptr, F_idx&& f_idx)
{
    using namespace dep0::testing;
    if (llvm::GetElementPtrInst::getPointerOperandIndex() != 0)
        return failure("LLVM changed the operand index from 0 to ", llvm::GetElementPtrInst::getPointerOperandIndex());
    auto const gep = llvm::dyn_cast<llvm::GetElementPtrInst>(v);
    if (not gep)
        return failure("instruction is not getelementptr");
    if (auto const result = std::forward<F_type>(f_type)(*gep->getSourceElementType()); not result)
        return failure("GEP source element type predicate failed: ", result.message());
    if (auto const result = std::forward<F_ptr>(f_ptr)(*gep->getPointerOperand()); not result)
        return failure("GEP operand predicate failed: ", result.message());
    if (auto const result = std::forward<F_idx>(f_idx)(*gep->getOperand(1)); not result)
        return failure("GEP index predicate failed: ", result.message());
    return true;
}

boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const*);
boost::test_tools::predicate_result is_return_of(llvm::Instruction const*, std::string_view);
boost::test_tools::predicate_result is_return_of(llvm::Instruction const*, llvm::Value const&);

template <dep0::testing::Predicate<llvm::Value> F>
boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const instr, F&& f)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const ret = dyn_cast<llvm::ReturnInst>(instr);
    if (not ret)
        return dep0::testing::failure("not a return instruction");
    if (not ret->getReturnValue())
        return dep0::testing::failure("return instruction has no value");
    return std::forward<F>(f)(*ret->getReturnValue());
}

template <
    dep0::testing::Predicate<llvm::Value> F_val,
    dep0::testing::Predicate<llvm::Value> F_ptr>
boost::test_tools::predicate_result is_store(llvm::Instruction const* v, F_val&& f_val, F_ptr&& f_ptr)
{
    auto const store = llvm::dyn_cast<llvm::StoreInst>(v);
    if (not store)
        return dep0::testing::failure("instruction is not store");
    if (auto const result = std::forward<F_val>(f_val)(*store->getValueOperand()); not result)
        return dep0::testing::failure("store value operand predicate failed: ", result.message());
    if (auto const result = std::forward<F_ptr>(f_ptr)(*store->getPointerOperand()); not result)
        return dep0::testing::failure("store pointer operand predicate failed: ", result.message());
    return true;
}

boost::test_tools::predicate_result is_signed_constant(llvm::Value const&, long long);

inline auto signed_constant(long long const value)
{
    return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
    {
        auto const c = dyn_cast<llvm::ConstantInt>(&v);
        if (not c)
            return dep0::testing::failure("value is not a constant");
        if (c->getSExtValue() != value)
            return dep0::testing::failure(c->getSExtValue(), " != ", value);
        return true;
    };
}

template <dep0::testing::Predicate<llvm::ConstantInt> F>
auto constant_int(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
    {
        auto const c = dyn_cast<llvm::ConstantInt>(&v);
        if (not c)
            return dep0::testing::failure("value is not a constant");
        return f(*c);
    };
}

inline auto unsigned_constant(unsigned long long const value)
{
    return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
    {
        auto const c = dyn_cast<llvm::ConstantInt>(&v);
        if (not c)
            return dep0::testing::failure("value is not a constant");
        if (c->getZExtValue() != value)
            return dep0::testing::failure(c->getZExtValue(), " != ", value);
        return true;
    };
}

} // namespace dep0::llvmgen::testing
