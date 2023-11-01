#include "llvmgen_tests_fixture.hpp"

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/transform/beta_delta_normalization.hpp"

#include "dep0/llvmgen/gen.hpp"

boost::test_tools::predicate_result LLVMGenTestsFixture::pass(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (parse_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    auto check_result = dep0::typecheck::check(*parse_result);
    if (check_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), check_result.error());
        return res;
    }
    if (apply_beta_delta_normalization)
    {
        dep0::transform::beta_delta_normalization_t beta_delta_normalization{};
        auto transform_result = beta_delta_normalization(*check_result);
        if (transform_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), transform_result.error());
            return res;
        }
    }
    auto gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
    if (gen_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), gen_result.error());
        return res;
    }
    pass_result.emplace(std::move(*gen_result));
    return true;
}

boost::test_tools::predicate_result LLVMGenTestsFixture::is_i32(
    llvm::Argument const* const arg,
    std::optional<std::string_view> const name)
{
    if (not arg)
        return dep0::testing::failure("arg is null");
    if (name)
    {
        if (arg->getName().str() != *name)
            return dep0::testing::failure("argument name: ", arg->getName().str(), " != ", *name);
    }
    else
    {
        if (not arg->getName().empty())
            return dep0::testing::failure("argument has a name but should be anonymous: ", arg->getName().str());
    }
    if (not arg->hasSExtAttr())
        return dep0::testing::failure("argument has no SExt atribute");
    return true;
}

boost::test_tools::predicate_result LLVMGenTestsFixture::is_u32(
    llvm::Argument const* const arg,
    std::string_view const name)
{
    if (not arg)
        return dep0::testing::failure("arg is null");
    if (arg->getName().str() != name)
        return dep0::testing::failure("argument name: ", arg->getName().str(), " != ", name);
    if (not arg->hasZExtAttr())
        return dep0::testing::failure("argument has no ZExt atribute");
    return true;
}

boost::test_tools::predicate_result LLVMGenTestsFixture::is_return_of(
    llvm::Instruction const* const instr,
    std::string_view const name)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const ret = cast<llvm::ReturnInst>(instr);
    if (not ret)
        return dep0::testing::failure("not a return instruction");
    if (not ret->getReturnValue())
        return dep0::testing::failure("return instruction has no value");
    if (auto const& str = ret->getReturnValue()->getName().str(); str != name)
        return dep0::testing::failure("return value ", str, " != ", name);
    return true;
}

boost::test_tools::predicate_result LLVMGenTestsFixture::is_signed_constant(llvm::Value const& v, long long const value)
{
    auto const c = cast<llvm::ConstantInt>(&v);
    if (not c)
        return dep0::testing::failure("value is not a constant");
    if (c->getSExtValue() != value)
        return dep0::testing::failure(c->getSExtValue(), " != ", value);
    return true;
}
