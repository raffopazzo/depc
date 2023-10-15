#pragma once

#include <boost/type_index.hpp>
#include <variant>

namespace dep0::testing {

template <typename... Ts>
auto pretty_name(std::variant<Ts...> const& x)
{
    return std::visit([] <typename T> (T const&) { return boost::typeindex::type_id<T>().pretty_name(); }, x);
}

} // namespace dep0::testing
