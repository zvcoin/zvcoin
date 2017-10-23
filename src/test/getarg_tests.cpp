#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ZVC");
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", false));
    BOOST_CHECK(GetBoolArg("-ZVC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ZVCo"));
    BOOST_CHECK(!GetBoolArg("-ZVCo", false));
    BOOST_CHECK(GetBoolArg("-ZVCo", true));

    ResetArgs("-ZVC=0");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));

    ResetArgs("-ZVC=1");
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", false));
    BOOST_CHECK(GetBoolArg("-ZVC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noZVC");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));

    ResetArgs("-noZVC=1");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));

    ResetArgs("-ZVC -noZVC");  // -ZVC should win
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", false));
    BOOST_CHECK(GetBoolArg("-ZVC", true));

    ResetArgs("-ZVC=1 -noZVC=1");  // -ZVC should win
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", false));
    BOOST_CHECK(GetBoolArg("-ZVC", true));

    ResetArgs("-ZVC=0 -noZVC=0");  // -ZVC should win
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ZVC=1");
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", false));
    BOOST_CHECK(GetBoolArg("-ZVC", true));

    ResetArgs("--noZVC=1");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", "eleven"), "eleven");

    ResetArgs("-ZVC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", "eleven"), "");

    ResetArgs("-ZVC=");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", "eleven"), "");

    ResetArgs("-ZVC=11");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", "eleven"), "11");

    ResetArgs("-ZVC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ZVC", 0), 0);

    ResetArgs("-ZVC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ZVC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ZVC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ZVC");
    BOOST_CHECK_EQUAL(GetBoolArg("-ZVC"), true);

    ResetArgs("--ZVC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ZVC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noZVC");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));

    ResetArgs("-noZVC=1");
    BOOST_CHECK(!GetBoolArg("-ZVC"));
    BOOST_CHECK(!GetBoolArg("-ZVC", true));
    BOOST_CHECK(!GetBoolArg("-ZVC", false));

    ResetArgs("-noZVC=0");
    BOOST_CHECK(GetBoolArg("-ZVC"));
    BOOST_CHECK(GetBoolArg("-ZVC", true));
    BOOST_CHECK(GetBoolArg("-ZVC", false));

    ResetArgs("-ZVC --noZVC");
    BOOST_CHECK(GetBoolArg("-ZVC"));

    ResetArgs("-noZVC -ZVC"); // ZVC always wins:
    BOOST_CHECK(GetBoolArg("-ZVC"));
}

BOOST_AUTO_TEST_SUITE_END()
