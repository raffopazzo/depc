#include "typecheck_tests_fixture.hpp"

#include "dep0/typecheck/check.hpp"
#include "dep0/typecheck/environment.hpp"

#include "dep0/parser/parse.hpp"

/**
 * Helper function used to ensure that the base environment is constructed only once for an entire test suite.
 * This is helpful because constructing a fresh base environment for each test would make running the test suite
 * unnecessarily slow, because we would need to parse and check the prelude for each test.
 * Moreover, instead of using a global variable, a function with a static local is preferred because apparently
 * ANTLR4 seems to also initialise some global which might not have been initialised yet when parsing the prelude.
 */
static dep0::typecheck::env_t const& get_base_env()
{
    static auto const env = dep0::typecheck::make_base_env().value();
    return env;
}

boost::test_tools::predicate_result TypecheckTestsFixture::pass(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (parse_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    auto check_result = dep0::typecheck::check(get_base_env(), *parse_result);
    if (check_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), check_result.error());
        return res;
    }
    pass_result.emplace(std::move(*check_result));
    return true;
}

boost::test_tools::predicate_result TypecheckTestsFixture::fail(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (parse_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    auto check_result = dep0::typecheck::check(get_base_env(), *parse_result);
    if (check_result.has_value())
    {
        auto res = boost::test_tools::predicate_result(false);
        res.message() << "Was expecting typecheck to fail but it succeeded";
        return res;
    }
    fail_result.emplace(std::move(check_result.error()));
    return true;
}
