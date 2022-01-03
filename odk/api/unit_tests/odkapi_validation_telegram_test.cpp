// Copyright DEWETRON GmbH 2017

#include "odkapi_export_xml.h"

#include "odkuni_xpugixml.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(plugin_validate_export_telegram_test)

BOOST_AUTO_TEST_CASE(ParseXML)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <ValidateExportSettings>
                <ExportProperties>
                    <Filename name="test.ext"/>
                    <Channels>
                        <Channel channel_id="10056"/>
                        <Channel channel_id="54992"/>
                    </Channels>
                    <Intervals>
                        <Interval begin="0.2" end="1.1"/>
                        <Interval begin="4.0" end="1200.4"/>
                    </Intervals>
                    <CustomProperties>
                        <Property>
                        </Property>
                    </CustomProperties>
                    <FormatId format="MDF41"/>
                    <Modes>
                        <Mode mode="NORMAL"/>
                        <Mode mode="REDUCED"/>
                    </Modes>
                </ExportProperties>
            </ValidateExportSettings>
            )xxx"
            ;

    odk::ValidateExport telegram;
    BOOST_CHECK(telegram.parse(xml_content));

    BOOST_REQUIRE_EQUAL(telegram.m_properties.m_channels.size(), 2);
    {
        BOOST_CHECK_EQUAL(telegram.m_properties.m_channels.at(0), 10056);
        BOOST_CHECK_EQUAL(telegram.m_properties.m_channels.at(1), 54992);
    }

    BOOST_REQUIRE_EQUAL(telegram.m_properties.m_export_intervals.size(), 2);
    {
        BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(0).m_begin, 0.2, 1e-6);
        BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(0).m_end, 1.1, 1e-6);

        BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(1).m_begin, 4.0, 1e-6);
        BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(1).m_end, 1200.4, 1e-6);
    }

    BOOST_CHECK_EQUAL(telegram.m_properties.m_custom_properties.size(), 0);
}

BOOST_AUTO_TEST_CASE(CreateXML)
{
    odk::ExportProperties exp_props;
    exp_props.m_channels.push_back(10056);
    exp_props.m_channels.push_back(54992);

    exp_props.m_export_intervals.emplace_back(0.2, 1.1);
    exp_props.m_export_intervals.emplace_back(4.0, 1200.4);

    exp_props.m_format_id = "MDF41";
    exp_props.m_filename = "testfile.ext";

    auto xml_from_properties = exp_props.generate();

    odk::ValidateExport telegram;
    telegram.m_properties = exp_props;

    const auto generated_xml = telegram.generate();

    {
        BOOST_CHECK(telegram.parse(generated_xml.c_str()));

        BOOST_REQUIRE_EQUAL(telegram.m_properties.m_channels.size(), 2);
        {
            BOOST_CHECK_EQUAL(telegram.m_properties.m_channels.at(0), 10056);
            BOOST_CHECK_EQUAL(telegram.m_properties.m_channels.at(1), 54992);
        }

        BOOST_REQUIRE(telegram.m_properties.m_export_intervals.size() == 2);
        {
            BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(0).m_begin, 0.2, 1e-6);
            BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(0).m_end, 1.1, 1e-6);

            BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(1).m_begin, 4.0, 1e-6);
            BOOST_CHECK_CLOSE(telegram.m_properties.m_export_intervals.at(1).m_end, 1200.4, 1e-6);
        }

        BOOST_CHECK(telegram.m_properties.m_custom_properties.size() == 0);
    }
}

BOOST_AUTO_TEST_CASE(ChannelErrorTest)
{
    odk::ChannelError e1(1, 2);
    BOOST_CHECK_EQUAL(e1.channel_id, 1);
    BOOST_CHECK_EQUAL(e1.error_code, 2);
    BOOST_CHECK(e1.error_message.empty());

    odk::ChannelError e2(123, 456, "message");
    BOOST_CHECK_EQUAL(e2.channel_id, 123);
    BOOST_CHECK_EQUAL(e2.error_code, 456);
    BOOST_CHECK_EQUAL(e2.error_message, "message");
}

BOOST_AUTO_TEST_CASE(ParseResponseSuccess)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <ValidationSuccess/>
            )xxx"
            ;

    odk::ValidateExportResponse response;
    BOOST_CHECK(response.parse(xml_content));

    BOOST_CHECK(response.m_success);
    BOOST_CHECK(response.m_channel_errors.empty());
}

BOOST_AUTO_TEST_CASE(ParseResponseFailure)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <ValidationFailed>
                <Channels>
                    <Channel>
                        <Id>123</Id>
                        <ErrorCode>456</ErrorCode>
                        <ErrorMessage>message</ErrorMessage>
                    </Channel>
                </Channels>
            </ValidationFailed>
            )xxx"
            ;

    odk::ValidateExportResponse response;
    BOOST_CHECK(response.parse(xml_content));

    BOOST_CHECK(!response.m_success);
    BOOST_REQUIRE_EQUAL(response.m_channel_errors.size(), 1);
    BOOST_CHECK_EQUAL(response.m_channel_errors.front().channel_id, 123);
    BOOST_CHECK_EQUAL(response.m_channel_errors.front().error_code, 456);
    BOOST_CHECK_EQUAL(response.m_channel_errors.front().error_message, "message");
}

BOOST_AUTO_TEST_SUITE_END()
