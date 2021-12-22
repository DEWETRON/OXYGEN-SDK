// Copyright DEWETRON GmbH 2017

#include "odkapi_block_descriptor_xml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

BOOST_AUTO_TEST_SUITE(block_descriptor)

BOOST_AUTO_TEST_CASE(parse_generate)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <BlockDescriptor stream_id="3" data_size="1024">
                <Channel channel_id="1" offset="0" first_sample_index="0" count="100" timestamp="0" duration="100" />
                <Channel channel_id="2" offset="8" first_sample_index="100" count="200" timestamp="200" duration="300" />
            </BlockDescriptor>
            )xxx"
            ;

    BlockDescriptor block_descriptor;
    BOOST_CHECK(block_descriptor.parse(xml_content));

    BOOST_REQUIRE_EQUAL(block_descriptor.m_block_channels.size(), 2);

    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_channel_id, 1);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_offset, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_first_sample_index, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_count, 100);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_timestamp, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_duration, 100);

    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_channel_id, 2);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_offset, 8);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_first_sample_index, 100);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_count, 200);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_timestamp, 200);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_duration, 300);

    auto serialized = block_descriptor.generate();
    BOOST_CHECK(block_descriptor.parse(serialized.c_str(), serialized.size()));

    BOOST_REQUIRE_EQUAL(block_descriptor.m_block_channels.size(), 2);

    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_channel_id, 1);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_offset, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_first_sample_index, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_count, 100);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_timestamp, 0);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(0).m_duration, 100);

    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_channel_id, 2);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_offset, 8);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_first_sample_index, 100);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_count, 200);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_timestamp, 200);
    BOOST_CHECK_EQUAL(block_descriptor.m_block_channels.at(1).m_duration, 300);
}

BOOST_AUTO_TEST_SUITE_END()
