#pragma once

#include "dep0/testing/failure.hpp"
#include "dep0/testing/pretty_name.hpp"

#include "dep0/ast/ast.hpp"
#include "dep0/ast/pretty_print.hpp"

#include <boost/test/tools/assertion_result.hpp>

namespace dep0::testing {

template <ast::Properties P>
boost::test_tools::predicate_result is_integer_def(
    ast::type_def_t<P> const& t,
    std::string_view const name,
    dep0::ast::sign_t const sign,
    dep0::ast::width_t const width,
    std::optional<boost::multiprecision::cpp_int> max_abs_value)
{
    auto const integer = std::get_if<typename ast::type_def_t<P>::integer_t>(&t.value);
    if (not integer)
        return failure("typedef is not integer_t byt ", pretty_name(t.value));
    auto result = boost::test_tools::predicate_result(true);
    pretty_print(result.message().stream(), t);
    if (integer->name != name)
    {
        result = false;
        result.message().stream() << " has wrong name";
    }
    if (integer->sign != sign)
    {
        result = false;
        result.message().stream() << " has wrong sign";
    }
    if (integer->width != width)
    {
        result = false;
        result.message().stream() << " has wrong width";
    }
    if (max_abs_value)
    {
        if (not integer->max_abs_value.has_value())
        {
            result = false;
            result.message().stream() << " has no max abs value but it should have " << *max_abs_value;
        }
        else if (*integer->max_abs_value != *max_abs_value)
        {
            result = false;
            result.message().stream()
                << " has wrong max abs value: "
                << *integer->max_abs_value << " != " << *max_abs_value;
        }
    }
    else if (integer->max_abs_value)
    {
        result = false;
        result.message().stream() << " has a max abs value but it should not";
    }
    return result;
}

} // namespace dep0::testing
