// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_xml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

BOOST_AUTO_TEST_SUITE(data_set_parse)

BOOST_AUTO_TEST_CASE(parse_generate_parse)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <RegisterDataSet protocol_version="1.0" data_set_key="418" type="SCALED" mode="NORMAL">
                <Channels>
                  <Channel channel_id="1"/>
                  <Channel channel_id="2"/>
                </Channels>
              </RegisterDataSet>
            )xxx"
            ;

    PluginDataSet data_set;
    BOOST_CHECK(data_set.parse(xml_content));

    BOOST_CHECK(data_set.m_channels.size() == 2);

    BOOST_CHECK(data_set.parse(data_set.generate()));

    BOOST_CHECK(data_set.m_channels.size() == 2);
}
BOOST_AUTO_TEST_CASE(PluginDataSet_data_test)
{

    {
        const char* const xml_content =
            R"xxx(<?xml version='1.0' encoding='UTF-8'?>
                <RegisterDataSet protocol_version="1.0" data_set_key="418" type="SCALED" mode="NORMAL">
                    <Channels>
                      <Channel channel_id="1"/>
                      <Channel channel_id="2"/>
                    </Channels>
                  </RegisterDataSet>
                )xxx"
                ;

        PluginDataSet data_set;
        BOOST_CHECK(data_set.parse(xml_content));

        BOOST_CHECK(data_set.m_channels.size() == 2);
        BOOST_CHECK(data_set.m_id == 418);
        BOOST_CHECK(data_set.m_data_set_type == DataSetType::SCALED);
        BOOST_CHECK(data_set.m_data_mode == DataSetMode::NORMAL);
        BOOST_CHECK(data_set.m_policy == StreamPolicy::EXACT);
    }
    {
        const char* const xml_content =
            R"xxx(<?xml version='1.0' encoding='UTF-8'?>
                <RegisterDataSet protocol_version="1.0" data_set_key="418" type="RAW" mode="NORMAL">
                    <Channels>
                      <Channel channel_id="1"/>
                      <Channel channel_id="2"/>
                    </Channels>
                  </RegisterDataSet>
                )xxx"
                ;

        PluginDataSet data_set;
        BOOST_CHECK(data_set.parse(xml_content));

        BOOST_CHECK(data_set.m_channels.size() == 2);
        BOOST_CHECK(data_set.m_id == 418);
        BOOST_CHECK(data_set.m_data_set_type == DataSetType::RAW);
        BOOST_CHECK(data_set.m_data_mode == DataSetMode::NORMAL);
    }
    {
        const char* const xml_content =
            R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <RegisterDataSet protocol_version="1.0" data_set_key="418" type="RAW" mode="NORMAL" policy="EXACT">
                <Channels>
                    <Channel channel_id="1"/>
                    <Channel channel_id="2"/>
                </Channels>
                </RegisterDataSet>
            )xxx"
            ;

        PluginDataSet data_set;
        BOOST_CHECK(data_set.parse(xml_content));

        BOOST_CHECK(data_set.m_channels.size() == 2);
        BOOST_CHECK(data_set.m_id == 418);
        BOOST_CHECK(data_set.m_data_set_type == DataSetType::RAW);
        BOOST_CHECK(data_set.m_data_mode == DataSetMode::NORMAL);
        BOOST_CHECK(data_set.m_policy == StreamPolicy::EXACT);
    }
    {
        const char* const xml_content =
            R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <RegisterDataSet protocol_version="1.0" data_set_key="418" type="RAW" mode="NORMAL" policy="RELAXED">
                <Channels>
                    <Channel channel_id="1"/>
                    <Channel channel_id="2"/>
                </Channels>
                </RegisterDataSet>
            )xxx"
            ;

        PluginDataSet data_set;
        BOOST_CHECK(data_set.parse(xml_content));

        BOOST_CHECK(data_set.m_channels.size() == 2);
        BOOST_CHECK(data_set.m_id == 418);
        BOOST_CHECK(data_set.m_data_set_type == DataSetType::RAW);
        BOOST_CHECK(data_set.m_data_mode == DataSetMode::NORMAL);
        BOOST_CHECK(data_set.m_policy == StreamPolicy::RELAXED);
    }

}

