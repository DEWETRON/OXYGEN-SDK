// Copyright DEWETRON GmbH 2017

#include "odkapi_types.h"
#include "odkapi_export_xml.h"

#include "odkuni_xpugixml.h"
#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

const static double FP_COMPARISON_TOLERANCE = 0.0000000000001; //! in percent

BOOST_AUTO_TEST_SUITE(plugin_export_properties_test)

BOOST_AUTO_TEST_CASE(ParseXML)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
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
                <SpecificSettings>
                </SpecificSettings>
                <FormatId format="MDF41"/>
            </ExportProperties>
            )xxx"
            ;

    ExportProperties exp_props;
    BOOST_CHECK(exp_props.parse(xml_content));

    BOOST_CHECK_EQUAL(exp_props.m_filename, "test.ext");

    BOOST_REQUIRE(exp_props.m_channels.size() == 2);
    {
        BOOST_CHECK_EQUAL(exp_props.m_channels.at(0), 10056);
        BOOST_CHECK_EQUAL(exp_props.m_channels.at(1), 54992);
    }

    BOOST_REQUIRE(exp_props.m_export_intervals.size() == 2);
    {
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
    }

    BOOST_CHECK_EQUAL(exp_props.m_custom_properties.size(), 0);
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

    auto xml_from_props = exp_props.generate();
    BOOST_REQUIRE(!xml_from_props.empty());

    {
        ExportProperties exp_props;
        BOOST_CHECK(exp_props.parse(xml_from_props));

        BOOST_REQUIRE(exp_props.m_channels.size() == 2);
        {
            BOOST_CHECK_EQUAL(exp_props.m_channels.at(0), 10056);
            BOOST_CHECK_EQUAL(exp_props.m_channels.at(1), 54992);
        }

        BOOST_REQUIRE(exp_props.m_export_intervals.size() == 2);
        {
            BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

            BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
        }

        BOOST_CHECK(exp_props.m_custom_properties.size() == 0);
        BOOST_CHECK_EQUAL(exp_props.m_filename, "testfile.ext");
    }
}

BOOST_AUTO_TEST_CASE(CreateXML_RootNode)
{

    //! xml without xml header <?xml version='1.0' encoding='UTF-8'?>
    const char* const xml_content =
        R"xxx(<ExportProperties>
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
                </CustomProperties>
                <FormatId format="MDF41"/>
            </ExportProperties>
            )xxx"
            ;

    ExportProperties exp_props;
    exp_props.m_channels.push_back(10056);
    exp_props.m_channels.push_back(54992);

    exp_props.m_export_intervals.push_back(Interval<double>(0.2, 1.1));
    exp_props.m_export_intervals.push_back(Interval<double>(4.0, 1200.4));

    exp_props.m_format_id = "MDF41";

    {
        //! no header in xml string
        const auto xml_from_props = exp_props.generateNodeXML();
        const auto pos = xml_from_props.find("<ExportProperties>");
        BOOST_CHECK_EQUAL(pos, 0);
    }


    //! load and check generated xml
    {
        ExportProperties new_exp_props;
        BOOST_CHECK(new_exp_props.parse(exp_props.generateNodeXML()));

        BOOST_REQUIRE_EQUAL(new_exp_props.m_channels.size(), 2);
        {
            BOOST_CHECK_EQUAL(new_exp_props.m_channels.at(0), 10056);
            BOOST_CHECK_EQUAL(new_exp_props.m_channels.at(1), 54992);
        }

        BOOST_REQUIRE_EQUAL(new_exp_props.m_export_intervals.size(), 2);
        {
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
        }

        BOOST_CHECK(new_exp_props.m_custom_properties.size() == 0);
    }
    //! load and check generated pugi node
    {
        ExportProperties new_exp_props;
        BOOST_CHECK(new_exp_props.parse(exp_props.generate()));

        BOOST_REQUIRE(new_exp_props.m_channels.size() == 2);
        {
            BOOST_CHECK_EQUAL(new_exp_props.m_channels.at(0), 10056);
            BOOST_CHECK_EQUAL(new_exp_props.m_channels.at(1), 54992);
        }

        BOOST_REQUIRE(new_exp_props.m_export_intervals.size() == 2);
        {
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
            BOOST_CHECK_CLOSE(new_exp_props.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
        }

        BOOST_CHECK(new_exp_props.m_custom_properties.size() == 0);
    }

}
BOOST_AUTO_TEST_CASE(ParseXML_additional_settings)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
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
                    <Property name="ZIP_DATA"><BooleanValue>True</BooleanValue></Property>
                </CustomProperties>
                <FormatId format="MDF41"/>
            </ExportProperties>
            )xxx"
            ;

    ExportProperties exp_props;
    BOOST_CHECK(exp_props.parse(xml_content));

    BOOST_CHECK_EQUAL(exp_props.m_filename, "test.ext");

    BOOST_REQUIRE(exp_props.m_channels.size() == 2);
    {
        BOOST_CHECK_EQUAL(exp_props.m_channels.at(0), 10056);
        BOOST_CHECK_EQUAL(exp_props.m_channels.at(1), 54992);
    }

    BOOST_REQUIRE(exp_props.m_export_intervals.size() == 2);
    {
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_begin, 0.2, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(0).m_end, 1.1, FP_COMPARISON_TOLERANCE);

        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_begin, 4.0, FP_COMPARISON_TOLERANCE);
        BOOST_CHECK_CLOSE(exp_props.m_export_intervals.at(1).m_end, 1200.4, FP_COMPARISON_TOLERANCE);
    }

    BOOST_REQUIRE(exp_props.m_custom_properties.size() == 1);
    BOOST_CHECK_EQUAL(exp_props.m_custom_properties.begin()->getName(), "ZIP_DATA");
    BOOST_CHECK_EQUAL(exp_props.m_custom_properties.begin()->getBoolValue(), true);
}

BOOST_AUTO_TEST_SUITE_END()
