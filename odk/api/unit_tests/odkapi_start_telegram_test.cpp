// Copyright DEWETRON GmbH 2017

#include "odkapi_export_xml.h"

#include "xpugixml.h"
#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

const static double FP_COMPARISON_TOLERANCE = 0.0000000000001; //! in percent

BOOST_AUTO_TEST_SUITE(plugin_start_export_telegram_test)

BOOST_AUTO_TEST_CASE(ParseXML)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <StartExport>
                <TransactionId transaction_id="1234567"/>
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
                </ExportProperties>
            </StartExport>
            )xxx"
            ;

    StartExport start_export;
    BOOST_CHECK(start_export.parse(xml_content));
    BOOST_CHECK_EQUAL(start_export.m_transaction_id, 1234567);

    BOOST_REQUIRE(start_export.m_properties.m_channels.size() == 2);
    {
        BOOST_CHECK_EQUAL(start_export.m_properties.m_channels.at(0), 10056);
        BOOST_CHECK_EQUAL(start_export.m_properties.m_channels.at(1), 54992);
    }

    BOOST_REQUIRE(start_export.m_properties.m_export_intervals.size() == 2);
    {
        BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

        BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
    }

    BOOST_CHECK(start_export.m_properties.m_custom_properties.size() == 0);
}

BOOST_AUTO_TEST_CASE(CreateXML)
{
    ExportProperties exp_props;
    exp_props.m_channels.push_back(10056);
    exp_props.m_channels.push_back(54992);

    exp_props.m_export_intervals.push_back(Interval<double>(0.2, 1.1));
    exp_props.m_export_intervals.push_back(Interval<double>(4.0, 1200.4));

    exp_props.m_format_id = "MDF41";
    exp_props.m_filename = "testfile.ext";

    auto xml_from_properties = exp_props.generate();

    StartExport start_export;
    start_export.m_transaction_id = 1234567;
    start_export.m_properties = exp_props;

    const auto generated_xml = start_export.generate();

    {
        BOOST_CHECK(start_export.parse(generated_xml.c_str()));
        BOOST_CHECK_EQUAL(start_export.m_transaction_id, 1234567);

        BOOST_REQUIRE(start_export.m_properties.m_channels.size() == 2);
        {
            BOOST_CHECK_EQUAL(start_export.m_properties.m_channels.at(0), 10056);
            BOOST_CHECK_EQUAL(start_export.m_properties.m_channels.at(1), 54992);
        }

        BOOST_REQUIRE(start_export.m_properties.m_export_intervals.size() == 2);
        {
            BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

            BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(start_export.m_properties.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
        }

        BOOST_CHECK(start_export.m_properties.m_custom_properties.size() == 0);
    }
}


BOOST_AUTO_TEST_SUITE_END()
