#define BOOST_TEST_MODULE dep0_llvmgen_tests
#include <boost/test/included/unit_test.hpp>

#include "dep0/parser/parse.hpp"
#include "dep0/typecheck/check.hpp"
#include "dep0/llvmgen/gen.hpp"

#include <filesystem>
#include <cstdlib>

struct Fixture
{
    std::filesystem::path testfiles = std::getenv("DEP0_TESTFILES_DIR");
    llvm::LLVMContext llvm_ctx;

    boost::test_tools::predicate_result pass(std::filesystem::path const file)
    {
        auto parse_result = dep0::parser::parse(testfiles / file);
        if (parse_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), parse_result.error());
            return res;
        }
        auto check_result = dep0::typecheck::check(dep0::typecheck::tt::context_t(), *parse_result);
        if (check_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), check_result.error());
            return res;
        }
        auto const gen_result = dep0::llvmgen::gen(llvm_ctx, "test.depc", *check_result);
        if (gen_result.has_error())
        {
            auto res = boost::test_tools::predicate_result(false);
            dep0::pretty_print(res.message().stream(), gen_result.error());
            return res;
        }
        return true;
    }
};

BOOST_FIXTURE_TEST_SUITE(dep0_llvmgen_tests, Fixture)

BOOST_AUTO_TEST_CASE(test_0000) { BOOST_TEST(pass("test_0000.depc")); }
BOOST_AUTO_TEST_CASE(test_0001) { BOOST_TEST(pass("test_0001.depc")); }
// BOOST_AUTO_TEST_CASE("test_0002.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0003) { BOOST_TEST(pass("test_0003.depc")); }
// BOOST_AUTO_TEST_CASE("test_0004.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0005) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0006) { BOOST_TEST(pass("test_0006.depc")); }
BOOST_AUTO_TEST_CASE(test_0007) { BOOST_TEST(pass("test_0007.depc")); }
// BOOST_AUTO_TEST_CASE("test_0008.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0009.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0010.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0011) { BOOST_TEST(pass("test_0011.depc")); }
// BOOST_AUTO_TEST_CASE("test_0012.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0013.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0014) { BOOST_TEST(pass("test_0014.depc")); }
BOOST_AUTO_TEST_CASE(test_0015) { BOOST_TEST(pass("test_0015.depc")); }
// BOOST_AUTO_TEST_CASE("test_0016.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0017.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0018) { BOOST_TEST(pass("test_0018.depc")); }
BOOST_AUTO_TEST_CASE(test_0019) { BOOST_TEST(pass("test_0019.depc")); }
BOOST_AUTO_TEST_CASE(test_0020) { BOOST_TEST(pass("test_0020.depc")); }
BOOST_AUTO_TEST_CASE(test_0021) { BOOST_TEST(pass("test_0021.depc")); }
BOOST_AUTO_TEST_CASE(test_0022) { BOOST_TEST(pass("test_0022.depc")); }
// BOOST_AUTO_TEST_CASE("test_0023.depc") doesn't type check
BOOST_AUTO_TEST_CASE(test_0024) { BOOST_TEST(pass("test_0024.depc")); }
// BOOST_AUTO_TEST_CASE("test_0025.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0025.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0026.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0027.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0028.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0029.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0030.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0031.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0032.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0033.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0034.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0035.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0036.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0037.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0038.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0039.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0040.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0041.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0042.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0043.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0044.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0045.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0046.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0047.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0048.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0049.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0050.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0051.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0052.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0053.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0054.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0055.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0056.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0057.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0058.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0059.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0060.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0061.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0062.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0063.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0064.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0065.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0066.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0067.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0068.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0069.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0070.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0071.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0072.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0073.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0074.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0075.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0076.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0077.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0078.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0079.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0080.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0081.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0082.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0083.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0084.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0085.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0086.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0087.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0088.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0089.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0090.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0091.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0092.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0093.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0094.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0095.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0096.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0097.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0098.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0099.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0100.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0101.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0102.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0103.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0104.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0105.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0106.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0107.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0108.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0109.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0110.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0111.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0112.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0113.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0114.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0115.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0116.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0117.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0118.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0119.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0120.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0121.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0122.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0123.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0124.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0125.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0126.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0127.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0128.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0129.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0130.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0131.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0132.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0133.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0134.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0135.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0136.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0137.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0138.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0139.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0140.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0141.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0142.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0143.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0144.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0145.depc") doesn't type check
// BOOST_AUTO_TEST_CASE("test_0146.depc") doesn't type check

BOOST_AUTO_TEST_SUITE_END()
