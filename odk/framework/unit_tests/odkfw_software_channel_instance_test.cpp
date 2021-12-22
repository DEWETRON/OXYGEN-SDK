// Copyright DEWETRON GmbH 2021
#include "odkfw_software_channel_instance.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_acquisition_task_xml.h"
#include "odkapi_software_channel_xml.h"
#include "odkapi_update_channels_xml.h"
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
            case odk::host_msg::SOFTWARE_CHANNEL_REGISTER:
            {
                const odk::IfXMLValue* xml_param = dynamic_cast<const odk::IfXMLValue*>(param);
                BOOST_REQUIRE(xml_param);
                odk::RegisterSoftwareChannel message;
                BOOST_REQUIRE(message.parse(xml_param->getValue()));
                BOOST_CHECK_EQUAL(message.m_display_name, "Test Channel");
                BOOST_CHECK_EQUAL(message.m_service_name, "TestServiceName");
                return odk::error_codes::OK;
            }

            case odk::host_msg::SOFTWARE_CHANNEL_UNREGISTER:
            {
                const odk::IfStringValue* string_param = dynamic_cast<const odk::IfStringValue*>(param);
                BOOST_REQUIRE(string_param);
                BOOST_CHECK_EQUAL(string_param->getValue(), "TestServiceName");
                return odk::error_codes::OK;
            }

            case odk::host_msg::ACQUISITION_TASK_ADD:
            {                
                const odk::IfXMLValue* xml_param = dynamic_cast<const odk::IfXMLValue*>(param);
                BOOST_REQUIRE(xml_param);

                odk::AddAcquisitionTaskTelegram telegram;
                BOOST_REQUIRE(telegram.parse(xml_param->getValue()));
                return odk::error_codes::OK;
            }

            case odk::host_msg::ACQUISITION_TASK_REMOVE:
            {
                BOOST_REQUIRE_EQUAL(param, nullptr);
                return odk::error_codes::OK;
            }

            default:
                BOOST_FAIL("Unexpected message");
                return 0;
            }
        }

        std::uint64_t PLUGIN_API messageSyncData(odk::MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const odk::IfValue** ret) final
        {
            switch (msg_id)
            {
            case odk::host_msg::SET_PLUGIN_OUTPUT_CHANNELS:
            {
                odk::UpdateChannelsTelegram telegram;
                BOOST_REQUIRE(telegram.parse(static_cast<const char*>(param)));
                return odk::error_codes::OK;
            }
            case odk::host_msg::SET_PLUGIN_CONFIGURATION:
            {
                odk::UpdateConfigTelegram telegram;
                BOOST_REQUIRE(telegram.parse(static_cast<const char*>(param)));
                return odk::error_codes::OK;
            }
            default:
                return TestHost::messageSyncData(msg_id, key, param, param_size, ret);
            }
        }
    };

    class TestInstance : public odk::framework::SoftwareChannelInstance
    {
        friend class Fixture;
    public:
        TestInstance() = default;

        static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
        {
            odk::RegisterSoftwareChannel info;
            info.m_display_name = "Test Channel";
            info.m_service_name = "TestServiceName";
            return info;
        }

        bool configure(const odk::UpdateChannelsTelegram& request,
            std::map<uint32_t, uint32_t>& channel_id_map) final
        {
            ODK_UNUSED(request);
            ODK_UNUSED(channel_id_map);
            return true;
        }

        bool update() final
        {
            return true;
        }

        void process(ProcessingContext& context, odk::IfHost* host) final
        {
            ODK_UNUSED(context);            
            ODK_UNUSED(host);
        }
    };

    class Fixture : public odk::framework::SoftwareChannelPlugin<TestInstance>
    {
    public:
        Fixture()
        {
            setPluginHost(&m_fixture_host);
            std::string error;
            init(error);
        }

        ~Fixture()
        {
            deinit();
        }

        FixtureHost m_fixture_host;
    };
}

BOOST_FIXTURE_TEST_SUITE(software_channel_instance_test_suite, Fixture)

BOOST_AUTO_TEST_CASE(CreateChannel)
{
    odk::CreateSoftwareChannel csc;
    csc.m_service_name = "TestServiceName";

    odk::InputChannelData ch1;
    ch1.channel_id = 1;
    ch1.data_format.m_sample_dimension = 1;
    ch1.data_format.m_sample_format = odk::ChannelDataformat::SampleFormat::DOUBLE;
    ch1.data_format.m_sample_value_type = odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR;
    ch1.data_format.m_sample_occurrence = odk::ChannelDataformat::SampleOccurrence::SYNC;
    ch1.data_format.m_sample_reduced_format = odk::ChannelDataformat::SampleReducedFormat::UNKNOWN;
    csc.m_all_selected_channels_data.push_back(ch1);

    auto start_xml = static_cast<odk::IfXMLValue*>(m_fixture_host.createValue(odk::IfXMLValue::type_index));
    start_xml->set(csc.generate().c_str());
    const odk::IfValue* result = nullptr;
    auto ret = static_cast<IfPlugin*>(this)->pluginMessage(odk::plugin_msg::SOFTWARE_CHANNEL_CREATE, 123, start_xml, &result);
    BOOST_CHECK_EQUAL(odk::error_codes::OK, ret);
    start_xml->release();
    start_xml = nullptr;
}

BOOST_AUTO_TEST_SUITE_END()
