#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/predicate.hpp"

namespace dep0::testing {

template <typename T, Predicate<T>... Fs>
boost::test_tools::predicate_result check_all(std::vector<T> const& v, Fs&&... fs)
{
    auto constexpr N = sizeof...(Fs);
    if (v.size() != N)
        return failure("wrong number of predicates for vector: ", N, " != ", v.size());
    auto result = boost::test_tools::predicate_result(true);
    auto it = v.begin();
    ([&] { if (result) result = std::forward<Fs>(fs)(*it++); }(), ...);
    return result;
}

} // namespace dep0::testing