BOOST_AUTO_TEST_CASE(parse_generate_parse_data_request)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataTransferRequest data_set_key="418">
                <Window start="0.0" end="12.34"/>
            </DataTransferRequest>
            )xxx"
    ;

    PluginDataRequest data_request;
    BOOST_CHECK(data_request.parse(xml_content));

    BOOST_CHECK_EQUAL(data_request.m_id, 418);
    BOOST_REQUIRE(data_request.m_data_window);
    BOOST_CHECK_EQUAL(data_request.m_data_window->m_start, 0.0);
    BOOST_CHECK_EQUAL(data_request.m_data_window->m_stop, 12.34);

    BOOST_CHECK(data_request.parse(data_request.generate()));

    BOOST_CHECK_EQUAL(data_request.m_id, 418);
    BOOST_REQUIRE(data_request.m_data_window);
    BOOST_CHECK_EQUAL(data_request.m_data_window->m_start, 0.0);
    BOOST_CHECK_EQUAL(data_request.m_data_window->m_stop, 12.34);
}

BOOST_AUTO_TEST_CASE(parse_generate_parse_data_request_single_value)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataTransferRequest data_set_key="418">
                <SingleValue timestamp="0.123"/>
            </DataTransferRequest>
            )xxx"
    ;

    PluginDataRequest data_request;
    BOOST_CHECK(data_request.parse(xml_content));

    BOOST_CHECK_EQUAL(data_request.m_id, 418);
    BOOST_REQUIRE(data_request.m_single_value);
    BOOST_CHECK_EQUAL(data_request.m_single_value->m_timestamp, 0.123);

    BOOST_CHECK(data_request.parse(data_request.generate()));

    BOOST_CHECK_EQUAL(data_request.m_id, 418);
    BOOST_REQUIRE(data_request.m_single_value);
    BOOST_CHECK_EQUAL(data_request.m_single_value->m_timestamp, 0.123);
}

BOOST_AUTO_TEST_CASE(parse_generate_parse_data_start_request)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataTransferRequest data_set_key="418">
                <Stream start="0.0" block_duration="0.1"/>
            </DataTransferRequest>
            )xxx"
    ;

    PluginDataStartRequest data_start_request;
    BOOST_CHECK(data_start_request.parse(xml_content));

    BOOST_CHECK_EQUAL(data_start_request.m_id, 418);
    BOOST_CHECK(data_start_request.m_start);
    BOOST_CHECK_EQUAL(data_start_request.m_start.get(), 0.0);
    BOOST_CHECK_EQUAL(data_start_request.m_block_duration.get(), 0.1);

    BOOST_CHECK(data_start_request.parse(data_start_request.generate().c_str()));

    BOOST_CHECK_EQUAL(data_start_request.m_id, 418);
    BOOST_CHECK(data_start_request.m_start);
    BOOST_CHECK_EQUAL(data_start_request.m_start.get(), 0.0);
    BOOST_CHECK_EQUAL(data_start_request.m_block_duration.get(), 0.1);
}

BOOST_AUTO_TEST_CASE(parse_generate_parse_data_start_request_optional)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataTransferRequest data_set_key="418">
                <Stream block_duration="0.1"/>
            </DataTransferRequest>
            )xxx"
    ;

    PluginDataStartRequest data_start_request;
    BOOST_CHECK(data_start_request.parse(xml_content));

    BOOST_CHECK_EQUAL(data_start_request.m_id, 418);
    BOOST_CHECK(!data_start_request.m_start);
    BOOST_CHECK_EQUAL(data_start_request.m_block_duration.get(), 0.1);

    BOOST_CHECK(data_start_request.parse(data_start_request.generate().c_str()));

    BOOST_CHECK_EQUAL(data_start_request.m_id, 418);
    BOOST_CHECK(!data_start_request.m_start);
    BOOST_CHECK_EQUAL(data_start_request.m_block_duration.get(), 0.1);
}

BOOST_AUTO_TEST_CASE(parse_generate_parse_data_stop_request)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataTransferRequest data_set_key="418"/>
            )xxx"
    ;

    PluginDataStopRequest data_stop_request;
    BOOST_CHECK(data_stop_request.parse(xml_content));
    BOOST_CHECK_EQUAL(data_stop_request.m_id, 418);

    BOOST_CHECK(data_stop_request.parse(data_stop_request.generate().c_str()));
    BOOST_CHECK_EQUAL(data_stop_request.m_id, 418);
}

BOOST_AUTO_TEST_SUITE_END()
