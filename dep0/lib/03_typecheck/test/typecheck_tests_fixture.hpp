#pragma ocne

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/error.hpp"

#include "dep0/testing/ast_predicates.hpp"

#include <boost/test/tools/assertion_result.hpp>

#include <filesystem>
#include <optional>
#include <cstdlib>

using namespace dep0::testing;

struct TypecheckTestsFixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    std::optional<dep0::typecheck::module_t> pass_result;
    std::optional<dep0::typecheck::error_t> fail_result;

    boost::test_tools::predicate_result pass(std::filesystem::path);
    boost::test_tools::predicate_result fail(std::filesystem::path);

    template <typename... Args>
    static constexpr auto app_of(Args&&... args)
    {
        return dep0::testing::app_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto pi_of(Args&&... args)
    {
        return dep0::testing::pi_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto array_of(Args&&... args)
    {
        return dep0::testing::array_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto init_list_of(Args&&... args)
    {
        return dep0::testing::init_list_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto return_of(Args&&... args)
    {
        return dep0::testing::return_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto subscript_of(Args&&... args)
    {
        return dep0::testing::subscript_of<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto lt(Args&&... args)
    {
        return dep0::testing::lt<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto plus(Args&&... args)
    {
        return dep0::testing::plus<dep0::typecheck::properties_t>(std::forward<Args>(args)...);
    }

    template <dep0::testing::Predicate<dep0::typecheck::expr_t> F>
    static auto arg_of(F&& f, std::optional<std::string> name = std::nullopt)
    {
        return dep0::testing::arg_of<dep0::typecheck::properties_t>(std::forward<F>(f), std::move(name));
    }

    template <typename... Args>
    static auto is_arg(dep0::typecheck::func_arg_t const& arg, Args&&... args)
    {
        return dep0::testing::is_arg<dep0::typecheck::properties_t>(arg, std::forward<Args>(args)...);
    }

    template <dep0::testing::Predicate<dep0::typecheck::expr_t> F>
    static auto is_expr_of(dep0::typecheck::sort_t const& x, F&& f)
    {
        auto const expr = std::get_if<dep0::typecheck::expr_t>(&x);
        if (not expr)
            return failure("sort is not expr_t but ", pretty_name(x));
        return std::forward<F>(f)(*expr);
    }

};
