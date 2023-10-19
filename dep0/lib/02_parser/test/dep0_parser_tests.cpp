#define BOOST_TEST_MODULE dep0_parser_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"

#include "dep0/testing/ast_predicates.hpp"

#include <boost/type_index.hpp>

#include <cstdlib>
#include <filesystem>
#include <type_traits>

using namespace dep0::testing;

namespace dep0 {
std::ostream& operator<<(std::ostream& os, std::optional<dep0::source_text> const& x)
{
    return os << (x ? x->view() : "nullopt");
}
} // namespace dep0

namespace dep0::ast {
std::ostream& operator<<(std::ostream& os, sign_t const& x)
{
    return os << (x == sign_t::unsigned_v ? "unsigned" : "signed");
}

std::ostream& operator<<(std::ostream& os, width_t const& x)
{
    return os << (
        x == width_t::_8 ? "8" :
        x == width_t::_16 ? "16" :
        x == width_t::_32 ? "32" :
        x == width_t::_64 ? "64" :
        "n/a");
}
} // namespace dep0::ast

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    std::optional<dep0::parser::module_t> pass_result;

    boost::test_tools::predicate_result pass(std::filesystem::path const file)
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

    boost::test_tools::predicate_result fail(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        auto res = boost::test_tools::predicate_result(parse_result.has_error());
        if (not res)
            res.message() << "Was expecting parsing to fail but it succeeded";
        return res;
    }

    static constexpr auto bool_ = type_expr_of<dep0::parser::properties_t>(is_type_bool);
    static constexpr auto i32 = type_expr_of<dep0::parser::properties_t>(is_type_i32);
    static constexpr auto u32 = type_expr_of<dep0::parser::properties_t>(is_type_u32);

    static auto type_binder(std::string const& s)
    {
        return dep0::testing::type_binder<dep0::parser::properties_t>(s);
    }

    template <dep0::testing::Predicate<dep0::parser::type_t> F>
    static auto arg_of_type(F&& f)
    {
        return dep0::testing::arg_of_type<dep0::parser::properties_t>(std::forward<F>(f));
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_parser_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001)
{
    BOOST_TEST_REQUIRE(pass("test_0001.depc"));
    BOOST_TEST(pass_result->func_defs.size() == 1ul);
}
BOOST_AUTO_TEST_CASE(test_0002) { BOOST_TEST(pass("test_0002.depc")); }
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004) { BOOST_TEST(pass("test_0004.depc")); }
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008) { BOOST_TEST(pass("test_0008.depc")); }
BOOST_AUTO_TEST_CASE(test_0009) { BOOST_TEST(pass("test_0009.depc")); }
BOOST_AUTO_TEST_CASE(test_0010) { BOOST_TEST(pass("test_0010.depc")); }
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012) { BOOST_TEST(pass("test_0012.depc")); }
BOOST_AUTO_TEST_CASE(test_0013) { BOOST_TEST(pass("test_0013.depc")); }
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(pass("test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(pass("test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("test_0022.depc")); }
BOOST_AUTO_TEST_CASE(test_0023) { BOOST_TEST(pass("test_0023.depc")); }
BOOST_AUTO_TEST_CASE(test_0024) { BOOST_TEST(pass("test_0024.depc")); }
BOOST_AUTO_TEST_CASE(test_0025) { BOOST_TEST(pass("test_0025.depc")); }
BOOST_AUTO_TEST_CASE(test_0026) { BOOST_TEST(pass("test_0026.depc")); }
BOOST_AUTO_TEST_CASE(test_0027) { BOOST_TEST(pass("test_0027.depc")); }
BOOST_AUTO_TEST_CASE(test_0028) { BOOST_TEST(pass("test_0028.depc")); }
BOOST_AUTO_TEST_CASE(test_0029) { BOOST_TEST(pass("test_0029.depc")); }
BOOST_AUTO_TEST_CASE(test_0030) { BOOST_TEST(pass("test_0030.depc")); }
BOOST_AUTO_TEST_CASE(test_0031) { BOOST_TEST(pass("test_0031.depc")); }
BOOST_AUTO_TEST_CASE(test_0032) { BOOST_TEST(pass("test_0032.depc")); }
BOOST_AUTO_TEST_CASE(test_0033) { BOOST_TEST(pass("test_0033.depc")); }
BOOST_AUTO_TEST_CASE(test_0034) { BOOST_TEST(pass("test_0034.depc")); }
BOOST_AUTO_TEST_CASE(test_0035) { BOOST_TEST(pass("test_0035.depc")); }
BOOST_AUTO_TEST_CASE(test_0036) { BOOST_TEST(pass("test_0036.depc")); }
BOOST_AUTO_TEST_CASE(test_0037) { BOOST_TEST(pass("test_0037.depc")); }
BOOST_AUTO_TEST_CASE(test_0038) { BOOST_TEST(pass("test_0038.depc")); }
BOOST_AUTO_TEST_CASE(test_0039) { BOOST_TEST(pass("test_0039.depc")); }
BOOST_AUTO_TEST_CASE(test_0040) { BOOST_TEST(pass("test_0040.depc")); }
BOOST_AUTO_TEST_CASE(test_0041) { BOOST_TEST(pass("test_0041.depc")); }
BOOST_AUTO_TEST_CASE(test_0042) { BOOST_TEST(pass("test_0042.depc")); }
BOOST_AUTO_TEST_CASE(test_0043) { BOOST_TEST(pass("test_0043.depc")); }
BOOST_AUTO_TEST_CASE(test_0044) { BOOST_TEST(pass("test_0044.depc")); }
BOOST_AUTO_TEST_CASE(test_0045) { BOOST_TEST(pass("test_0045.depc")); }
BOOST_AUTO_TEST_CASE(test_0046) { BOOST_TEST(pass("test_0046.depc")); }
BOOST_AUTO_TEST_CASE(test_0047) { BOOST_TEST(pass("test_0047.depc")); }
BOOST_AUTO_TEST_CASE(test_0048) { BOOST_TEST(pass("test_0048.depc")); }
BOOST_AUTO_TEST_CASE(test_0049) { BOOST_TEST(pass("test_0049.depc")); }
BOOST_AUTO_TEST_CASE(test_0050) { BOOST_TEST(pass("test_0050.depc")); }
BOOST_AUTO_TEST_CASE(test_0051) { BOOST_TEST(pass("test_0051.depc")); }
BOOST_AUTO_TEST_CASE(test_0052) { BOOST_TEST(pass("test_0052.depc")); }
BOOST_AUTO_TEST_CASE(test_0053) { BOOST_TEST(pass("test_0053.depc")); }
BOOST_AUTO_TEST_CASE(test_0054) { BOOST_TEST(pass("test_0054.depc")); }
BOOST_AUTO_TEST_CASE(test_0055) { BOOST_TEST(pass("test_0055.depc")); }
BOOST_AUTO_TEST_CASE(test_0056) { BOOST_TEST(pass("test_0056.depc")); }
BOOST_AUTO_TEST_CASE(test_0057) { BOOST_TEST(pass("test_0057.depc")); }
BOOST_AUTO_TEST_CASE(test_0058) { BOOST_TEST(pass("test_0058.depc")); }
BOOST_AUTO_TEST_CASE(test_0059) { BOOST_TEST(pass("test_0059.depc")); }
BOOST_AUTO_TEST_CASE(test_0060) { BOOST_TEST(pass("test_0060.depc")); }
BOOST_AUTO_TEST_CASE(test_0061) { BOOST_TEST(pass("test_0061.depc")); }
BOOST_AUTO_TEST_CASE(test_0062) { BOOST_TEST(pass("test_0062.depc")); }
BOOST_AUTO_TEST_CASE(test_0063) { BOOST_TEST(pass("test_0063.depc")); }
BOOST_AUTO_TEST_CASE(test_0064) { BOOST_TEST(pass("test_0064.depc")); }
BOOST_AUTO_TEST_CASE(test_0065) { BOOST_TEST(pass("test_0065.depc")); }
BOOST_AUTO_TEST_CASE(test_0066) { BOOST_TEST(pass("test_0066.depc")); }
BOOST_AUTO_TEST_CASE(test_0067) { BOOST_TEST(pass("test_0067.depc")); }
BOOST_AUTO_TEST_CASE(test_0068) { BOOST_TEST(pass("test_0068.depc")); }
BOOST_AUTO_TEST_CASE(test_0069) { BOOST_TEST(pass("test_0069.depc")); }
BOOST_AUTO_TEST_CASE(test_0070) { BOOST_TEST(pass("test_0070.depc")); }
BOOST_AUTO_TEST_CASE(test_0071) { BOOST_TEST(pass("test_0071.depc")); }
BOOST_AUTO_TEST_CASE(test_0072) { BOOST_TEST(pass("test_0072.depc")); }
BOOST_AUTO_TEST_CASE(test_0073) { BOOST_TEST(pass("test_0073.depc")); }
BOOST_AUTO_TEST_CASE(test_0074) { BOOST_TEST(pass("test_0074.depc")); }
BOOST_AUTO_TEST_CASE(test_0075) { BOOST_TEST(pass("test_0075.depc")); }
BOOST_AUTO_TEST_CASE(test_0076) { BOOST_TEST(pass("test_0076.depc")); }
BOOST_AUTO_TEST_CASE(test_0077) { BOOST_TEST(pass("test_0077.depc")); }
BOOST_AUTO_TEST_CASE(test_0078) { BOOST_TEST(pass("test_0078.depc")); }
BOOST_AUTO_TEST_CASE(test_0079) { BOOST_TEST(pass("test_0079.depc")); }
BOOST_AUTO_TEST_CASE(test_0080) { BOOST_TEST(pass("test_0080.depc")); }
BOOST_AUTO_TEST_CASE(test_0081) { BOOST_TEST(pass("test_0081.depc")); }
BOOST_AUTO_TEST_CASE(test_0082) { BOOST_TEST(pass("test_0082.depc")); }
BOOST_AUTO_TEST_CASE(test_0083) { BOOST_TEST(pass("test_0083.depc")); }
BOOST_AUTO_TEST_CASE(test_0084) { BOOST_TEST(pass("test_0084.depc")); }
BOOST_AUTO_TEST_CASE(test_0085) { BOOST_TEST(pass("test_0085.depc")); }
BOOST_AUTO_TEST_CASE(test_0086) { BOOST_TEST(pass("test_0086.depc")); }
BOOST_AUTO_TEST_CASE(test_0087) { BOOST_TEST(pass("test_0087.depc")); }
BOOST_AUTO_TEST_CASE(test_0088) { BOOST_TEST(pass("test_0088.depc")); }
BOOST_AUTO_TEST_CASE(test_0089) { BOOST_TEST(pass("test_0089.depc")); }
BOOST_AUTO_TEST_CASE(test_0090) { BOOST_TEST(pass("test_0090.depc")); }
BOOST_AUTO_TEST_CASE(test_0091) { BOOST_TEST(pass("test_0091.depc")); }
BOOST_AUTO_TEST_CASE(test_0092) { BOOST_TEST(pass("test_0092.depc")); }
BOOST_AUTO_TEST_CASE(test_0093) { BOOST_TEST(pass("test_0093.depc")); }
BOOST_AUTO_TEST_CASE(test_0094) { BOOST_TEST(pass("test_0094.depc")); }
BOOST_AUTO_TEST_CASE(test_0095) { BOOST_TEST(pass("test_0095.depc")); }
BOOST_AUTO_TEST_CASE(test_0096) { BOOST_TEST(pass("test_0096.depc")); }
BOOST_AUTO_TEST_CASE(test_0097) { BOOST_TEST(pass("test_0097.depc")); }
BOOST_AUTO_TEST_CASE(test_0098) { BOOST_TEST(pass("test_0098.depc")); }
BOOST_AUTO_TEST_CASE(test_0099) { BOOST_TEST(pass("test_0099.depc")); }
BOOST_AUTO_TEST_CASE(test_0100) { BOOST_TEST(pass("test_0100.depc")); }
BOOST_AUTO_TEST_CASE(test_0101) { BOOST_TEST(pass("test_0101.depc")); }
BOOST_AUTO_TEST_CASE(test_0102) { BOOST_TEST(pass("test_0102.depc")); }
BOOST_AUTO_TEST_CASE(test_0103) { BOOST_TEST(pass("test_0103.depc")); }
BOOST_AUTO_TEST_CASE(test_0104) { BOOST_TEST(pass("test_0104.depc")); }
BOOST_AUTO_TEST_CASE(test_0105) { BOOST_TEST(pass("test_0105.depc")); }
BOOST_AUTO_TEST_CASE(test_0106) { BOOST_TEST(pass("test_0106.depc")); }
BOOST_AUTO_TEST_CASE(test_0107) { BOOST_TEST(pass("test_0107.depc")); }
BOOST_AUTO_TEST_CASE(test_0108) { BOOST_TEST(pass("test_0108.depc")); }
BOOST_AUTO_TEST_CASE(test_0109) { BOOST_TEST(pass("test_0109.depc")); }
BOOST_AUTO_TEST_CASE(test_0110) { BOOST_TEST(pass("test_0110.depc")); }
BOOST_AUTO_TEST_CASE(test_0111) { BOOST_TEST(pass("test_0111.depc")); }
BOOST_AUTO_TEST_CASE(test_0112) { BOOST_TEST(pass("test_0112.depc")); }
BOOST_AUTO_TEST_CASE(test_0113) { BOOST_TEST(pass("test_0113.depc")); }
BOOST_AUTO_TEST_CASE(test_0114) { BOOST_TEST(pass("test_0114.depc")); }
BOOST_AUTO_TEST_CASE(test_0115) { BOOST_TEST(pass("test_0115.depc")); }
BOOST_AUTO_TEST_CASE(test_0116) { BOOST_TEST(pass("test_0116.depc")); }
BOOST_AUTO_TEST_CASE(test_0117) { BOOST_TEST(pass("test_0117.depc")); }
BOOST_AUTO_TEST_CASE(test_0118) { BOOST_TEST(pass("test_0118.depc")); }
BOOST_AUTO_TEST_CASE(test_0119) { BOOST_TEST(pass("test_0119.depc")); }
BOOST_AUTO_TEST_CASE(test_0120) { BOOST_TEST(pass("test_0120.depc")); }
BOOST_AUTO_TEST_CASE(test_0121) { BOOST_TEST(pass("test_0121.depc")); }
BOOST_AUTO_TEST_CASE(test_0122) { BOOST_TEST(pass("test_0122.depc")); }
BOOST_AUTO_TEST_CASE(test_0123) { BOOST_TEST(pass("test_0123.depc")); }
BOOST_AUTO_TEST_CASE(test_0124) { BOOST_TEST(pass("test_0124.depc")); }
BOOST_AUTO_TEST_CASE(test_0125) { BOOST_TEST(pass("test_0125.depc")); }
BOOST_AUTO_TEST_CASE(test_0126) { BOOST_TEST(pass("test_0126.depc")); }
BOOST_AUTO_TEST_CASE(test_0127) { BOOST_TEST(pass("test_0127.depc")); }
BOOST_AUTO_TEST_CASE(test_0128) { BOOST_TEST(pass("test_0128.depc")); }
BOOST_AUTO_TEST_CASE(test_0129) { BOOST_TEST(pass("test_0129.depc")); }
BOOST_AUTO_TEST_CASE(test_0130) { BOOST_TEST(pass("test_0130.depc")); }
BOOST_AUTO_TEST_CASE(test_0131) { BOOST_TEST(pass("test_0131.depc")); }
BOOST_AUTO_TEST_CASE(test_0132) { BOOST_TEST(pass("test_0132.depc")); }
BOOST_AUTO_TEST_CASE(test_0133) { BOOST_TEST(pass("test_0133.depc")); }
BOOST_AUTO_TEST_CASE(test_0134) { BOOST_TEST(pass("test_0134.depc")); }
BOOST_AUTO_TEST_CASE(test_0135) { BOOST_TEST(pass("test_0135.depc")); }
BOOST_AUTO_TEST_CASE(test_0136) { BOOST_TEST(pass("test_0136.depc")); }
BOOST_AUTO_TEST_CASE(test_0137) { BOOST_TEST(pass("test_0137.depc")); }
BOOST_AUTO_TEST_CASE(test_0138) { BOOST_TEST(pass("test_0138.depc")); }
BOOST_AUTO_TEST_CASE(test_0139) { BOOST_TEST(pass("test_0139.depc")); }
BOOST_AUTO_TEST_CASE(test_0140) { BOOST_TEST(pass("test_0140.depc")); }
BOOST_AUTO_TEST_CASE(test_0141) { BOOST_TEST(pass("test_0141.depc")); }
BOOST_AUTO_TEST_CASE(test_0142) { BOOST_TEST(pass("test_0142.depc")); }
BOOST_AUTO_TEST_CASE(test_0143) { BOOST_TEST(pass("test_0143.depc")); }
BOOST_AUTO_TEST_CASE(test_0144) { BOOST_TEST(pass("test_0144.depc")); }
BOOST_AUTO_TEST_CASE(test_0145) { BOOST_TEST(pass("test_0145.depc")); }
BOOST_AUTO_TEST_CASE(test_0146) { BOOST_TEST(pass("test_0146.depc")); }
BOOST_AUTO_TEST_CASE(test_0147) { BOOST_TEST(pass("test_0147.depc")); }
BOOST_AUTO_TEST_CASE(test_0148) { BOOST_TEST(pass("test_0148.depc")); }
BOOST_AUTO_TEST_CASE(test_0149) { BOOST_TEST(pass("test_0149.depc")); }
BOOST_AUTO_TEST_CASE(test_0150) { BOOST_TEST(pass("test_0150.depc")); }
BOOST_AUTO_TEST_CASE(test_0151) { BOOST_TEST(fail("test_0151.depc")); }
BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("test_0152.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
    auto const integer_defs_0 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
    auto const integer_defs_1 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[1].value);
    auto const integer_defs_2 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[2].value);
    auto const integer_defs_3 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[3].value);
    auto const integer_defs_4 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[4].value);
    auto const integer_defs_5 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[5].value);
    auto const integer_defs_6 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[6].value);
    auto const integer_defs_7 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[7].value);
    auto const integer_defs_8 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[8].value);
    auto const integer_defs_9 = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[9].value);
    BOOST_TEST_REQUIRE(integer_defs_0);
    BOOST_TEST_REQUIRE(integer_defs_1);
    BOOST_TEST_REQUIRE(integer_defs_2);
    BOOST_TEST_REQUIRE(integer_defs_3);
    BOOST_TEST_REQUIRE(integer_defs_4);
    BOOST_TEST_REQUIRE(integer_defs_5);
    BOOST_TEST_REQUIRE(integer_defs_6);
    BOOST_TEST_REQUIRE(integer_defs_7);
    BOOST_TEST_REQUIRE(integer_defs_8);
    BOOST_TEST_REQUIRE(integer_defs_9);
    BOOST_TEST(integer_defs_0->name == "hours_t");
    BOOST_TEST(integer_defs_0->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_0->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_0->max_abs_value.has_value());
    BOOST_TEST(integer_defs_0->max_abs_value == "23");
    BOOST_TEST(integer_defs_1->name == "minutes_t");
    BOOST_TEST(integer_defs_1->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_1->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_1->max_abs_value == "59");
    BOOST_TEST(integer_defs_2->name == "seconds_t");
    BOOST_TEST(integer_defs_2->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_2->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_2->max_abs_value == "59");
    BOOST_TEST(integer_defs_3->name == "millis_t");
    BOOST_TEST(integer_defs_3->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_3->width == dep0::ast::width_t::_16);
    BOOST_TEST(integer_defs_3->max_abs_value == "999");
    BOOST_TEST(integer_defs_4->name == "nanos_t");
    BOOST_TEST(integer_defs_4->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_4->width == dep0::ast::width_t::_32);
    BOOST_TEST(integer_defs_4->max_abs_value == "999'999'999");
    BOOST_TEST(integer_defs_5->name == "duration_t");
    BOOST_TEST(integer_defs_5->sign == dep0::ast::sign_t::signed_v);
    BOOST_TEST(integer_defs_5->width == dep0::ast::width_t::_64);
    BOOST_TEST(integer_defs_5->max_abs_value.has_value() == false);
    BOOST_TEST(integer_defs_6->name == "ascii_t");
    BOOST_TEST(integer_defs_6->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_6->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_6->max_abs_value == "127");
    BOOST_TEST(integer_defs_7->name == "sign_t");
    BOOST_TEST(integer_defs_7->sign == dep0::ast::sign_t::signed_v);
    BOOST_TEST(integer_defs_7->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_7->max_abs_value == "1");
    BOOST_TEST(integer_defs_8->name == "signal_t");
    BOOST_TEST(integer_defs_8->sign == dep0::ast::sign_t::signed_v);
    BOOST_TEST(integer_defs_8->width == dep0::ast::width_t::_8);
    BOOST_TEST(integer_defs_8->max_abs_value == "15");
    BOOST_TEST(integer_defs_9->name == "key_t");
    BOOST_TEST(integer_defs_9->sign == dep0::ast::sign_t::unsigned_v);
    BOOST_TEST(integer_defs_9->width == dep0::ast::width_t::_64);
    BOOST_TEST(integer_defs_9->max_abs_value.has_value() == false);
}
BOOST_AUTO_TEST_CASE(test_0153) { BOOST_TEST(pass("test_0153.depc")); }
BOOST_AUTO_TEST_CASE(test_0154) { BOOST_TEST(fail("test_0154.depc")); }
BOOST_AUTO_TEST_CASE(test_0155) { BOOST_TEST(fail("test_0155.depc")); }
BOOST_AUTO_TEST_CASE(test_0156) { BOOST_TEST(fail("test_0156.depc")); }
BOOST_AUTO_TEST_CASE(test_0157) { BOOST_TEST(fail("test_0157.depc")); }
BOOST_AUTO_TEST_CASE(test_0158) { BOOST_TEST(fail("test_0158.depc")); }
BOOST_AUTO_TEST_CASE(test_0159) { BOOST_TEST(fail("test_0159.depc")); }
BOOST_AUTO_TEST_CASE(test_0160) { BOOST_TEST(fail("test_0160.depc")); }
BOOST_AUTO_TEST_CASE(test_0161) { BOOST_TEST(fail("test_0161.depc")); }
BOOST_AUTO_TEST_CASE(test_0162) { BOOST_TEST(fail("test_0162.depc")); }
BOOST_AUTO_TEST_CASE(test_0163) { BOOST_TEST(pass("test_0163.depc")); }
BOOST_AUTO_TEST_CASE(test_0164) { BOOST_TEST(pass("test_0164.depc")); }
BOOST_AUTO_TEST_CASE(test_0165) { BOOST_TEST(fail("test_0165.depc")); }
BOOST_AUTO_TEST_CASE(test_0166) { BOOST_TEST(fail("test_0166.depc")); }
BOOST_AUTO_TEST_CASE(test_0167)
{
    BOOST_TEST_REQUIRE(pass("test_0167.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
    BOOST_TEST(is_type_i32(f.value.ret_type));
    auto const& arg = f.value.args[0];
    BOOST_TEST(arg.name.txt == "x");
    BOOST_TEST(is_type_of(arg.sort, is_type_i32));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var(("x"))));
}

