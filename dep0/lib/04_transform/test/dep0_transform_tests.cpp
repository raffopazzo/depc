#define BOOST_TEST_MODULE dep0_transform_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/transform/run.hpp"

#include "dep0/typecheck/check.hpp"

dep0::expected<std::true_type> fail(dep0::typecheck::module_t&) { return dep0::error_t{"Test failure"}; }
static_assert(dep0::transform::Transform<decltype(fail)>);

BOOST_AUTO_TEST_SUITE(dep0_transform_tests)

BOOST_AUTO_TEST_CASE(run_all)
{
    using namespace dep0;
    bool f_called = false;
    bool g_called = false;
    auto f = [&] (typecheck::module_t&) -> expected<std::true_type> { f_called = true; return std::true_type{}; };
    auto g = [&] (typecheck::module_t&) -> expected<std::true_type> { g_called = true; return std::true_type{}; };
    static_assert(transform::Transform<decltype(f)>);
    static_assert(transform::Transform<decltype(g)>);
    auto m = typecheck::check(parser::module_t{source_loc_t{0, 0, source_text::from_literal("")}}).value();
    auto result1 = transform::run(m, std::vector<transform::transform_t>{f, g});
    BOOST_TEST(result1.has_value());
    BOOST_TEST(f_called);
    BOOST_TEST(g_called);

    // also with parameter pack
    f_called = false;
    g_called = false;
    auto result2 = transform::run(m, f, g);
    BOOST_TEST(result2.has_value());
    BOOST_TEST(f_called);
    BOOST_TEST(g_called);
}

BOOST_AUTO_TEST_CASE(stop_at_first_error)
{
    using namespace dep0;
    bool f_called = false;
    auto f = [&] (typecheck::module_t&) -> expected<std::true_type> { f_called = true; return std::true_type{}; };
    static_assert(transform::Transform<decltype(f)>);
    auto m = typecheck::check(parser::module_t{source_loc_t{0, 0, source_text::from_literal("")}}).value();
    auto result1 = transform::run(m, std::vector<transform::transform_t>{fail, f});
    BOOST_TEST(result1.has_error());
    BOOST_TEST(result1.error().error == "Test failure");
    BOOST_TEST(not f_called);

    // also with parameter pack
    f_called = false;
    auto result2 = transform::run(m, fail, f);
    BOOST_TEST(result2.has_error());
    BOOST_TEST(result2.error().error == "Test failure");
    BOOST_TEST(not f_called);
}

BOOST_AUTO_TEST_SUITE_END()
