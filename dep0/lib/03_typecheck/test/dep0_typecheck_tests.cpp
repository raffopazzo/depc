#define BOOST_TEST_MODULE dep0_typecheck_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"

#include <boost/type_index.hpp>

#include <filesystem>
#include <cstdlib>

template <typename... Ts>
auto pretty_name(std::variant<Ts...> const& x)
{
    return std::visit([] <typename T> (T const&) { return boost::typeindex::type_id<T>().pretty_name(); }, x);
}

boost::test_tools::predicate_result is_type_var(dep0::typecheck::type_t const& type, std::string_view const name)
{
    auto failed = boost::test_tools::predicate_result(false);
    auto const var = std::get_if<dep0::typecheck::type_t::var_t>(&type.value);
    if (not var)
        failed.message() << "type is not var_t but " << pretty_name(type.value);
    else if (var->name != name)
        failed.message() << var->name << " != " << name;
    else
        return true;
    return failed;
}

boost::test_tools::predicate_result is_var(dep0::typecheck::expr_t const& expr, std::string_view const name)
{
    auto failed = boost::test_tools::predicate_result(false);
    auto const var = std::get_if<dep0::typecheck::expr_t::var_t>(&expr.value);
    if (not var)
        failed.message().stream() << "expression is not var_t but " << pretty_name(expr.value);
    else if (var->name != name)
        failed.message().stream() << var->name << " != " << name;
    else
        return true;
    return failed;
}

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
    BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    auto const& arg = f.value.args[0];
    BOOST_TEST(arg.name == "x");
    auto const arg_type = std::get_if<dep0::typecheck::type_t>(&arg.sort);
    BOOST_TEST_REQUIRE(arg_type);
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arg_type->value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
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
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* call = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(is_var(call->func.get(), "id"));
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
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* call = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(is_var(call->func.get(), "first"));
    BOOST_TEST_REQUIRE(call->args.size() == 2ul);
    auto const *expr0 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[0].value);
    auto const *expr1 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[1].value);
    BOOST_TEST_REQUIRE(expr0);
    BOOST_TEST_REQUIRE(expr1);
    BOOST_TEST(expr0->number == "0");
    BOOST_TEST(expr1->number == "1");
}
BOOST_AUTO_TEST_CASE(test_0170) { BOOST_TEST(fail("test_0170.depc")); }
BOOST_AUTO_TEST_CASE(test_0171) { BOOST_TEST(fail("test_0171.depc")); }
BOOST_AUTO_TEST_CASE(test_0172)
{
    BOOST_TEST_REQUIRE(pass("test_0172.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);
    auto const& f = pass_result->func_defs[1];
    BOOST_TEST(f.name == "main");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 2ul);
    auto const* call = std::get_if<dep0::typecheck::expr_t::app_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(is_var(call->func.get(), "first"));
    BOOST_TEST_REQUIRE(call->args.size() == 2ul);
    auto const *expr0 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[0].value);
    auto const *expr1 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[1].value);
    BOOST_TEST_REQUIRE(expr0);
    BOOST_TEST_REQUIRE(expr1);
    BOOST_TEST(expr0->number == "0");
    BOOST_TEST(expr1->number == "1");
}

BOOST_AUTO_TEST_CASE(test_0173)
{
    BOOST_TEST_REQUIRE(pass("test_0173.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* expr = std::get_if<dep0::typecheck::expr_t::arith_expr_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(expr);
    auto const* plus = std::get_if<dep0::typecheck::expr_t::arith_expr_t::plus_t>(&expr->value);
    BOOST_TEST_REQUIRE(plus);
    auto const* lhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->lhs.get().value);
    auto const* rhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->rhs.get().value);
    BOOST_TEST_REQUIRE(lhs);
    BOOST_TEST_REQUIRE(rhs);
    BOOST_TEST(not lhs->sign.has_value());
    BOOST_TEST(not rhs->sign.has_value());
    BOOST_TEST(lhs->number == "1");
    BOOST_TEST(rhs->number == "2");
}

