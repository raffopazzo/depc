/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
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

// primitive

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

// array

template <Predicate<llvm::Type> F>
boost::test_tools::predicate_result is_array_of(llvm::ArrayType const& x, std::size_t const size, F&& f)
{
    using namespace dep0::testing;
    if (auto const n = x.getNumElements(); n != size)
        return failure("array size: ", n, " != ", size);
    if (auto const result = std::forward<F>(f)(*x.getElementType()); not result)
        return failure("array element type:", result.message());
    return true;
}

template <Predicate<llvm::Type> F>
boost::test_tools::predicate_result is_array_of(llvm::Type const& x, std::size_t const size, F&& f)
{
    using namespace dep0::testing;
    if (auto const p = llvm::dyn_cast<llvm::ArrayType>(&x))
        return is_array_of(*p, size, std::forward<F>(f));
    else
        return failure("type is not an array");
}

// pointer

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

// struct

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::StructType const& s, std::string_view const name, Types&&... types)
{
    using namespace dep0::testing;
    if (name.empty())
    {
        if (not s.isLiteral())
            return failure("struct is not literal but has name ", s.getName().str());
    }
    else
    {
        if (s.isLiteral())
            return failure("struct is literal but should have name ", name);
        if (auto const x = static_cast<std::string_view>(s.getName()); x != name)
            return failure("struct name: ", x, " != ", name);
    }
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
            if (auto const tmp = std::forward<Types>(types)(*s.getElementType(i)); not tmp)
                result = failure("literal struct element predidcate failed at index ", i, ": ", tmp.message());
        }
    }(), ...);
    return result;
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result
is_struct(llvm::StructType const* const p, std::string_view const name, Types&&... types)
{
    if (not p)
        return dep0::testing::failure("struct is null");
    return is_struct(*p, name, std::forward<Types>(types)...);
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::Type const& t, std::string_view const name, Types&&... types)
{
    if (auto const p = dyn_cast<llvm::StructType>(&t))
        return is_struct(*p, name, std::forward<Types>(types)...);
    else
        return dep0::testing::failure("type is not a struct but ", to_string(t));
}

template <Predicate<llvm::Type>... Types>
boost::test_tools::predicate_result is_struct(llvm::Type const* const p, std::string_view const name, Types&&... types)
{
    if (not p)
        return dep0::testing::failure("type null");
    return is_struct(*p, name, std::forward<Types>(types)...);
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
auto array_of(std::size_t const size, F&& f)
{
    return [size, f=std::forward<F>(f)] (llvm::Type const& x)
    {
        return impl::is_array_of(x, size, f);
    };
}

template <Predicate<llvm::Type> F>
auto pointer_to(F&& f)
{
    return [f=std::forward<F>(f)] (llvm::Type const& x)
    {
        return impl::is_pointer_to(x, f);
    };
}

inline constexpr auto is_struct = boost::hana::overload(
    [] <Predicate<llvm::Type>... F> (llvm::StructType const& x, std::string_view const name, F&&... f)
    {
        return impl::is_struct(x, name, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::StructType const* const p, std::string_view const name, F&&... f)
    {
        return impl::is_struct(p, name, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const& x, std::string_view const name, F&&... f)
    {
        return impl::is_struct(x, name, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const* const p, std::string_view const name, F&&... f)
    {
        return impl::is_struct(p, name, std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::StructType const& x, F&&... f)
    {
        return impl::is_struct(x, "", std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::StructType const* const p, F&&... f)
    {
        return impl::is_struct(p, "", std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const& x, F&&... f)
    {
        return impl::is_struct(x, "", std::forward<F>(f)...);
    },
    [] <Predicate<llvm::Type>... F> (llvm::Type const* const p, F&&... f)
    {
        return impl::is_struct(p, "", std::forward<F>(f)...);
    }
);

template <Predicate<llvm::Type>... Types>
auto struct_of(std::string_view const name, Types&&... types)
{
    struct predicate_t
    {
        std::string name;
        std::tuple<std::remove_cvref_t<Types>...> types;

        boost::test_tools::predicate_result operator()(llvm::StructType const& x) const
        {
            return std::apply(is_struct, std::tuple_cat(std::tie(x, name), types));
        }

        boost::test_tools::predicate_result operator()(llvm::StructType const* const p) const
        {
            return std::apply(is_struct, std::tuple_cat(std::make_tuple(p, name), types));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const& x) const
        {
            return std::apply(is_struct, std::tuple_cat(std::tie(x, name), types));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const* const p) const
        {
            return std::apply(is_struct, std::tuple_cat(std::make_tuple(p, name), types));
        }
    };
    return predicate_t{{std::move(types)...}};
}

template <Predicate<llvm::Type>... Types>
auto struct_of(Types&&... types)
{
    struct predicate_t
    {
        std::tuple<std::remove_cvref_t<Types>...> types;

        boost::test_tools::predicate_result operator()(llvm::StructType const& x) const
        {
            return std::apply(is_struct, std::tuple_cat(std::tie(x), types));
        }

        boost::test_tools::predicate_result operator()(llvm::StructType const* const p) const
        {
            return std::apply(is_struct, std::tuple_cat(std::make_tuple(p), types));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const& x) const
        {
            return std::apply(is_struct, std::tuple_cat(std::tie(x), types));
        }

        boost::test_tools::predicate_result operator()(llvm::Type const* const p) const
        {
            return std::apply(is_struct, std::tuple_cat(std::make_tuple(p), types));
        }
    };
    return predicate_t{{std::move(types)...}};
}

inline constexpr auto is_void = boost::hana::overload(
    [] (llvm::Type const& x) { return impl::is_void(x); },
    [] (llvm::Type const* const p) { return impl::is_void(p); }
);

} // namespace dep0::llvmgen::testing
