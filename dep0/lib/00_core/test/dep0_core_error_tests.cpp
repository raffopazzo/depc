#define BOOST_TEST_MODULE dep0_core_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/error.hpp"

#include <sstream>

static std::string to_string(dep0::error_t const& x)
{
    std::ostringstream out;
    dep0::pretty_print(out, x);
    return out.str();
}

// WARN if argument is not a global literal we'll eventually crash!
static dep0::source_text from_literal(char const* s)
{
    return {dep0::make_source_handle<char const*>(s), s};
}

BOOST_AUTO_TEST_SUITE(dep0_core_tests)

BOOST_AUTO_TEST_CASE(pretty_print_error)
{
    using namespace dep0;
    auto const stmt = from_literal("return -1;");
    auto const body = from_literal(R"(int foo()
{
    return -1;
})");
    auto const empty = dep0::error_t{};
    auto const simple = dep0::error_t{"unknown error"};
    auto const with_location_only = dep0::error_t{"syntax error", source_loc_t{1, 2, from_literal("")}};
    auto const with_location_stmt = dep0::error_t{"syntax error", source_loc_t{1, 2, stmt}};
    auto const with_location_body = dep0::error_t{"syntax error", source_loc_t{1, 2, body}};
    auto const with_simple_reason = dep0::error_t{"parse failed", std::vector{simple}};
    auto const with_multiple_reasons =
        dep0::error_t{
            "typecheck failed",
            std::vector{
                simple,
                with_location_only,
                with_location_body,
                with_location_stmt
            }};
    auto const with_one_nested_reason = dep0::error_t{"codegen failed", std::vector{with_multiple_reasons}};
    auto const with_two_nested_reasons =
        dep0::error_t{
            "codegen failed",
            std::vector{
                with_multiple_reasons,
                with_simple_reason
            }};
    BOOST_TEST(to_string(empty) == "");
    BOOST_TEST(to_string(simple) == "unknown error");
    BOOST_TEST(to_string(with_location_only) == "at 1:2 syntax error");
    BOOST_TEST(to_string(with_location_stmt) == "at 1:2 `return -1;` syntax error");
    BOOST_TEST(to_string(with_location_body) == R"(at 1:2 syntax error
    > int foo()
    > {
    >     return -1;
    > })");
    BOOST_TEST(to_string(with_simple_reason) == "parse failed because unknown error");
    BOOST_TEST(to_string(with_multiple_reasons) == R"(typecheck failed because:
    1. unknown error
    2. at 1:2 syntax error
    3. at 1:2 syntax error
        > int foo()
        > {
        >     return -1;
        > }
    4. at 1:2 `return -1;` syntax error)");
    BOOST_TEST(to_string(with_one_nested_reason) == R"(codegen failed because typecheck failed because:
    1. unknown error
    2. at 1:2 syntax error
    3. at 1:2 syntax error
        > int foo()
        > {
        >     return -1;
        > }
    4. at 1:2 `return -1;` syntax error)");
    BOOST_TEST(to_string(with_two_nested_reasons) == R"(codegen failed because:
    1. typecheck failed because:
        1. unknown error
        2. at 1:2 syntax error
        3. at 1:2 syntax error
            > int foo()
            > {
            >     return -1;
            > }
        4. at 1:2 `return -1;` syntax error
    2. parse failed because unknown error)");
}

BOOST_AUTO_TEST_SUITE_END()