BOOST_AUTO_TEST_CASE(test_0174)
{
    BOOST_TEST_REQUIRE(pass("test_0174.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* expr = std::get_if<dep0::typecheck::expr_t::arith_expr_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(expr);
    auto const* plus = std::get_if<dep0::typecheck::expr_t::arith_expr_t::plus_t>(&expr->value);
    BOOST_TEST_REQUIRE(plus);
    auto const* lhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->lhs.get().value);
    auto const* rhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->rhs.get().value);
    BOOST_TEST_REQUIRE(lhs);
    BOOST_TEST_REQUIRE(rhs);
    BOOST_TEST(not lhs->sign.has_value());
    BOOST_TEST(not rhs->sign.has_value());
    BOOST_TEST(lhs->number == "1");
    BOOST_TEST(rhs->number == "2");
}

BOOST_AUTO_TEST_CASE(test_0175)
{
    BOOST_TEST_REQUIRE(pass("test_0175.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "three");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* expr = std::get_if<dep0::typecheck::expr_t::arith_expr_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(expr);
    auto const* plus = std::get_if<dep0::typecheck::expr_t::arith_expr_t::plus_t>(&expr->value);
    BOOST_TEST_REQUIRE(plus);
    auto const* lhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->lhs.get().value);
    auto const* rhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->rhs.get().value);
    BOOST_TEST_REQUIRE(lhs);
    BOOST_TEST_REQUIRE(rhs);
    BOOST_TEST(not lhs->sign.has_value());
    BOOST_TEST_REQUIRE(rhs->sign.has_value());
    BOOST_TEST(lhs->number == "1");
    BOOST_TEST(rhs->sign.value() == '+');
    BOOST_TEST(rhs->number == "2");
}

BOOST_AUTO_TEST_CASE(test_0176)
{
    BOOST_TEST_REQUIRE(pass("test_0176.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 1ul);
    auto const& f = pass_result->func_defs[0];
    BOOST_TEST(f.name == "minus_one");
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
    auto const* ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0].value);
    BOOST_TEST_REQUIRE(ret);
    BOOST_TEST_REQUIRE(ret->expr.has_value());
    auto const* expr = std::get_if<dep0::typecheck::expr_t::arith_expr_t>(&ret->expr->value);
    BOOST_TEST_REQUIRE(expr);
    auto const* plus = std::get_if<dep0::typecheck::expr_t::arith_expr_t::plus_t>(&expr->value);
    BOOST_TEST_REQUIRE(plus);
    auto const* lhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->lhs.get().value);
    auto const* rhs = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&plus->rhs.get().value);
    BOOST_TEST_REQUIRE(lhs);
    BOOST_TEST_REQUIRE(rhs);
    BOOST_TEST(not lhs->sign.has_value());
    BOOST_TEST_REQUIRE(rhs->sign.has_value());
    BOOST_TEST(lhs->number == "1");
    BOOST_TEST(rhs->sign.value() == '-');
    BOOST_TEST(rhs->number == "2");
}

BOOST_AUTO_TEST_CASE(test_0177) { BOOST_TEST_REQUIRE(fail("test_0177.depc")); }

