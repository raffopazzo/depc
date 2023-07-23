#define BOOST_TEST_MODULE dep0_typecheck_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    std::optional<dep0::typecheck::module_t> pass_result;

    boost::test_tools::predicate_result pass(std::filesystem::path const file)
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

    boost::test_tools::predicate_result fail(std::filesystem::path const file)
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
        return true;
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_typecheck_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("test_0001.depc")); }
BOOST_AUTO_TEST_CASE(test_0002) { BOOST_TEST(fail("test_0002.depc")); }
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
BOOST_AUTO_TEST_CASE(test_0004) { BOOST_TEST(fail("test_0004.depc")); }
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0005.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("test_0007.depc")); }
BOOST_AUTO_TEST_CASE(test_0008) { BOOST_TEST(fail("test_0008.depc")); }
BOOST_AUTO_TEST_CASE(test_0009) { BOOST_TEST(fail("test_0009.depc")); }
BOOST_AUTO_TEST_CASE(test_0010) { BOOST_TEST(fail("test_0010.depc")); }
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("test_0011.depc")); }
BOOST_AUTO_TEST_CASE(test_0012) { BOOST_TEST(fail("test_0012.depc")); }
BOOST_AUTO_TEST_CASE(test_0013) { BOOST_TEST(fail("test_0013.depc")); }
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("test_0015.depc")); }
BOOST_AUTO_TEST_CASE(test_0016) { BOOST_TEST(fail("test_0016.depc")); }
BOOST_AUTO_TEST_CASE(test_0017) { BOOST_TEST(fail("test_0017.depc")); }
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("test_0022.depc")); }
BOOST_AUTO_TEST_CASE(test_0023) { BOOST_TEST(fail("test_0023.depc")); }
BOOST_AUTO_TEST_CASE(test_0024) { BOOST_TEST(pass("test_0024.depc")); }
BOOST_AUTO_TEST_CASE(test_0025) { BOOST_TEST(fail("test_0025.depc")); }
BOOST_AUTO_TEST_CASE(test_0026) { BOOST_TEST(fail("test_0026.depc")); }
BOOST_AUTO_TEST_CASE(test_0027) { BOOST_TEST(fail("test_0027.depc")); }
BOOST_AUTO_TEST_CASE(test_0028) { BOOST_TEST(fail("test_0028.depc")); }
BOOST_AUTO_TEST_CASE(test_0029) { BOOST_TEST(fail("test_0029.depc")); }
BOOST_AUTO_TEST_CASE(test_0030) { BOOST_TEST(fail("test_0030.depc")); }
BOOST_AUTO_TEST_CASE(test_0031) { BOOST_TEST(fail("test_0031.depc")); }
BOOST_AUTO_TEST_CASE(test_0032) { BOOST_TEST(fail("test_0032.depc")); }
BOOST_AUTO_TEST_CASE(test_0033) { BOOST_TEST(fail("test_0033.depc")); }
BOOST_AUTO_TEST_CASE(test_0034) { BOOST_TEST(fail("test_0034.depc")); }
BOOST_AUTO_TEST_CASE(test_0035) { BOOST_TEST(fail("test_0035.depc")); }
BOOST_AUTO_TEST_CASE(test_0036) { BOOST_TEST(fail("test_0036.depc")); }
BOOST_AUTO_TEST_CASE(test_0037) { BOOST_TEST(fail("test_0037.depc")); }
BOOST_AUTO_TEST_CASE(test_0038) { BOOST_TEST(fail("test_0038.depc")); }
BOOST_AUTO_TEST_CASE(test_0039) { BOOST_TEST(fail("test_0039.depc")); }
BOOST_AUTO_TEST_CASE(test_0040) { BOOST_TEST(fail("test_0040.depc")); }
BOOST_AUTO_TEST_CASE(test_0041) { BOOST_TEST(fail("test_0041.depc")); }
BOOST_AUTO_TEST_CASE(test_0042) { BOOST_TEST(fail("test_0042.depc")); }
BOOST_AUTO_TEST_CASE(test_0043) { BOOST_TEST(fail("test_0043.depc")); }
BOOST_AUTO_TEST_CASE(test_0044) { BOOST_TEST(fail("test_0044.depc")); }
BOOST_AUTO_TEST_CASE(test_0045) { BOOST_TEST(fail("test_0045.depc")); }
BOOST_AUTO_TEST_CASE(test_0046) { BOOST_TEST(fail("test_0046.depc")); }
BOOST_AUTO_TEST_CASE(test_0047) { BOOST_TEST(fail("test_0047.depc")); }
BOOST_AUTO_TEST_CASE(test_0048) { BOOST_TEST(fail("test_0048.depc")); }
BOOST_AUTO_TEST_CASE(test_0049) { BOOST_TEST(fail("test_0049.depc")); }
BOOST_AUTO_TEST_CASE(test_0050) { BOOST_TEST(fail("test_0050.depc")); }
BOOST_AUTO_TEST_CASE(test_0051) { BOOST_TEST(fail("test_0051.depc")); }
BOOST_AUTO_TEST_CASE(test_0052) { BOOST_TEST(fail("test_0052.depc")); }
BOOST_AUTO_TEST_CASE(test_0053) { BOOST_TEST(fail("test_0053.depc")); }
BOOST_AUTO_TEST_CASE(test_0054) { BOOST_TEST(fail("test_0054.depc")); }
BOOST_AUTO_TEST_CASE(test_0055) { BOOST_TEST(fail("test_0055.depc")); }
BOOST_AUTO_TEST_CASE(test_0056) { BOOST_TEST(fail("test_0056.depc")); }
BOOST_AUTO_TEST_CASE(test_0057) { BOOST_TEST(fail("test_0057.depc")); }
BOOST_AUTO_TEST_CASE(test_0058) { BOOST_TEST(fail("test_0058.depc")); }
BOOST_AUTO_TEST_CASE(test_0059) { BOOST_TEST(fail("test_0059.depc")); }
BOOST_AUTO_TEST_CASE(test_0060) { BOOST_TEST(fail("test_0060.depc")); }
BOOST_AUTO_TEST_CASE(test_0061) { BOOST_TEST(fail("test_0061.depc")); }
BOOST_AUTO_TEST_CASE(test_0062) { BOOST_TEST(fail("test_0062.depc")); }
BOOST_AUTO_TEST_CASE(test_0063) { BOOST_TEST(fail("test_0063.depc")); }
BOOST_AUTO_TEST_CASE(test_0064) { BOOST_TEST(fail("test_0064.depc")); }
BOOST_AUTO_TEST_CASE(test_0065) { BOOST_TEST(fail("test_0065.depc")); }
BOOST_AUTO_TEST_CASE(test_0066) { BOOST_TEST(fail("test_0066.depc")); }
BOOST_AUTO_TEST_CASE(test_0067) { BOOST_TEST(fail("test_0067.depc")); }
BOOST_AUTO_TEST_CASE(test_0068) { BOOST_TEST(fail("test_0068.depc")); }
BOOST_AUTO_TEST_CASE(test_0069) { BOOST_TEST(fail("test_0069.depc")); }
BOOST_AUTO_TEST_CASE(test_0070) { BOOST_TEST(fail("test_0070.depc")); }
BOOST_AUTO_TEST_CASE(test_0071) { BOOST_TEST(fail("test_0071.depc")); }
BOOST_AUTO_TEST_CASE(test_0072) { BOOST_TEST(fail("test_0072.depc")); }
BOOST_AUTO_TEST_CASE(test_0073) { BOOST_TEST(fail("test_0073.depc")); }
BOOST_AUTO_TEST_CASE(test_0074) { BOOST_TEST(fail("test_0074.depc")); }
BOOST_AUTO_TEST_CASE(test_0075) { BOOST_TEST(fail("test_0075.depc")); }
BOOST_AUTO_TEST_CASE(test_0076) { BOOST_TEST(fail("test_0076.depc")); }
BOOST_AUTO_TEST_CASE(test_0077) { BOOST_TEST(fail("test_0077.depc")); }
BOOST_AUTO_TEST_CASE(test_0078) { BOOST_TEST(fail("test_0078.depc")); }
BOOST_AUTO_TEST_CASE(test_0079) { BOOST_TEST(fail("test_0079.depc")); }
BOOST_AUTO_TEST_CASE(test_0080) { BOOST_TEST(fail("test_0080.depc")); }
BOOST_AUTO_TEST_CASE(test_0081) { BOOST_TEST(fail("test_0081.depc")); }
BOOST_AUTO_TEST_CASE(test_0082) { BOOST_TEST(fail("test_0082.depc")); }
BOOST_AUTO_TEST_CASE(test_0083) { BOOST_TEST(fail("test_0083.depc")); }
BOOST_AUTO_TEST_CASE(test_0084) { BOOST_TEST(fail("test_0084.depc")); }
BOOST_AUTO_TEST_CASE(test_0085) { BOOST_TEST(fail("test_0085.depc")); }
BOOST_AUTO_TEST_CASE(test_0086) { BOOST_TEST(fail("test_0086.depc")); }
BOOST_AUTO_TEST_CASE(test_0087) { BOOST_TEST(fail("test_0087.depc")); }
BOOST_AUTO_TEST_CASE(test_0088) { BOOST_TEST(fail("test_0088.depc")); }
BOOST_AUTO_TEST_CASE(test_0089) { BOOST_TEST(fail("test_0089.depc")); }
BOOST_AUTO_TEST_CASE(test_0090) { BOOST_TEST(fail("test_0090.depc")); }
BOOST_AUTO_TEST_CASE(test_0091) { BOOST_TEST(fail("test_0091.depc")); }
BOOST_AUTO_TEST_CASE(test_0092) { BOOST_TEST(fail("test_0092.depc")); }
BOOST_AUTO_TEST_CASE(test_0093) { BOOST_TEST(fail("test_0093.depc")); }
BOOST_AUTO_TEST_CASE(test_0094) { BOOST_TEST(fail("test_0094.depc")); }
BOOST_AUTO_TEST_CASE(test_0095) { BOOST_TEST(fail("test_0095.depc")); }
BOOST_AUTO_TEST_CASE(test_0096) { BOOST_TEST(fail("test_0096.depc")); }
BOOST_AUTO_TEST_CASE(test_0097) { BOOST_TEST(fail("test_0097.depc")); }
BOOST_AUTO_TEST_CASE(test_0098) { BOOST_TEST(fail("test_0098.depc")); }
BOOST_AUTO_TEST_CASE(test_0099) { BOOST_TEST(fail("test_0099.depc")); }
BOOST_AUTO_TEST_CASE(test_0100) { BOOST_TEST(fail("test_0100.depc")); }
BOOST_AUTO_TEST_CASE(test_0101) { BOOST_TEST(fail("test_0101.depc")); }
BOOST_AUTO_TEST_CASE(test_0102) { BOOST_TEST(fail("test_0102.depc")); }
BOOST_AUTO_TEST_CASE(test_0103) { BOOST_TEST(fail("test_0103.depc")); }
BOOST_AUTO_TEST_CASE(test_0104) { BOOST_TEST(fail("test_0104.depc")); }
BOOST_AUTO_TEST_CASE(test_0105) { BOOST_TEST(fail("test_0105.depc")); }
BOOST_AUTO_TEST_CASE(test_0106) { BOOST_TEST(fail("test_0106.depc")); }
BOOST_AUTO_TEST_CASE(test_0107) { BOOST_TEST(fail("test_0107.depc")); }
BOOST_AUTO_TEST_CASE(test_0108) { BOOST_TEST(fail("test_0108.depc")); }
BOOST_AUTO_TEST_CASE(test_0109) { BOOST_TEST(fail("test_0109.depc")); }
BOOST_AUTO_TEST_CASE(test_0110) { BOOST_TEST(fail("test_0110.depc")); }
BOOST_AUTO_TEST_CASE(test_0111) { BOOST_TEST(fail("test_0111.depc")); }
BOOST_AUTO_TEST_CASE(test_0112) { BOOST_TEST(fail("test_0112.depc")); }
BOOST_AUTO_TEST_CASE(test_0113) { BOOST_TEST(fail("test_0113.depc")); }
BOOST_AUTO_TEST_CASE(test_0114) { BOOST_TEST(fail("test_0114.depc")); }
BOOST_AUTO_TEST_CASE(test_0115) { BOOST_TEST(fail("test_0115.depc")); }
BOOST_AUTO_TEST_CASE(test_0116) { BOOST_TEST(fail("test_0116.depc")); }
BOOST_AUTO_TEST_CASE(test_0117) { BOOST_TEST(fail("test_0117.depc")); }
BOOST_AUTO_TEST_CASE(test_0118) { BOOST_TEST(fail("test_0118.depc")); }
BOOST_AUTO_TEST_CASE(test_0119) { BOOST_TEST(fail("test_0119.depc")); }
BOOST_AUTO_TEST_CASE(test_0120) { BOOST_TEST(fail("test_0120.depc")); }
BOOST_AUTO_TEST_CASE(test_0121) { BOOST_TEST(fail("test_0121.depc")); }
BOOST_AUTO_TEST_CASE(test_0122) { BOOST_TEST(fail("test_0122.depc")); }
BOOST_AUTO_TEST_CASE(test_0123) { BOOST_TEST(fail("test_0123.depc")); }
BOOST_AUTO_TEST_CASE(test_0124) { BOOST_TEST(fail("test_0124.depc")); }
BOOST_AUTO_TEST_CASE(test_0125) { BOOST_TEST(fail("test_0125.depc")); }
BOOST_AUTO_TEST_CASE(test_0126) { BOOST_TEST(fail("test_0126.depc")); }
BOOST_AUTO_TEST_CASE(test_0127) { BOOST_TEST(fail("test_0127.depc")); }
BOOST_AUTO_TEST_CASE(test_0128) { BOOST_TEST(fail("test_0128.depc")); }
BOOST_AUTO_TEST_CASE(test_0129) { BOOST_TEST(fail("test_0129.depc")); }
BOOST_AUTO_TEST_CASE(test_0130) { BOOST_TEST(fail("test_0130.depc")); }
BOOST_AUTO_TEST_CASE(test_0131) { BOOST_TEST(fail("test_0131.depc")); }
BOOST_AUTO_TEST_CASE(test_0132) { BOOST_TEST(fail("test_0132.depc")); }
BOOST_AUTO_TEST_CASE(test_0133) { BOOST_TEST(fail("test_0133.depc")); }
BOOST_AUTO_TEST_CASE(test_0134) { BOOST_TEST(fail("test_0134.depc")); }
BOOST_AUTO_TEST_CASE(test_0135) { BOOST_TEST(fail("test_0135.depc")); }
BOOST_AUTO_TEST_CASE(test_0136) { BOOST_TEST(fail("test_0136.depc")); }
BOOST_AUTO_TEST_CASE(test_0137) { BOOST_TEST(fail("test_0137.depc")); }
BOOST_AUTO_TEST_CASE(test_0138) { BOOST_TEST(fail("test_0138.depc")); }
BOOST_AUTO_TEST_CASE(test_0139) { BOOST_TEST(fail("test_0139.depc")); }
BOOST_AUTO_TEST_CASE(test_0140) { BOOST_TEST(fail("test_0140.depc")); }
BOOST_AUTO_TEST_CASE(test_0141) { BOOST_TEST(fail("test_0141.depc")); }
BOOST_AUTO_TEST_CASE(test_0142) { BOOST_TEST(fail("test_0142.depc")); }
BOOST_AUTO_TEST_CASE(test_0143) { BOOST_TEST(fail("test_0143.depc")); }
BOOST_AUTO_TEST_CASE(test_0144) { BOOST_TEST(fail("test_0144.depc")); }
BOOST_AUTO_TEST_CASE(test_0145) { BOOST_TEST(fail("test_0145.depc")); }
BOOST_AUTO_TEST_CASE(test_0146) { BOOST_TEST(fail("test_0146.depc")); }
BOOST_AUTO_TEST_CASE(test_0147) { BOOST_TEST(fail("test_0147.depc")); }
BOOST_AUTO_TEST_CASE(test_0148) { BOOST_TEST(fail("test_0148.depc")); }
BOOST_AUTO_TEST_CASE(test_0149) { BOOST_TEST(fail("test_0149.depc")); }
BOOST_AUTO_TEST_CASE(test_0150) { BOOST_TEST(fail("test_0150.depc")); }
// BOOST_AUTO_TEST_CASE(test_0151) doesn't parse
BOOST_AUTO_TEST_CASE(test_0152)
{
    BOOST_TEST_REQUIRE(pass("test_0152.depc"));
    BOOST_TEST(pass_result->type_defs.size() == 10ul);
    BOOST_TEST(pass_result->func_defs.size() == 16ul);
}
BOOST_AUTO_TEST_CASE(test_0153) { BOOST_TEST(fail("test_0153.depc")); }
// BOOST_AUTO_TEST_CASE(test_0154) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0155) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0156) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0157) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0158) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0159) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0160) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0161) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0162) doesn't parse
BOOST_AUTO_TEST_CASE(test_0163) { BOOST_TEST(pass("test_0163.depc")); }
BOOST_AUTO_TEST_CASE(test_0164) { BOOST_TEST(pass("test_0164.depc")); }
// BOOST_AUTO_TEST_CASE(test_0165) doesn't parse
// BOOST_AUTO_TEST_CASE(test_0166) doesn't parse
BOOST_AUTO_TEST_CASE(test_0167)
{
    BOOST_TEST_REQUIRE(pass("test_0167.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST_REQUIRE(f.args.size() == 1ul);
    auto const& arg = f.args[0];
    BOOST_TEST(arg.name == "x");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arg.type.value));
    BOOST_TEST_REQUIRE(f.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* var = std::get_if<dep0::typecheck::expr_t::var_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(var);
    BOOST_TEST(var->name == "x");
}
BOOST_AUTO_TEST_CASE(test_0168)
{
    BOOST_TEST_REQUIRE(pass("test_0168.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST_REQUIRE(f.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* call = std::get_if<dep0::typecheck::expr_t::fun_call_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(call->name == "id");
    BOOST_TEST_REQUIRE(call->args.size() == 1ul);
    auto const *expr = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[0].value);
    BOOST_TEST_REQUIRE(expr);
    BOOST_TEST(expr->number == "0");
}
BOOST_AUTO_TEST_CASE(test_0169)
{
    BOOST_TEST_REQUIRE(pass("test_0169.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST_REQUIRE(f.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* call = std::get_if<dep0::typecheck::expr_t::fun_call_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(call->name == "first");
    BOOST_TEST_REQUIRE(call->args.size() == 2ul);
    auto const *expr0 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[0].value);
    auto const *expr1 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[1].value);
    BOOST_TEST_REQUIRE(expr0);
    BOOST_TEST_REQUIRE(expr1);
    BOOST_TEST(expr0->number == "0");
    BOOST_TEST(expr1->number == "1");
}

BOOST_AUTO_TEST_SUITE_END()
