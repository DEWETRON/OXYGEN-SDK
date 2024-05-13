// Copyright DEWETRON GmbH 2021
#include "odkfw_export_instance.h"
#include "odkfw_export_plugin.h"
#include "odkapi_export_xml.h"
#include "test_host.h"
#include "values.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/unit_test.hpp>

namespace
{
    class FixtureHost : public TestHost
    {
    public:
        FixtureHost() = default;

        std::uint64_t PLUGIN_API messageSync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret) override
        {
            ODK_UNUSED(key);
            if (ret)
            {
                *ret = nullptr;
            }

            switch (msg_id)
            {
            case odk::host_msg::EXPORT_REGISTER:
            {
                const odk::IfXMLValue* xml_param = dynamic_cast<const odk::IfXMLValue*>(param);
                BOOST_REQUIRE(xml_param);
                odk::RegisterExport message;
                BOOST_REQUIRE(message.parse(xml_param->getValue()));
                BOOST_CHECK_EQUAL(message.m_format_id, "TestFormat");
                return odk::error_codes::OK;
            }

            case odk::host_msg::EXPORT_UNREGISTER:
            {
                const odk::IfStringValue* string_param = dynamic_cast<const odk::IfStringValue*>(param);
                BOOST_REQUIRE(string_param);
                BOOST_CHECK_EQUAL(string_param->getValue(), "TestFormat");
                return odk::error_codes::OK;
            }

            default:
                BOOST_FAIL("Unexpected message");
                return 0;
            }
        }

        const odk::IfValue* PLUGIN_API query(const char* context, const char* item, const odk::IfValue* param) override
        {
            if (boost::algorithm::starts_with(context, "#Oxygen#Channels#"))
            {
                int channel_index = std::atoi(context + std::strlen("#Oxygen#Channels#"));
                BOOST_REQUIRE_GE(channel_index, 1);
                BOOST_REQUIRE_LE(channel_index, 2);

                if (boost::algorithm::equals(item, "DataFormat"))
                {
                    odk::ChannelDataformat data_format;
                    data_format.m_sample_dimension = 1;
                    data_format.m_sample_format = odk::ChannelDataformat::SampleFormat::DOUBLE;
                    data_format.m_sample_value_type = odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR;
                    data_format.m_sample_occurrence = odk::ChannelDataformat::SampleOccurrence::SYNC;
                    data_format.m_sample_reduced_format = odk::ChannelDataformat::SampleReducedFormat::UNKNOWN;

                    return new XmlValue(data_format.generate());
                }
                if (boost::algorithm::equals(item, "Timebase"))
                {
                    const odk::Timebase timebase(10000);
                    return new XmlValue(timebase.generate());
                }
            }
            return TestHost::query(context, item, param);
        }
    };

    class TestInstance : public odk::framework::ExportInstance
    {
        friend class Fixture;
    public:
        TestInstance() = default;

        static odk::RegisterExport getExportInfo()
        {
            odk::RegisterExport info;
            info.m_format_id = "TestFormat";
            info.m_file_extension = "bin";
            return info;
        }

        void validate(const ValidationContext& context, odk::ValidateExportResponse& response) const override
        {
            BOOST_CHECK_EQUAL(context.m_properties.m_filename, "filename.bin");
            BOOST_CHECK_EQUAL(context.m_properties.m_format_id, "TestFormat");
            BOOST_REQUIRE_EQUAL(context.m_properties.m_export_intervals.size(), 1);
            BOOST_CHECK_EQUAL(context.m_properties.m_export_intervals.front().m_begin, 0);
            BOOST_CHECK_EQUAL(context.m_properties.m_export_intervals.front().m_end, 1);
            BOOST_CHECK_EQUAL(context.m_properties.m_channels.size(), 2);

            response.m_success = true;
            response.m_channel_warnings.emplace_back(1, odk::error_codes::INVALID_MODE);
        }

        bool exportData(const ProcessingContext&) override
        {
            return false;
        }

        void cancel() override
        {
        }

    };

    class Fixture
    {
    public:
        Fixture()
        {
            static_cast<odk::IfPlugin*>(&plugin)->setPluginHost(&host);

            const odk::IfValue* init_result = nullptr;
            auto ret = static_cast<odk::IfPlugin*>(&plugin)->pluginMessage(odk::plugin_msg::INIT, 0, nullptr, &init_result);
            BOOST_CHECK_EQUAL(ret, odk::error_codes::OK);
            BOOST_CHECK_EQUAL(init_result, nullptr);
        }

        ~Fixture()
        {
            const odk::IfValue* deinit_result = nullptr;
            auto ret = static_cast<odk::IfPlugin*>(&plugin)->pluginMessage(odk::plugin_msg::DEINIT, 0, nullptr, &deinit_result);
            BOOST_CHECK_EQUAL(ret, odk::error_codes::OK);
            BOOST_CHECK_EQUAL(deinit_result, nullptr);
        }

        FixtureHost host;
        odk::framework::ExportPlugin<TestInstance> plugin;
    };
}

BOOST_FIXTURE_TEST_SUITE(export_instance_test_suite, Fixture)

BOOST_AUTO_TEST_CASE(InitialState)
{
    TestInstance instance;
    BOOST_CHECK_EQUAL(instance.getID(), 0);
}

BOOST_AUTO_TEST_CASE(Validate)
{
    odk::ValidateExport validate_telegram;
    validate_telegram.m_properties.m_format_id = "TestFormat";
    validate_telegram.m_properties.m_filename = "filename.bin";
    validate_telegram.m_properties.m_export_intervals.emplace_back(0, 1);
    validate_telegram.m_properties.m_channels.push_back(1);
    validate_telegram.m_properties.m_channels.push_back(2);

    auto start_xml = static_cast<odk::IfXMLValue*>(host.createValue(odk::IfXMLValue::type_index));
    start_xml->set(validate_telegram.generate().c_str());
    const odk::IfValue* validation_result = nullptr;
    auto ret = static_cast<odk::IfPlugin*>(&plugin)->pluginMessage(odk::plugin_msg::EXPORT_VALIDATE_SETTINGS, 123, start_xml, &validation_result);
    BOOST_CHECK_EQUAL(odk::error_codes::OK, ret);
    start_xml->release();
    start_xml = nullptr;

    if (validation_result)
    {
        auto xml_result = dynamic_cast<const odk::IfXMLValue*>(validation_result);
        BOOST_REQUIRE(xml_result);

        odk::ValidateExportResponse response;
        BOOST_REQUIRE(response.parse(xml_result->getValue()));
        validation_result->release();
        validation_result = nullptr;

        BOOST_CHECK(response.m_success);
        BOOST_CHECK(response.m_channel_errors.empty());
        BOOST_REQUIRE_EQUAL(response.m_channel_warnings.size(), 1);
        BOOST_CHECK_EQUAL(response.m_channel_warnings.front().channel_id, 1);
        BOOST_CHECK_EQUAL(response.m_channel_warnings.front().error_code, odk::error_codes::INVALID_MODE);
    }
    else
    {
        BOOST_FAIL("No result");
    }
}

BOOST_AUTO_TEST_SUITE_END()
