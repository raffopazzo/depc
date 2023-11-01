#pragma once

#include "dep0/testing/predicate.hpp"
#include "dep0/testing/failure.hpp"
#include "dep0/unique_ref.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include <boost/test/tools/assertion_result.hpp>

#include <cstdlib>
#include <filesystem>
#include <optional>

struct LLVMGenTestsFixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;
    std::optional<dep0::unique_ref<llvm::Module>> pass_result;
    bool apply_beta_delta_normalization = false;

    boost::test_tools::predicate_result pass(std::filesystem::path);

    template <dep0::testing::Predicate<llvm::Function> F>
    boost::test_tools::predicate_result has_function(std::string_view const name, F&& f)
    {
        auto const function = pass_result.value()->getFunction(name);
        if (not function)
            return dep0::testing::failure("function not found: ", name);
        return std::forward<F>(f)(*function);
    }

    static boost::test_tools::predicate_result is_i32(llvm::Argument const*, std::optional<std::string_view>);
    static boost::test_tools::predicate_result is_u32(llvm::Argument const*, std::string_view);

    template <dep0::testing::Predicate<llvm::FunctionType> F>
    static boost::test_tools::predicate_result is_pointer_to_function(llvm::Type const* const type, F&& f)
    {
        if (not type)
            return dep0::testing::failure("type is null");
        if (not type->isPointerTy())
            return dep0::testing::failure("type is not a pointer");
        auto const fn_type = cast<llvm::FunctionType>(type->getPointerElementType());
        if (not fn_type)
            return dep0::testing::failure("type is not a pointer to function");
        return std::forward<F>(f)(*fn_type);
    }

    static boost::test_tools::predicate_result is_return_of(llvm::Instruction const*, std::string_view);

    template <dep0::testing::Predicate<llvm::Value> F>
    static boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const instr, F&& f)
    {
        if (not instr)
            return dep0::testing::failure("instr is null");
        auto const ret = cast<llvm::ReturnInst>(instr);
        if (not ret)
            return dep0::testing::failure("not a return instruction");
        if (not ret->getReturnValue())
            return dep0::testing::failure("return instruction has no value");
        return std::forward<F>(f)(*ret->getReturnValue());
    }

    template <dep0::testing::Predicate<llvm::CallInst> F>
    static boost::test_tools::predicate_result is_call_inst(llvm::Instruction const& instr, F&& f)
    {
        auto const call = cast<llvm::CallInst>(&instr);
        if (not call)
            return dep0::testing::failure("not a call instruction");
        return std::forward<F>(f)(*call);
    }

    template <dep0::testing::Predicate<llvm::CallInst> F>
    static auto call_inst(F&& f)
    {
        return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const call = cast<llvm::CallInst>(&v);
            if (not call)
                return dep0::testing::failure("value is not a function call");
            return f(*call);
        };
    }

    template <dep0::testing::Predicate<llvm::ConstantInt> F>
    static auto constant_int(F&& f)
    {
        return [f=std::forward<F>(f)] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            return f(*c);
        };
    }

    static boost::test_tools::predicate_result is_signed_constant(llvm::Value const&, long long);

    static inline auto signed_constant(long long const value)
    {
        return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            if (c->getSExtValue() != value)
                return dep0::testing::failure(c->getSExtValue(), " != ", value);
            return true;
        };
    }

    static inline auto unsigned_constant(unsigned long long const value)
    {
        return [value] (llvm::Value const& v) -> boost::test_tools::predicate_result
        {
            auto const c = cast<llvm::ConstantInt>(&v);
            if (not c)
                return dep0::testing::failure("value is not a constant");
            if (c->getZExtValue() != value)
                return dep0::testing::failure(c->getZExtValue(), " != ", value);
            return true;
        };
    }
};
