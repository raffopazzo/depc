#pragma once

#include "dep0/parser/ast.hpp"
#include "dep0/testing/ast_predicates.hpp"

#include "dep0/match.hpp"

#include <boost/test/tools/assertion_result.hpp>

#include <cstdlib>
#include <optional>
#include <filesystem>

struct ParserTestsFixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");

    /**
     * Historically `pass_result` was just `dep0::parser::module_t`;
     * when definitions and declarations were replaced by a variant
     * to allow definitions and declarations in any order,
     * there was the need to avoid breaking all tests;
     * the solution adopted was to introduce `pass_result_t` and
     * store definitions and declarations in member fields that
     * had the same name as the old `module_t`.
     * This arguably incurs unnecessary copies but the result
     * is also more ergonomic even for new tests that only have
     * function definitions, since they are all in `func_defs`.
     * See https://github.com/raffopazzo/depc/pull/29
     */
    struct pass_result_t
    {
        dep0::source_loc_t properties;
        std::vector<dep0::parser::module_t::entry_t> entries;
        std::vector<dep0::parser::type_def_t> type_defs;
        std::vector<dep0::parser::func_decl_t> func_decls;
        std::vector<dep0::parser::func_def_t> func_defs;

        explicit pass_result_t(dep0::parser::module_t m)
            : properties(m.properties)
            , entries(std::move(m.entries))
        {
            for (auto const& x: entries)
                dep0::match(
                    x,
                    [this] (dep0::parser::type_def_t const& x) { type_defs.push_back(x); },
                    [this] (dep0::parser::func_decl_t const& x) { func_decls.push_back(x); },
                    [this] (dep0::parser::func_def_t const& x) { func_defs.push_back(x); });
        }
    };
    std::optional<pass_result_t> pass_result;

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
    static constexpr auto array_of(Args&&... args)
    {
        return dep0::testing::array_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto init_list_of(Args&&... args)
    {
        return dep0::testing::init_list_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto return_of(Args&&... args)
    {
        return dep0::testing::return_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto subscript_of(Args&&... args)
    {
        return dep0::testing::subscript_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto not_of(Args&&... args)
    {
        return dep0::testing::not_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto and_of(Args&&... args)
    {
        return dep0::testing::and_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto or_of(Args&&... args)
    {
        return dep0::testing::or_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto xor_of(Args&&... args)
    {
        return dep0::testing::xor_of<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto eq(Args&&... args)
    {
        return dep0::testing::eq<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto neq(Args&&... args)
    {
        return dep0::testing::neq<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto gt(Args&&... args)
    {
        return dep0::testing::gt<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto gte(Args&&... args)
    {
        return dep0::testing::gte<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto lt(Args&&... args)
    {
        return dep0::testing::lt<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto lte(Args&&... args)
    {
        return dep0::testing::lte<dep0::parser::properties_t>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static constexpr auto plus(Args&&... args)
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
