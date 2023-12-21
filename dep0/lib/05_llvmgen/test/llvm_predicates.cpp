#include "llvm_predicates.hpp"

namespace dep0::llvmgen::testing {

boost::test_tools::predicate_result has_void_return_type(llvm::Function const& f)
{
    if (not f.getReturnType()->isIntegerTy(8))
        return dep0::testing::failure("void return type should be mapped to i8");
    if (f.getAttributes().hasAttributes(llvm::AttributeList::ReturnIndex))
        return dep0::testing::failure("function returning void should have no return attributes");
    return true;
}

boost::test_tools::predicate_result is_i32_type(llvm::Type const& type)
{
    if (not type.isIntegerTy(32))
        return dep0::testing::failure("type is not 32bit integer");
    else
        return true;
}

boost::test_tools::predicate_result is_bool(
    llvm::Argument const* const arg,
    std::optional<std::string_view> const name)
{
    if (not arg)
        return dep0::testing::failure("arg is null");
    if (not arg->getType()->isIntegerTy(1))
        return dep0::testing::failure("argument type is not 1bit integer");
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
    if (arg->hasSExtAttr())
        return dep0::testing::failure("argument should not have SExt atribute");
    if (arg->hasZExtAttr())
        return dep0::testing::failure("argument should not have ZExt atribute");
    return true;
}

boost::test_tools::predicate_result is_i32(llvm::Argument const* const arg, std::optional<std::string_view> const name)
{
    if (not arg)
        return dep0::testing::failure("arg is null");
    if (not arg->getType()->isIntegerTy(32))
        return dep0::testing::failure("argument type is not 32bit integer");
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

boost::test_tools::predicate_result is_u32(llvm::Argument const* const arg, std::optional<std::string_view> const name)
{
    if (not arg)
        return dep0::testing::failure("arg is null");
    if (not arg->getType()->isIntegerTy(32))
        return dep0::testing::failure("argument type is not 32bit integer");
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
    if (not arg->hasZExtAttr())
        return dep0::testing::failure("argument has no ZExt atribute");
    return true;
}

boost::test_tools::predicate_result is_return_of_void(llvm::Instruction const* const instr)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const ret = dyn_cast<llvm::ReturnInst>(instr);
    if (not ret)
        return dep0::testing::failure("not a return instruction");
    if (not ret->getReturnValue())
        return dep0::testing::failure("return instruction has no value");
    if (not ret->getReturnValue()->getType()->isIntegerTy(8))
        return dep0::testing::failure("return value should be an i8");
    auto const val = dyn_cast<llvm::ConstantInt>(ret->getReturnValue());
    if (not val)
        return dep0::testing::failure("return value should be a constant int");
    if (not val->isZero())
        return dep0::testing::failure("return value should be the constant 0");
    return true;
}

boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const instr, std::string_view const name)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const ret = dyn_cast<llvm::ReturnInst>(instr);
    if (not ret)
        return dep0::testing::failure("not a return instruction");
    if (not ret->getReturnValue())
        return dep0::testing::failure("return instruction has no value");
    if (auto const& str = ret->getReturnValue()->getName().str(); str != name)
        return dep0::testing::failure("return value ", str, " != ", name);
    return true;
}

boost::test_tools::predicate_result is_return_of(llvm::Instruction const* const instr, llvm::Value const& val)
{
    if (not instr)
        return dep0::testing::failure("instr is null");
    auto const ret = dyn_cast<llvm::ReturnInst>(instr);
    if (not ret)
        return dep0::testing::failure("not a return instruction");
    if (not ret->getReturnValue())
        return dep0::testing::failure("return instruction has no value");
    if (ret->getReturnValue() != &val)
    {
        std::string expected, actual;
        auto stream1 = llvm::raw_string_ostream(expected);
        auto stream2 = llvm::raw_string_ostream(actual);
        val.print(stream1);
        ret->getReturnValue()->print(stream2);
        return dep0::testing::failure("return value ", expected, " != ", actual);
    }
    return true;
}

boost::test_tools::predicate_result is_signed_constant(llvm::Value const& v, long long const value)
{
    auto const c = dyn_cast<llvm::ConstantInt>(&v);
    if (not c)
        return dep0::testing::failure("value is not a constant");
    if (c->getSExtValue() != value)
        return dep0::testing::failure(c->getSExtValue(), " != ", value);
    return true;
}

} // namespace dep0::llvmgen::testing
