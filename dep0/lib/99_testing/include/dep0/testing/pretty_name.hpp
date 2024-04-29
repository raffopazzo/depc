#pragma once

#include <boost/type_index.hpp>
#include <variant>

namespace dep0::testing {

template <typename... Ts>
auto pretty_name(std::variant<Ts...> const& x)
{
    std::string const s =
        std::visit(
            [] <typename T> (T const&)
            {
                return boost::typeindex::type_id<T>().pretty_name();
            },
            x);
    if (auto const i = s.find_last_of(':'); i != s.npos)
        return s.substr(i+1);
    else
        return s;
}

} // namespace dep0::testing