BOOST_AUTO_TEST_CASE(test_0178)
{
    BOOST_TEST_REQUIRE(pass("test_0178.depc"));
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 24ul);
    {
        auto const& f = pass_result->func_defs[0ul];
        BOOST_TEST(f.name == "id");
        BOOST_TEST_REQUIRE(f.value.args.size() == 2ul);
        BOOST_TEST(f.value.args[0ul].name == "t");
        BOOST_TEST(f.value.args[1ul].name == "x");
        BOOST_TEST(std::holds_alternative<dep0::ast::typename_t>(f.value.args[0ul].sort));
        auto const s1 = std::get_if<dep0::typecheck::type_t>(&f.value.args[1ul].sort);
        BOOST_TEST_REQUIRE(s1);
        auto const t1 = std::get_if<dep0::typecheck::type_t::var_t>(&s1->value);
        BOOST_TEST_REQUIRE(t1);
        BOOST_TEST(t1->name == "t");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::var_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(expr->name == "x");
    }
    {
        auto const& f = pass_result->func_defs[1ul];
        BOOST_TEST(f.name == "f");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "id"));
        BOOST_TEST_REQUIRE(expr->args.size() == 2ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&expr->args[0ul].value);
        auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[1ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST_REQUIRE(arg1);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arg0->value));
        BOOST_TEST(arg1->name == "x");
    }
    {
        auto const& f = pass_result->func_defs[3ul];
        BOOST_TEST(f.name == "h");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "id"));
        BOOST_TEST_REQUIRE(expr->args.size() == 2ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&expr->args[0ul].value);
        auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[1ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST_REQUIRE(arg1);
        auto const typ0 = std::get_if<dep0::typecheck::type_t::var_t>(&arg0->value);
        BOOST_TEST(typ0->name == "int");
        BOOST_TEST(arg1->name == "x");
    }
    {
        auto const& f = pass_result->func_defs[7ul];
        BOOST_TEST(f.name == "apply");
        auto const ret_type = std::get_if<dep0::typecheck::type_t::var_t>(&f.value.ret_type.value);
        BOOST_TEST_REQUIRE(ret_type);
        BOOST_TEST(ret_type->name == "t");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "f"));
        BOOST_TEST_REQUIRE(expr->args.size() == 1ul);
        auto const arg = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[0ul].value);
        BOOST_TEST_REQUIRE(arg);
        BOOST_TEST(arg->name == "x");
    }
    {
        auto const& f = pass_result->func_defs[8ul];
        BOOST_TEST(f.name == "apply_f");
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr_type = std::get_if<dep0::typecheck::type_t>(&ret->expr->properties.sort);
        BOOST_TEST_REQUIRE(expr_type);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(expr_type->value));
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "apply"));
        BOOST_TEST_REQUIRE(expr->args.size() == 3ul);
        auto const type0 = std::get_if<dep0::ast::typename_t>(&expr->args[0ul].properties.sort);
        auto const type1 = std::get_if<dep0::typecheck::type_t>(&expr->args[1ul].properties.sort);
        auto const type2 = std::get_if<dep0::typecheck::type_t>(&expr->args[2ul].properties.sort);
        BOOST_TEST(type0);
        BOOST_TEST_REQUIRE(type1);
        BOOST_TEST_REQUIRE(type2);
        auto const arr1 = std::get_if<dep0::typecheck::type_t::arr_t>(&type1->value);
        BOOST_TEST_REQUIRE(arr1);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arr1->ret_type.get().value));
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(type2->value));
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&expr->args[0ul].value);
        auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[1ul].value);
        auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&expr->args[2ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST_REQUIRE(arg1);
        BOOST_TEST_REQUIRE(arg2);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arg0->value));
        BOOST_TEST(arg1->name == "f");
        BOOST_TEST(arg2->sign.value_or(0) == '-');
        BOOST_TEST(arg2->number == "1");
    }
    {
        auto const& f = pass_result->func_defs[9ul];
        BOOST_TEST(f.name == "apply_g");
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(f.value.ret_type.value));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "apply"));
        BOOST_TEST_REQUIRE(expr->args.size() == 3ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&expr->args[0ul].value);
        auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[1ul].value);
        auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&expr->args[2ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST_REQUIRE(arg1);
        BOOST_TEST_REQUIRE(arg2);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(arg0->value));
        BOOST_TEST(arg1->name == "g");
        BOOST_TEST(arg2->sign.has_value() == false);
        BOOST_TEST(arg2->number == "1");
    }
    {
        auto const& f = pass_result->func_defs[10ul];
        BOOST_TEST(f.name == "apply_h");
        auto const ret_type = std::get_if<dep0::typecheck::type_t::var_t>(&f.value.ret_type.value);
        BOOST_TEST_REQUIRE(ret_type);
        BOOST_TEST(ret_type->name == "int");
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "apply"));
        BOOST_TEST_REQUIRE(expr->args.size() == 3ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&expr->args[0ul].value);
        auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&expr->args[1ul].value);
        auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&expr->args[2ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST_REQUIRE(arg1);
        BOOST_TEST_REQUIRE(arg2);
        auto const typ0 = std::get_if<dep0::typecheck::type_t::var_t>(&arg0->value);
        BOOST_TEST_REQUIRE(typ0);
        BOOST_TEST(typ0->name == "int");
        BOOST_TEST(arg1->name == "h");
        BOOST_TEST(arg2->sign.has_value() == false);
        BOOST_TEST(arg2->number == "1");
    }
    {
        auto const& f = pass_result->func_defs[11ul];
        BOOST_TEST(f.name == "apply_0");
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const expr = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(expr);
        BOOST_TEST(is_var(expr->func.get(), "apply"));
        BOOST_TEST_REQUIRE(expr->args.size() == 1ul);
        auto const arg0 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&expr->args[0ul].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST(arg0->sign.has_value() == false);
        BOOST_TEST(arg0->number == "0");
    }
    {
        auto const& f = pass_result->func_defs[12ul];
        BOOST_TEST(f.name == "discard_v1");
        BOOST_TEST_CHECKPOINT("checking prototype of " << f.name);
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(f.value.args[0ul].name == "t");
        BOOST_TEST(f.value.args[1ul].name == "f");
        BOOST_TEST(f.value.args[2ul].name == "x");
        auto const arg_t_type = std::get_if<dep0::ast::typename_t>(&f.value.args[0ul].sort);
        auto const arg_f_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[1ul].sort);
        auto const arg_x_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[2ul].sort);
        BOOST_TEST_REQUIRE(arg_t_type);
        BOOST_TEST_REQUIRE(arg_f_type);
        BOOST_TEST_REQUIRE(arg_x_type);
        auto const arg_f = std::get_if<dep0::typecheck::type_t::arr_t>(&arg_f_type->value);
        BOOST_TEST_REQUIRE(arg_f);
        {
            BOOST_TEST_REQUIRE(arg_f->arg_types.size() == 2ul);
            auto const arg_0 = std::get_if<dep0::typecheck::type_t::var_t>(&arg_f->arg_types[0ul]);
            auto const arg_1 = std::get_if<dep0::typecheck::type_t>(&arg_f->arg_types[1ul]);
            BOOST_TEST_REQUIRE(arg_0);
            BOOST_TEST_REQUIRE(arg_1);
            BOOST_TEST(arg_0->name == "u");
            BOOST_TEST(is_type_var(*arg_1, "u"));
        }
        BOOST_TEST(is_type_var(*arg_x_type, "t"));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_CHECKPOINT("checking body of " << f.name);
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        BOOST_TEST(is_var(ret->expr.value(), "x"));
    }
    {
        auto const& f = pass_result->func_defs[13ul];
        BOOST_TEST(f.name == "discard_v2");
        BOOST_TEST_CHECKPOINT("checking prototype of " << f.name);
        BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
        BOOST_TEST(f.value.args[0ul].name == "t");
        BOOST_TEST(f.value.args[1ul].name == "f");
        BOOST_TEST(f.value.args[2ul].name == "x");
        auto const arg_t_type = std::get_if<dep0::ast::typename_t>(&f.value.args[0ul].sort);
        auto const arg_f_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[1ul].sort);
        auto const arg_x_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[2ul].sort);
        BOOST_TEST_REQUIRE(arg_t_type);
        BOOST_TEST_REQUIRE(arg_f_type);
        BOOST_TEST_REQUIRE(arg_x_type);
        auto const arg_f = std::get_if<dep0::typecheck::type_t::arr_t>(&arg_f_type->value);
        BOOST_TEST_REQUIRE(arg_f);
        {
            BOOST_TEST_REQUIRE(arg_f->arg_types.size() == 2ul);
            auto const arg_0 = std::get_if<dep0::typecheck::type_t::var_t>(&arg_f->arg_types[0ul]);
            auto const arg_1 = std::get_if<dep0::typecheck::type_t>(&arg_f->arg_types[1ul]);
            BOOST_TEST_REQUIRE(arg_0);
            BOOST_TEST_REQUIRE(arg_1);
            BOOST_TEST(arg_0->name == "t");
            BOOST_TEST(is_type_var(*arg_1, "t"));
        }
        BOOST_TEST(is_type_var(*arg_x_type, "t"));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_CHECKPOINT("checking body of " << f.name);
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        BOOST_TEST(is_var(ret->expr.value(), "x"));
    }
    {
        auto const& f = pass_result->func_defs[14ul];
        BOOST_TEST(f.name == "discard_id_v1");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(f.value.ret_type.value));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const app = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(app);
        BOOST_TEST(is_var(app->func.get(), "discard_v1"));
        BOOST_TEST_REQUIRE(app->args.size() == 3ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&app->args[0].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(arg0->value));
        BOOST_TEST(is_var(app->args[1], "id"));
        auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&app->args[2].value);
        BOOST_TEST_REQUIRE(arg2);
        BOOST_TEST(arg2->sign.has_value() == false);
        BOOST_TEST(arg2->number == "0");
    }
    {
        auto const& f = pass_result->func_defs[15ul];
        BOOST_TEST(f.name == "discard_id_v2");
        BOOST_TEST(f.value.args.size() == 0ul);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(f.value.ret_type.value));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const app = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(app);
        BOOST_TEST(is_var(app->func.get(), "discard_v2"));
        BOOST_TEST_REQUIRE(app->args.size() == 3ul);
        auto const arg0 = std::get_if<dep0::typecheck::type_t>(&app->args[0].value);
        BOOST_TEST_REQUIRE(arg0);
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::u32_t>(arg0->value));
        BOOST_TEST(is_var(app->args[1], "id"));
        auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&app->args[2].value);
        BOOST_TEST_REQUIRE(arg2);
        BOOST_TEST(arg2->sign.has_value() == false);
        BOOST_TEST(arg2->number == "0");
    }
    {
        auto const& f = pass_result->func_defs[17ul];
        BOOST_TEST(f.name == "multi_f");
        BOOST_TEST_CHECKPOINT("checking prototype of " << f.name);
        BOOST_TEST_REQUIRE(f.value.args.size() == 5ul);
        BOOST_TEST(f.value.args[0ul].name == "t");
        BOOST_TEST(f.value.args[1ul].name == "f");
        BOOST_TEST(f.value.args[2ul].name == "x");
        BOOST_TEST(f.value.args[3ul].name == "y");
        BOOST_TEST(f.value.args[4ul].name == "z");
        auto const arg_t_type = std::get_if<dep0::ast::typename_t>(&f.value.args[0ul].sort);
        auto const arg_f_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[1ul].sort);
        auto const arg_x_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[2ul].sort);
        auto const arg_y_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[3ul].sort);
        auto const arg_z_type = std::get_if<dep0::typecheck::type_t>(&f.value.args[4ul].sort);
        BOOST_TEST_REQUIRE(arg_t_type);
        BOOST_TEST_REQUIRE(arg_f_type);
        BOOST_TEST_REQUIRE(arg_x_type);
        BOOST_TEST_REQUIRE(arg_y_type);
        BOOST_TEST_REQUIRE(arg_z_type);
        auto const arg_f = std::get_if<dep0::typecheck::type_t::arr_t>(&arg_f_type->value);
        BOOST_TEST_REQUIRE(arg_f);
        {
            BOOST_TEST_REQUIRE(arg_f->arg_types.size() == 2ul);
            auto const arg_0 = std::get_if<dep0::typecheck::type_t::var_t>(&arg_f->arg_types[0ul]);
            auto const arg_1 = std::get_if<dep0::typecheck::type_t>(&arg_f->arg_types[1ul]);
            BOOST_TEST_REQUIRE(arg_0);
            BOOST_TEST_REQUIRE(arg_1);
            BOOST_TEST(arg_0->name == "u");
            BOOST_TEST(is_type_var(*arg_1, "u"));
        }
        BOOST_TEST(is_type_var(*arg_x_type, "t"));
        BOOST_TEST(is_type_var(*arg_y_type, "t"));
        BOOST_TEST(is_type_var(*arg_z_type, "t"));
        BOOST_TEST(is_type_var(f.value.ret_type, "t"));
        BOOST_TEST_CHECKPOINT("checking body of " << f.name);
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const if_1 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_1);
        auto const f_bool = std::get_if<dep0::typecheck::expr_t::app_t>(&if_1->cond.value);
        BOOST_TEST_REQUIRE(f_bool);
        BOOST_TEST(is_var(f_bool->func.get(), "f"));
        {
            BOOST_TEST_REQUIRE(f_bool->args.size() == 2ul);
            auto const arg0 = std::get_if<dep0::typecheck::type_t>(&f_bool->args[0ul].value);
            auto const arg1 = std::get_if<dep0::typecheck::expr_t::boolean_constant_t>(&f_bool->args[1ul].value);
            BOOST_TEST_REQUIRE(arg0);
            BOOST_TEST_REQUIRE(arg1);
            BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::bool_t>(arg0->value));
            BOOST_TEST(arg1->value == "true");
        }
        BOOST_TEST_REQUIRE(if_1->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_1->false_branch->stmts.size() == 1ul);
        auto const if_2 = std::get_if<dep0::typecheck::stmt_t::if_else_t>(&if_1->false_branch->stmts[0ul].value);
        BOOST_TEST_REQUIRE(if_2);
        auto const int_to_bool = std::get_if<dep0::typecheck::expr_t::app_t>(&if_2->cond.value);
        BOOST_TEST_REQUIRE(int_to_bool);
        BOOST_TEST(is_var(int_to_bool->func.get(), "int_to_bool"));
        BOOST_TEST_REQUIRE(int_to_bool->args.size() == 1ul);
        auto const f_int = std::get_if<dep0::typecheck::expr_t::app_t>(&int_to_bool->args[0].value);
        BOOST_TEST_REQUIRE(f_int);
        BOOST_TEST(is_var(f_int->func.get(), "f"));
        {
            BOOST_TEST_REQUIRE(f_int->args.size() == 2ul);
            auto const arg0 = std::get_if<dep0::typecheck::type_t>(&f_int->args[0ul].value);
            auto const arg1 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&f_int->args[1ul].value);
            BOOST_TEST_REQUIRE(arg0);
            BOOST_TEST_REQUIRE(arg1);
            BOOST_TEST(is_type_var(*arg0, "int"));
            BOOST_TEST(arg1->number == "0");
        }
        BOOST_TEST_REQUIRE(if_2->false_branch.has_value());
        BOOST_TEST_REQUIRE(if_2->false_branch->stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&if_2->false_branch->stmts[0].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        auto const f_t = std::get_if<dep0::typecheck::expr_t::app_t>(&ret->expr->value);
        BOOST_TEST_REQUIRE(f_t);
        BOOST_TEST(is_var(f_t->func.get(), "f"));
        BOOST_TEST_REQUIRE(f_t->args.size() == 2ul);
        {
            auto const arg0 = std::get_if<dep0::typecheck::type_t>(&f_t->args[0ul].value);
            auto const arg1 = std::get_if<dep0::typecheck::expr_t::var_t>(&f_t->args[1ul].value);
            BOOST_TEST_REQUIRE(arg0);
            BOOST_TEST_REQUIRE(arg1);
            BOOST_TEST(is_type_var(*arg0, "t"));
            BOOST_TEST(arg1->name == "z");
        }
    }
    {
        auto const& f = pass_result->func_defs[19ul];
        BOOST_TEST(f.name == "get_id");
        BOOST_TEST(f.value.args.size() == 0ul);
        auto const ret_type = std::get_if<dep0::typecheck::type_t::arr_t>(&f.value.ret_type.value);
        BOOST_TEST_REQUIRE(ret_type);
        BOOST_TEST_REQUIRE(ret_type->arg_types.size() == 2ul);
        auto const ret_arg_0 = std::get_if<dep0::typecheck::type_t::var_t>(&ret_type->arg_types[0]);
        auto const ret_arg_1 = std::get_if<dep0::typecheck::type_t>(&ret_type->arg_types[1]);
        BOOST_TEST_REQUIRE(ret_arg_0);
        BOOST_TEST_REQUIRE(ret_arg_1);
        BOOST_TEST(is_type_var(*ret_arg_1, "t"));
        BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);
        auto const ret = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
        BOOST_TEST_REQUIRE(ret);
        BOOST_TEST_REQUIRE(ret->expr.has_value());
        BOOST_TEST(is_var(*ret->expr, "id"));
    }
    {
        auto const& f = pass_result->func_defs[22ul];
        BOOST_TEST(f.name == "apply_id_v1");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(f.value.args[0ul].name == "x");
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    }
    {
        auto const& f = pass_result->func_defs[23ul];
        BOOST_TEST(f.name == "apply_id_v2");
        BOOST_TEST_REQUIRE(f.value.args.size() == 1ul);
        BOOST_TEST(f.value.args[0ul].name == "x");
        BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(f.value.ret_type.value));
    }
}