BOOST_AUTO_TEST_CASE(test_0168)
{
    BOOST_TEST_REQUIRE(pass("test_0168.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_app_of(expr, var("id"), is_zero);
    }));
}

BOOST_AUTO_TEST_CASE(test_0169)
{
    BOOST_TEST_REQUIRE(pass("test_0169.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_app_of(expr, var("first"), is_zero, numeric_constant("1"));
    }));
}
BOOST_AUTO_TEST_CASE(test_0170) { BOOST_TEST(pass("test_0170.depc")); }
BOOST_AUTO_TEST_CASE(test_0171) { BOOST_TEST(pass("test_0171.depc")); }
BOOST_AUTO_TEST_CASE(test_0172)
{
    BOOST_TEST_REQUIRE(pass("test_0172.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 2ul);
    BOOST_TEST(is_func_call_of(f.value.body.stmts[0ul], var("first"), is_zero, numeric_constant("1")));
    BOOST_TEST(is_return_of(f.value.body.stmts[1ul], is_zero));
}

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("test_0173.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("test_0174.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("test_0175.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("+2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("test_0176.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "minus_one");
    BOOST_TEST(is_type_i32(f.value.ret_type));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("-2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0177)
{
    BOOST_TEST_REQUIRE(pass("test_0177.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "minus_one");
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, numeric_constant("1"), numeric_constant("-2"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0178)
{
    BOOST_TEST_REQUIRE(pass("test_0178.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 24ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "x");
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), i32, var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "h");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("id"), var("int"), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "apply");
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("x"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "apply_f");
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), i32, var("f"), numeric_constant("-1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "apply_g");
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), u32, var("g"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "apply_h");
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), var("int"), var("h"), numeric_constant("1"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "apply_0");
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("apply"), numeric_constant("0"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "discard_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "f");
        BOOST_TEST(f.value.args[2ul].name.txt == "x");
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), arg_of_type(type_var("u"))}, type_var("u"));
        }));
        BOOST_TEST(is_type_of(f.value.args[2ul].sort, type_var("t")));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "discard_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "f");
        BOOST_TEST(f.value.args[2ul].name.txt == "x");
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("t"), arg_of_type(type_var("t"))}, type_var("t"));
        }));
        BOOST_TEST(is_type_of(f.value.args[2ul].sort, type_var("t")));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[14ul];
        BOOST_TEST(f.name == "discard_id_v1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v1"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[15ul];
        BOOST_TEST(f.name == "discard_id_v2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_u32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("discard_v2"), u32, var("id"), is_zero);
        }));
    }
    {
        auto const& f = pass_result->func_defs[17ul];
        BOOST_TEST(f.name == "multi_f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 5ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "f");
        BOOST_TEST(f.value.args[2ul].name.txt == "x");
        BOOST_TEST(f.value.args[3ul].name.txt == "y");
        BOOST_TEST(f.value.args[4ul].name.txt == "z");
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, [] (dep0::parser::type_t const& t)
        {
            return is_arr_of(t, std::tuple{type_binder("u"), arg_of_type(type_var("u"))}, type_var("u"));
        }));
        BOOST_TEST(is_type_of(f.value.args[2ul].sort, type_var("t")));
        BOOST_TEST(is_type_of(f.value.args[3ul].sort, type_var("t")));
        BOOST_TEST(is_type_of(f.value.args[4ul].sort, type_var("t")));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const if_1 = std::get_if<dep0::parser::stmt_t::if_else_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_1);
        BOOST_TEST(is_app_of(if_1->cond, var("f"), bool_, boolean_constant(true)));
        BOOST_TEST_REQUIRE(if_1->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_1->false_branch->stmts.size() == 1ul);
        auto const if_2 = std::get_if<dep0::parser::stmt_t::if_else_t>(&if_1->false_branch->stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_2);
        BOOST_TEST(is_app_of(if_2->cond, var("int_to_bool"), [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("int"), is_zero);
        }));
        BOOST_TEST_REQUIRE(if_2->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_2->false_branch->stmts.size() == 1ul);
        BOOST_TEST(is_return_of(if_2->false_branch->stmts[0], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("t"), var("z"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[19ul];
        BOOST_TEST(f.name == "get_id");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_arr_of(f.value.ret_type, std::tuple{type_binder("t"), arg_of_type(type_var("t"))}, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("id")));
    }
    {
        auto const& f = pass_result->func_defs[22ul];
        BOOST_TEST(f.name == "apply_id_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "x");
        BOOST_TEST(is_type_i32(f.value.ret_type));
    }
    {
        auto const& f = pass_result->func_defs[23ul];
        BOOST_TEST(f.name == "apply_id_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "x");
        BOOST_TEST(is_type_i32(f.value.ret_type));
    }
}

BOOST_AUTO_TEST_CASE(test_0179)
{
    BOOST_TEST_REQUIRE(pass("test_0179.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);

    auto const& succ = pass_result->func_defs[0ul];
    BOOST_TEST(succ.name == "succ");
    BOOST_TEST_REQUIRE(succ.value.args.size() == 2ul);
    BOOST_TEST(succ.value.args[0ul].name.txt == "t");
    BOOST_TEST(succ.value.args[1ul].name.txt == "x");
    BOOST_TEST(std::holds_alternative<dep0::ast::typename_t>(succ.value.args[0ul].sort));
    BOOST_TEST(is_type_of(succ.value.args[1ul].sort, type_var("t")));
    BOOST_TEST_REQUIRE(succ.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(succ.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, var("x"), numeric_constant("1"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0180)
{
    BOOST_TEST_REQUIRE(pass("test_0180.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);

    auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(t->name == "t");

    auto const& succ = pass_result->func_defs[0ul];
    BOOST_TEST(succ.name == "succ");
    BOOST_TEST_REQUIRE(succ.value.args.size() == 2ul);
    BOOST_TEST(succ.value.args[0ul].name.txt == "t");
    BOOST_TEST(succ.value.args[1ul].name.txt == "x");
    BOOST_TEST(is_typename(succ.value.args[0ul].sort));
    BOOST_TEST(is_type_of(succ.value.args[1ul].sort, type_var("t")));
    BOOST_TEST_REQUIRE(succ.value.body.stmts.size() == 1ul);
    BOOST_TEST(is_return_of(succ.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
    {
        return is_plus(expr, var("x"), numeric_constant("1"));
    }));
}

BOOST_AUTO_TEST_CASE(test_0181)
{
    BOOST_TEST_REQUIRE(pass("test_0181.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);

    {
        auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(t->name == "int");
    }
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "int");
        BOOST_TEST(f.value.args[2ul].name.txt == "x");
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_typename(f.value.args[1ul].sort));
        BOOST_TEST(is_type_of(f.value.args[2ul].sort, type_var("t")));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& g = pass_result->func_defs[1ul];
        BOOST_TEST(g.value.args.empty());
        BOOST_TEST(is_type_var(g.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(g.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(g.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("int"), i32, is_zero);
        }));
    }
}

BOOST_AUTO_TEST_CASE(test_0182)
{
    BOOST_TEST_REQUIRE(pass("test_0182.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);

    {
        auto const t = std::get_if<dep0::parser::type_def_t::integer_t>(&pass_result->type_defs[0].value);
        BOOST_TEST_REQUIRE(t);
        BOOST_TEST(t->name == "int");
    }
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "t");
        BOOST_TEST(f.value.args[1ul].name.txt == "x");
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "u");
        BOOST_TEST(f.value.args[1ul].name.txt == "f");
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST(is_typename(f.value.args[0ul].sort));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, [] (dep0::parser::type_t const& type)
        {
            auto const arr = std::get_if<dep0::parser::type_t::arr_t>(&type.value);
            BOOST_TEST_REQUIRE(arr);
            auto const arg_type0 = std::get_if<dep0::parser::type_t::var_t>(&arr->arg_types[0]);
            auto const arg_type1 = std::get_if<dep0::parser::type_t>(&arr->arg_types[1]);
            BOOST_TEST_REQUIRE(arg_type0);
            BOOST_TEST_REQUIRE(arg_type1);
            BOOST_TEST(arg_type0->name.txt == "int");
            BOOST_TEST(is_type_var(*arg_type1, "int"));
            BOOST_TEST(is_type_var(arr->ret_type.get(), "u"));
            return boost::test_tools::predicate_result(true);
        }));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], is_zero));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(is_type_var(f.value.ret_type, "int"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), var("int"), var("id"));
        }));
    }
}

BOOST_AUTO_TEST_CASE(test_0183)
{
    BOOST_TEST_REQUIRE(pass("test_0183.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 3ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "x");
        BOOST_TEST(f.value.args[1ul].name.txt == "y");
        BOOST_TEST(is_type_of(f.value.args[0ul].sort, is_type_i32));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, is_type_i32));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], var("x")));
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "g");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name.txt == "x");
        BOOST_TEST(f.value.args[1ul].name.txt == "y");
        BOOST_TEST(is_type_of(f.value.args[0ul].sort, is_type_i32));
        BOOST_TEST(is_type_of(f.value.args[1ul].sort, is_type_i32));
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("f"), var("y"), numeric_constant("2"));
        }));
    }
    {
        auto const& f = pass_result->func_defs[2ul];
        BOOST_TEST(f.name == "zero");
        BOOST_TEST(f.value.args.empty());
        BOOST_TEST(is_type_i32(f.value.ret_type));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        BOOST_TEST(is_return_of(f.value.body.stmts[0ul], [] (dep0::parser::expr_t const& expr)
        {
            return is_app_of(expr, var("g"), numeric_constant("1"), numeric_constant("0"));
        }));
    }
}

BOOST_AUTO_TEST_SUITE_END()
