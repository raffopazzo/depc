#include "parser_tests_fixture.hpp"

#include "dep0/parser/parse.hpp"

boost::test_tools::predicate_result ParserTestsFixture::pass(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    if (not parse_result)
    {
        auto res = boost::test_tools::predicate_result(false);
        dep0::pretty_print(res.message().stream(), parse_result.error());
        return res;
    }
    pass_result.emplace(std::move(*parse_result));
    return true;
}

boost::test_tools::predicate_result ParserTestsFixture::fail(std::filesystem::path const file)
{
    auto parse_result = dep0::parser::parse(testfiles / file);
    auto res = boost::test_tools::predicate_result(parse_result.has_error());
    if (not res)
        res.message() << "Was expecting parsing to fail but it succeeded";
    return res;
}