BOOST_AUTO_TEST_CASE(test_0179) { BOOST_TEST_REQUIRE(fail("test_0179.depc")); }
BOOST_AUTO_TEST_CASE(test_0180) { BOOST_TEST_REQUIRE(fail("test_0180.depc")); }

BOOST_AUTO_TEST_CASE(test_0181)
{
    BOOST_TEST_REQUIRE(pass("test_0181.depc"));
    BOOST_TEST_REQUIRE(pass_result->type_defs.size() == 1ul);
    BOOST_TEST_REQUIRE(pass_result->func_defs.size() == 2ul);

    auto const t = std::get_if<dep0::typecheck::type_def_t::integer_t>(&pass_result->type_defs[0].value);
    BOOST_TEST_REQUIRE(t);
    BOOST_TEST(t->name == "int");

    auto const& f = pass_result->func_defs[0ul];
    BOOST_TEST(f.name == "f");
    BOOST_TEST_REQUIRE(f.value.args.size() == 3ul);
    BOOST_TEST(f.value.args[0ul].name == "t");
    BOOST_TEST(f.value.args[1ul].name == "int");
    BOOST_TEST(f.value.args[2ul].name == "x");
    auto const f_ret_type = std::get_if<dep0::typecheck::type_t::var_t>(&f.value.ret_type.value);
    BOOST_TEST_REQUIRE(f_ret_type);
    BOOST_TEST(f_ret_type->name == "t");
    BOOST_TEST(std::holds_alternative<dep0::ast::typename_t>(f.value.args[0ul].sort));
    BOOST_TEST(std::holds_alternative<dep0::ast::typename_t>(f.value.args[1ul].sort));
    auto const s2 = std::get_if<dep0::typecheck::type_t>(&f.value.args[2ul].sort);
    BOOST_TEST_REQUIRE(s2);
    auto const t2 = std::get_if<dep0::typecheck::type_t::var_t>(&s2->value);
    BOOST_TEST_REQUIRE(t2);
    BOOST_TEST(t2->name == "t");
    BOOST_TEST_REQUIRE(f.value.body.stmts.size() == 1ul);

    auto const ret_f = std::get_if<dep0::typecheck::stmt_t::return_t>(&f.value.body.stmts[0ul].value);
    BOOST_TEST_REQUIRE(ret_f);
    BOOST_TEST_REQUIRE(ret_f->expr.has_value());
    BOOST_TEST(is_var(*ret_f->expr, "x"));

    auto const& g = pass_result->func_defs[1ul];
    BOOST_TEST(g.value.args.empty());
    BOOST_TEST(is_type_var(g.value.ret_type, "int"));
    BOOST_TEST_REQUIRE(g.value.body.stmts.size() == 1ul);
    auto const ret_g = std::get_if<dep0::typecheck::stmt_t::return_t>(&g.value.body.stmts[0ul].value);
    BOOST_TEST_REQUIRE(ret_g);
    BOOST_TEST_REQUIRE(ret_g->expr.has_value());
    auto const call = std::get_if<dep0::typecheck::expr_t::app_t>(&ret_g->expr->value);
    BOOST_TEST_REQUIRE(call);
    BOOST_TEST(is_var(call->func.get(), "f"));
    BOOST_TEST_REQUIRE(call->args.size() == 3ul);
    auto const arg0 = std::get_if<dep0::typecheck::type_t>(&call->args[0].value);
    auto const arg1 = std::get_if<dep0::typecheck::type_t>(&call->args[1].value);
    auto const arg2 = std::get_if<dep0::typecheck::expr_t::numeric_constant_t>(&call->args[2].value);
    BOOST_TEST_REQUIRE(arg0);
    BOOST_TEST_REQUIRE(arg1);
    BOOST_TEST_REQUIRE(arg2);
    BOOST_TEST(is_type_var(*arg0, "int"));
    BOOST_TEST(std::holds_alternative<dep0::typecheck::type_t::i32_t>(arg1->value));
    BOOST_TEST(not arg2->sign.has_value());
    BOOST_TEST(arg2->number == "0");
}

BOOST_AUTO_TEST_SUITE_END()
