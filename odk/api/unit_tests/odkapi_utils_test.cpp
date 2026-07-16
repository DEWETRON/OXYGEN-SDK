// Copyright DEWETRON GmbH 2018

#include "odkapi_utils.h"
#include "odkapi_error_codes.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

#include <functional>
#include <numeric>

using namespace odk;

namespace
{
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

    class TestHost : public odk::IfHost
    {
    public:
        std::function<uint64_t(MessageId msg_id, std::uint64_t key, const IfValue* param, const IfValue** ret)> fn_messageSync;
        std::function<uint64_t(MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret)> fn_messageSyncData;

        IfValue* PLUGIN_API createValue(IfValue::Type type) const final
        {
            return nullptr;
        }

        std::uint64_t PLUGIN_API messageSync(MessageId msg_id, std::uint64_t key, const IfValue* param, const IfValue** ret = nullptr) final
        {
            if (fn_messageSync)
            {
                return fn_messageSync(msg_id, key, param, ret);
            }
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        std::uint64_t PLUGIN_API messageSyncData(MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret = nullptr) final
        {
            if (fn_messageSyncData)
            {
                return fn_messageSyncData(msg_id, key, param, param_size, ret);
            }
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        std::uint64_t PLUGIN_API messageAsync(MessageId msg_id, std::uint64_t key, const IfValue* param) final
        {
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        const IfValue* PLUGIN_API query(const char* context, const char* item, const IfValue* param) final
        {
            return nullptr;
        }

        const IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) final
        {
            return nullptr;
        }
    };
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

BOOST_AUTO_TEST_CASE(add_sample_single_test)
{
    TestHost host;
    uint64_t timestamp = 0x123;
    double value = 3.1415;

    host.fn_messageSyncData = [=](MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg::ADD_SAMPLE);
            BOOST_CHECK_EQUAL(key, 55);
            BOOST_REQUIRE(param);
            BOOST_REQUIRE_EQUAL(param_size, sizeof(uint64_t) + sizeof(double));
            const uint64_t* data_timestamp = reinterpret_cast<const uint64_t*>(param);
            const double* data_value = reinterpret_cast<const double*>(data_timestamp + 1);
            BOOST_CHECK_EQUAL(*data_timestamp, timestamp);
            BOOST_CHECK_EQUAL(*data_value, value);
            return odk::error_codes::OK;
        };
    BOOST_CHECK_EQUAL(addSample(&host, 55, timestamp, &value, sizeof(value)), odk::error_codes::OK);
    BOOST_CHECK_EQUAL(addSample(&host, 55, timestamp, value), odk::error_codes::OK);
}

BOOST_AUTO_TEST_CASE(add_sample_vector_test)
{
    TestHost host;
    uint64_t timestamp = 0x123;
    std::vector<double> data(512);
    std::iota(data.begin(), data.end(), 1.0);

    host.fn_messageSyncData = [=](MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg::ADD_SAMPLE);
            BOOST_CHECK_EQUAL(key, 55);
            BOOST_REQUIRE(param);
            BOOST_REQUIRE_EQUAL(param_size, sizeof(uint64_t) + sizeof(double) * data.size());
            const uint64_t* data_timestamp = reinterpret_cast<const uint64_t*>(param);
            const double* data_value = reinterpret_cast<const double*>(data_timestamp + 1);
            BOOST_CHECK_EQUAL(*data_timestamp, timestamp);
            BOOST_CHECK_EQUAL_COLLECTIONS(data_value, data_value + data.size(), data.begin(), data.end());
            return odk::error_codes::OK;
        };
    BOOST_CHECK_EQUAL(addSample(&host, 55, timestamp, data.data(), sizeof(double) * data.size()), odk::error_codes::OK);
}

BOOST_AUTO_TEST_CASE(add_samples_test)
{
    TestHost host;
    uint64_t timestamp = 0x123;
    double value[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    host.fn_messageSyncData = [=](MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg::ADD_CONTIGUOUS_SAMPLES);
            BOOST_CHECK_EQUAL(key, 55);
            BOOST_REQUIRE(param);
            BOOST_REQUIRE_EQUAL(param_size, sizeof(uint64_t) + sizeof(value));
            const uint64_t* data_timestamp = reinterpret_cast<const uint64_t*>(param);
            const double* data_value = reinterpret_cast<const double*>(data_timestamp + 1);
            BOOST_CHECK_EQUAL(*data_timestamp, timestamp);
            BOOST_CHECK_EQUAL_COLLECTIONS(data_value, data_value + 8, value, value + 8);
            return odk::error_codes::OK;
        };
    BOOST_CHECK_EQUAL(addSamples(&host, 55, timestamp, value, sizeof(value)), odk::error_codes::OK);

    uint64_t timestamp2 = 0x01020304;
    std::vector<int> data(512);
    std::iota(data.begin(), data.end(), 1);

    host.fn_messageSyncData = [=](MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg::ADD_CONTIGUOUS_SAMPLES);
            BOOST_CHECK_EQUAL(key, 1024);
            BOOST_REQUIRE(param);
            BOOST_REQUIRE_EQUAL(param_size, sizeof(uint64_t) + sizeof(int) * data.size());
            const uint64_t* data_timestamp = reinterpret_cast<const uint64_t*>(param);
            const int* data_value = reinterpret_cast<const int*>(data_timestamp + 1);
            BOOST_CHECK_EQUAL(*data_timestamp, timestamp2);
            BOOST_CHECK_EQUAL_COLLECTIONS(data_value, data_value + data.size(), data.begin(), data.end());
            return odk::error_codes::OK;
        };
    BOOST_CHECK_EQUAL(addSamples(&host, 1024, timestamp2, data.data(), sizeof(int) * data.size()), odk::error_codes::OK);
}

BOOST_AUTO_TEST_SUITE_END()
