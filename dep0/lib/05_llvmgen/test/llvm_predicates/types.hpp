#pragma once

#include "llvm_predicates/predicate.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/Type.h>

#include <boost/hana.hpp>
#include <boost/test/tools/assertion_result.hpp>

namespace dep0::llvmgen::testing {

namespace impl {

boost::test_tools::predicate_result is_i1(llvm::Type const&);
boost::test_tools::predicate_result is_i1(llvm::Type const*);

boost::test_tools::predicate_result is_i8(llvm::Type const&);
boost::test_tools::predicate_result is_i8(llvm::Type const*);

boost::test_tools::predicate_result is_i16(llvm::Type const&);
boost::test_tools::predicate_result is_i16(llvm::Type const*);

boost::test_tools::predicate_result is_i32(llvm::Type const&);
boost::test_tools::predicate_result is_i32(llvm::Type const*);

boost::test_tools::predicate_result is_i64(llvm::Type const&);
boost::test_tools::predicate_result is_i64(llvm::Type const*);

template <Predicate<llvm::Type> F>
boost::test_tools::predicate_result is_pointer_to(llvm::Type const& x, F&& f)
{
    using namespace dep0::testing;
    if (not x.isPointerTy())
        return failure("type is not a pointer");
    if (auto const result = std::forward<F>(f)(*x.getPointerElementType()); not result)
        return failure("predicate failed for pointer element type: ", result.message());
    return true;
}

template <Predicate<llvm::Type> F>
boost::test_tools::predicate_result is_pointer_to(llvm::Type const* const p, F&& f)
{
    if (not p)
        return dep0::testing::failure("type is null");
    return is_pointer_to(*p, std::forward<F>(f));
}

boost::test_tools::predicate_result is_void(llvm::Type const&);
boost::test_tools::predicate_result is_void(llvm::Type const*);

} // namespace impl

inline constexpr auto is_i1 = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_i1(x); },
    [] (llvm::Type const* const p) { return impl::is_i1(p); }
);

inline constexpr auto is_i8 = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_i8(x); },
    [] (llvm::Type const* const p) { return impl::is_i8(p); }
);

inline constexpr auto is_i16 = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_i16(x); },
    [] (llvm::Type const* const p) { return impl::is_i16(p); }
);

inline constexpr auto is_i32 = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_i32(x); },
    [] (llvm::Type const* const p) { return impl::is_i32(p); }
);

inline constexpr auto is_i64 = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_i64(x); },
    [] (llvm::Type const* const p) { return impl::is_i64(p); }
);

inline constexpr auto is_pointer_to = boost::hana::overload(
    [] <Predicate<llvm::Type> F> (llvm::Type const& x, F&& f)
    {
        return impl::is_pointer_to(x, std::forward<F>(f));
    },
    [] <Predicate<llvm::Type> F> (llvm::Type const* const p, F&& f)
    {
        return impl::is_pointer_to(p, std::forward<F>(f));
    }
);

template <Predicate<llvm::Type> F>
auto pointer_to(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Type const& x)
    {
        return impl::is_pointer_to(x, f);
    };
}

inline constexpr auto is_void = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_void(x); },
    [] (llvm::Type const* const p) { return impl::is_void(p); }
);

} // namespace dep0::llvmgen::testing
