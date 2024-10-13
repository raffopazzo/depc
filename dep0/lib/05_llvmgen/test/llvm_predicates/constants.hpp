#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_constant(llvm::Value const&, bool);
boost::test_tools::predicate_result is_constant(llvm::Value const*, bool);

boost::test_tools::predicate_result is_constant(llvm::Value const&, int);
boost::test_tools::predicate_result is_constant(llvm::Value const*, int);

boost::test_tools::predicate_result is_constant(llvm::Value const&, std::int64_t);
boost::test_tools::predicate_result is_constant(llvm::Value const*, std::int64_t);

boost::test_tools::predicate_result is_constant(llvm::Value const&, std::uint64_t);
boost::test_tools::predicate_result is_constant(llvm::Value const*, std::uint64_t);

} // namespace impl

inline constexpr auto is_constant = boost::hana::overload(
    [] (llvm::Value const& x, bool const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, bool const v) { return impl::is_constant(p, v); },
    [] (llvm::Value const& x, int const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, int const v) { return impl::is_constant(p, v); },
    [] (llvm::Value const& x, std::int64_t const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, std::int64_t const v) { return impl::is_constant(p, v); },
    [] (llvm::Value const& x, std::uint64_t const v) { return impl::is_constant(x, v); },
    [] (llvm::Value const* const p, std::uint64_t const v) { return impl::is_constant(p, v); }
);

inline auto constant(bool const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

inline auto constant(int const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

inline auto constant(std::int64_t const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

inline auto constant(std::uint64_t const v)
{
    return [v] (llvm::Value const& x)
    {
        return impl::is_constant(x, v);
    };
}

template <Predicate<llvm::Instruction> F>
boost::test_tools::predicate_result is_const_expr_of(llvm::Value const& x, F&& f)
{
    using namespace dep0::testing;
    auto const c = llvm::dyn_cast<llvm::ConstantExpr>(&x);
    if (not c)
        return failure("value is not a constant expression but: ", to_string(x));
    auto const inst = c->getAsInstruction();
    if (not inst)
        return failure("constant expression is not an instruction");
    if (auto const result = std::forward<F>(f)(*inst); not result)
        return failure("inside constant expression: ", result.message());
    return true;
}

template <Predicate<llvm::Instruction> F>
boost::test_tools::predicate_result is_const_expr_of(llvm::Value const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("valut is null");
    return is_const_expr_of(*p, std::forward<F>(f));
}

template <Predicate<llvm::Instruction> F>
inline auto const_expr_of(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Value const& x)
    {
        return is_const_expr_of(x, f);
    };
}

} // namespace dep0::llvmgen::testing
