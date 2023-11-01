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

    static constexpr auto bool_ = type_expr_of<dep0::typecheck::properties_t>(dep0::testing::is_type_bool);
    static constexpr auto i32 = type_expr_of<dep0::typecheck::properties_t>(dep0::testing::is_type_i32);
    static constexpr auto u32 = type_expr_of<dep0::typecheck::properties_t>(dep0::testing::is_type_u32);
    static inline auto int_() { return type_expr_of<dep0::typecheck::properties_t>(dep0::testing::type_var("int")); }

    template <dep0::testing::Predicate<dep0::typecheck::type_t> F>
    static auto term_binder(F&& f)
    {
        return dep0::testing::term_binder<dep0::typecheck::properties_t>(std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::typecheck::type_t> F>
    static auto term_binder(std::string const& name, F&& f)
    {
        return dep0::testing::term_binder<dep0::typecheck::properties_t>(name, std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::typecheck::type_t> F>
    static auto type_of(F&& f)
    {
        return dep0::testing::type_of<dep0::typecheck::properties_t>(std::forward<F>(f));
    }

    template <dep0::testing::Predicate<dep0::typecheck::func_arg_t> F>
    static auto is_arg(dep0::typecheck::func_arg_t const& arg, F&& f)
    {
        return dep0::testing::is_arg<dep0::typecheck::properties_t>(arg, std::forward<F>(f));
    }
};
