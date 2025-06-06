// Copyright DEWETRON GmbH 2018

#include "odkapi_utils.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

using namespace odk;

//old function used for counter test
ODK_NODISCARD inline double convertTickToTimeOld(std::uint64_t tick, double frequency)
{
    return std::nextafter(tick / frequency, std::numeric_limits<double>::max());
}

std::uint64_t convertTickToTimeToTick(std::uint64_t tick, double frequency)
{
    //auto tm = convertTickToTimeOld(tick, frequency);
    auto tm = odk::convertTickToTime(tick, frequency);
    auto converted = odk::convertTimeToTickAtOrAfter(tm, frequency);
    return converted;
}

BOOST_AUTO_TEST_SUITE(utils)

BOOST_AUTO_TEST_CASE(convert_fixed_ticks_test)
{
    std::vector<std::uint64_t> ticks{ 0, 7, 9999999999, 10000000000, 10000000001 };
    double frequency = 0.01;

    for (auto tick : ticks)
    {
        auto converted = convertTickToTimeToTick(tick, frequency);
        BOOST_REQUIRE_MESSAGE(tick == converted,
            "Roundtrip error converting tick to time to tick: "
            << tick << "!=" << converted << ", freq = " << frequency);
    }
}

BOOST_AUTO_TEST_CASE(convert_ticks_test)
{
    std::vector<double> frequencies{ 0.001, 0.01, 0.1, 1, 10, 20, 100, 1000, 10000, 20000, 100000, 1000000 };
    for (auto frequency : frequencies)
    {
        for (std::uint64_t tick = 0; tick < 10001; ++tick)
        {
            auto converted = convertTickToTimeToTick(tick, frequency);
            BOOST_REQUIRE_MESSAGE(tick == converted,
                "Roundtrip error converting tick to time to tick: "
                << tick << "!=" << converted << ", freq = " << frequency);
        }

        for (std::uint64_t tick = 9999999800; tick < 10000001001; ++tick)
        {
            auto converted = convertTickToTimeToTick(tick, frequency);
            BOOST_REQUIRE_MESSAGE(tick == converted,
                "Roundtrip error converting tick to time to tick: "
                << tick << "!=" << converted << ", freq = " << frequency);
        }
    }
}

BOOST_AUTO_TEST_CASE(convert_timestamp_to_tick_test)
{
    BOOST_CHECK_EQUAL(1000, convertTimestampToTick(odk::Timestamp(1000, 100.0), 100.0));
    BOOST_CHECK_EQUAL(2000, convertTimestampToTick(odk::Timestamp(1000, 100.0), 200.0));
    BOOST_CHECK_EQUAL(500, convertTimestampToTick(odk::Timestamp(1000, 100.0), 50.0));

    BOOST_CHECK_EQUAL(1234 * 2, convertTimestampToTick(odk::Timestamp(1234, 100.0), 200.0));
    BOOST_CHECK_EQUAL(1233 / 2, convertTimestampToTick(odk::Timestamp(1233, 100.0), 50.0));
}

BOOST_AUTO_TEST_SUITE_END()
