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

    static constexpr auto bool_ = type_expr_of<dep0::parser::properties_t>(dep0::testing::is_type_bool);
    static constexpr auto i32 = type_expr_of<dep0::parser::properties_t>(dep0::testing::is_type_i32);
    static constexpr auto u32 = type_expr_of<dep0::parser::properties_t>(dep0::testing::is_type_u32);

    template <typename... Args>
    static constexpr auto app_of(Args&&... args)
    {
        return dep0::testing::app_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <dep0::testing::Predicate<dep0::parser::type_t> F>
    static auto term_binder(F&& f)
    {
        return dep0::testing::term_binder<dep0::parser::properties_t>(std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::parser::type_t> F>
    static auto term_binder(std::string const& name, F&& f)
    {
        return dep0::testing::term_binder<dep0::parser::properties_t>(name, std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::parser::type_t> F>
    static auto type_of(F&& f)
    {
        return dep0::testing::type_of<dep0::parser::properties_t>(std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::parser::func_arg_t> F>
    static auto is_arg(dep0::parser::func_arg_t const& arg, F&& f)
    {
        return dep0::testing::is_arg<dep0::parser::properties_t>(arg, std::forward<F>(f));
    }
};
