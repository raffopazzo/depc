#pragma once

#include "dep0/parser/ast.hpp"
#include "dep0/testing/ast_predicates.hpp"

#include <boost/test/tools/assertion_result.hpp>

#include <cstdlib>
#include <optional>
#include <filesystem>

struct ParserTestsFixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    std::optional<dep0::parser::module_t> pass_result;

    boost::test_tools::predicate_result pass(std::filesystem::path);
    boost::test_tools::predicate_result fail(std::filesystem::path);

    template <typename... Args>
    static constexpr auto app_of(Args&&... args)
    {
        return dep0::testing::app_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto pi_of(Args&&... args)
    {
        return dep0::testing::pi_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr auto plus(Args&&... args)
    {
        return dep0::testing::plus<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <dep0::testing::Predicate<dep0::parser::expr_t> F>
    static auto arg_of(F&& f, std::optional<std::string> name = std::nullopt)
    {
        return dep0::testing::arg_of<dep0::parser::properties_t>(std::forward<F>(f), std::move(name));
    }

    template <typename... Args>
    static auto is_arg(dep0::parser::func_arg_t const& arg, Args&&... args)
    {
        return dep0::testing::is_arg<dep0::parser::properties_t>(arg, std::forward<Args>(args)...);
    }
};
