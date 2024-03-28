#pragma once

#include "llvm_predicates/predicate.hpp"
#include "llvm_predicates/to_string.hpp"

#include "dep0/testing/failure.hpp"

#include <llvm/IR/DerivedTypes.h>
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

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::StructType const& s, Types&&... types)
{
    using namespace dep0::testing;
    if (not s.isLiteral())
        return failure("struct is not literal but has name ", s.getName().str());
    auto constexpr N = sizeof...(Types);
    if (s.getNumElements() != N)
        return failure("literal struct has wrong number of elements: ", s.getNumElements(), " != ", N);
    boost::test_tools::predicate_result result = true;
    int next = 0u;
    ([&]
    {
        if (result)
        {
            auto const i = next++;
            if (auto const tmp = types(s.getElementType(i)); not tmp)
                result = failure("literal struct element predidcate failed at index ", i, ": ", tmp.message());
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::StructType const* const p, Types&&... types)
{
    using namespace dep0::testing;
    if (not p)
        return failure("struct is null");
    return is_struct(*p, std::forward<Types>(types)...);
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::Type const& t, Types&&... types)
{
    using namespace dep0::testing;
    auto const p = dyn_cast<llvm::StructType>(&t);
    if (not p)
        return failure("type is not a struct but ", to_string(t));
    return is_struct(*p, std::forward<Types>(types)...);
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::Type const* const p, Types&&... types)
{
    using namespace dep0::testing;
    if (not p)
        return failure("type null");
    return is_struct(*p, std::forward<Types>(types)...);
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

inline constexpr auto is_struct = boost::hana::overload(
    [] <Predicate<llvm::Type>... F> (llvm::StructType const& x, F&&... f)
    {
        return impl::is_struct(x, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::StructType const* const p, F&&... f)
    {
        return impl::is_struct(p, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const& x, F&&... f)
    {
        return impl::is_struct(x, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const* const p, F&&... f)
    {
        return impl::is_struct(p, std::forward<F>(f)...);
    }
);

template <Predicate<llvm::Type>... Types>
auto struct_of(Types&&... types)
{
    struct predicate_t
    {
        std::tuple<std::remove_cvref_t<Types>...> types;

        boost::test_tools::predicate_result operator()(llvm::StructType const& x) const
        {
            return std::apply(
                is_struct,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, std::get<Is>(types)...);
                }(std::make_index_sequence<sizeof...(Types)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::StructType const* const p) const
        {
            return std::apply(
                is_struct,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(p, std::get<Is>(types)...);
                }(std::make_index_sequence<sizeof...(Types)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const& x) const
        {
            return std::apply(
                is_struct,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(x, std::get<Is>(types)...);
                }(std::make_index_sequence<sizeof...(Types)>{}));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const* const p) const
        {
            return std::apply(
                is_struct,
                [&] <std::size_t... Is> (std::index_sequence<Is...>)
                {
                    return std::forward_as_tuple(p, std::get<Is>(types)...);
                }(std::make_index_sequence<sizeof...(Types)>{}));
        }
    };
    return predicate_t{{std::move(types)...}};
}

inline constexpr auto is_void = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_void(x); },
    [] (llvm::Type const* const p) { return impl::is_void(p); }
);

} // namespace dep0::llvmgen::testing
