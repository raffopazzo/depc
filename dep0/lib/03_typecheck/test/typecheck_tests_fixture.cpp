#include "typecheck_tests_fixture.hpp"

#include "dep0/typecheck/check.hpp"

#include "dep0/parser/parse.hpp"

boost::test_tools::predicate_result TypecheckTestsFixture::pass(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (parse_result.has_error())
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    auto check_result = dep0::typecheck::check(*parse_result);
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
    auto check_result = dep0::typecheck::check(*parse_result);
    if (check_result.has_value())
    {
        auto res = boost::test_tools::predicate_result(false);
        res.message() << "Was expecting typecheck to fail but it succeeded";
        return res;
    }
    fail_result.emplace(std::move(check_result.error()));
    return true;
}
