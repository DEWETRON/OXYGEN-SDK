// Copyright DEWETRON GmbH 2022

#include "odkfw_resampler.h"
#include "odkbase_if_host.h"
#include "odkapi_message_ids.h"

#include <boost/test/unit_test.hpp>
#include <numeric>

namespace
{
    class TestHost : public odk::IfHost
    {
    public:
        ODK_NODISCARD odk::IfValue* PLUGIN_API createValue(odk::IfValue::Type type) const override
        {
            return nullptr;
        }

        std::uint64_t PLUGIN_API messageSync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret) override
        {
            return 0;
        }

        std::uint64_t PLUGIN_API messageSyncData(odk::MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const odk::IfValue** ret) override
        {
            if (msg_id == odk::host_msg::ADD_CONTIGUOUS_SAMPLES)
            {
                const std::uint8_t* data = reinterpret_cast<const std::uint8_t*>(param);
                std::uint64_t timestamp = 0;
                if (param_size >= sizeof(std::uint64_t))
                {
                    timestamp = *reinterpret_cast<const std::uint64_t*>(data);
                    data += sizeof(std::uint64_t);
                    param_size -= sizeof(std::uint64_t);
                }

                const double* data_f64 = reinterpret_cast<const double*>(data);
                received_samples.insert(received_samples.end(), data_f64, data_f64 + param_size / sizeof(double));
            }
            else
            {
                BOOST_FAIL("Unexpected message");
            }
            return 0;
        }

        std::uint64_t PLUGIN_API messageAsync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param) override
        {
            return 0;
        }

        const odk::IfValue* PLUGIN_API query(const char* context, const char* item, const odk::IfValue* param) override
        {
            return nullptr;
        }

        const odk::IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) override
        {
            return nullptr;
        }

        std::vector<double> received_samples;
    };
}

BOOST_AUTO_TEST_SUITE(resampler_test_suite)

BOOST_AUTO_TEST_CASE(ResampleSameSampleRate)
{
    TestHost host;

    odk::framework::Resampler resampler(100);
    
    const std::vector<double> samples_a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const std::vector<double> samples_b = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 0);
    
    resampler.addSamples(&host, 0, 0.1, samples_a.data(), samples_a.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 9);

    resampler.addSamples(&host, 0, 0.2, samples_b.data(), samples_b.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 19);

    std::vector<double> expected(19);
    std::iota(expected.begin(), expected.end(), 0);
    
    BOOST_REQUIRE_EQUAL(host.received_samples.size(), expected.size());
    for (std::size_t n = 0; n < expected.size(); ++n)
    {
        BOOST_CHECK_CLOSE(expected[n], host.received_samples[n], 1e-6);
    }
}

BOOST_AUTO_TEST_CASE(ResampleDoubleSampleRate)
{
    TestHost host;

    odk::framework::Resampler resampler(100);

    const std::vector<double> samples_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    const std::vector<double> samples_b = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 0);

    resampler.addSamples(&host, 0, 0.05, samples_a.data(), samples_a.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 4);

    resampler.addSamples(&host, 0, 0.1, samples_b.data(), samples_b.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 9);

    std::vector<double> expected;
    for (int n = 0; n < 9; ++n)
    {
        expected.push_back(2 * n);
    }

    BOOST_REQUIRE_EQUAL(host.received_samples.size(), expected.size());
    for (std::size_t n = 0; n < expected.size(); ++n)
    {
        BOOST_CHECK_CLOSE(expected[n], host.received_samples[n], 1e-6);
    }
}

BOOST_AUTO_TEST_CASE(ResampleHalfSampleRate)
{
    TestHost host;

    odk::framework::Resampler resampler(100);

    const std::vector<double> samples_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    const std::vector<double> samples_b = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 0);

    resampler.addSamples(&host, 0, 0.2, samples_a.data(), samples_a.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 19);

    resampler.addSamples(&host, 0, 0.4, samples_b.data(), samples_b.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 39);

    std::vector<double> expected;
    for (int n = 0; n < 39; ++n)
    {
        expected.push_back(0.5 * n);
    }
    BOOST_REQUIRE_EQUAL(host.received_samples.size(), expected.size());
    for (std::size_t n = 0; n < expected.size(); ++n)
    {
        BOOST_CHECK_CLOSE(expected[n], host.received_samples[n], 1e-6);
    }
}

BOOST_AUTO_TEST_CASE(ResampleCloseSampleRate)
{
    TestHost host;

    double nominal_rate = 1000;
    double real_rate = 1002;
    odk::framework::Resampler resampler(nominal_rate);

    const std::vector<double> samples_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    const std::vector<double> samples_b = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 0);

    resampler.addSamples(&host, 0, 10 / real_rate, samples_a.data(), samples_a.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 8);

    resampler.addSamples(&host, 0, 20 / real_rate, samples_b.data(), samples_b.size());
    BOOST_CHECK_EQUAL(resampler.getSampleCount(), 18);

    std::vector<double> expected;
    for (int n = 0; n < 18; ++n)
    {
        expected.push_back(n * real_rate / nominal_rate);
    }
    BOOST_REQUIRE_EQUAL(host.received_samples.size(), expected.size());
    for (std::size_t n = 0; n < expected.size(); ++n)
    {
        BOOST_CHECK_CLOSE(expected[n], host.received_samples[n], 1e-6);
    }
}

BOOST_AUTO_TEST_SUITE_END()
