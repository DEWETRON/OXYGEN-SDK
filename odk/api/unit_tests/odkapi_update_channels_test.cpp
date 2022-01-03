// Copyright DEWETRON GmbH 2019

#include "odkapi_update_channels_xml.h"
#include "odkuni_xpugixml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

#include <cmath>

BOOST_AUTO_TEST_SUITE(update_plugin_channels_test)

BOOST_AUTO_TEST_CASE(ParseXML)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
<UpdatePluginChannels protocol_version="1.0">

    <Channel local_id="4294967294"/>

    <Channel local_id="2" default_name="Ch1" domain="UnitTests" deletable="true" valid="true">
        <DataFormat sample_occurrence="SYNC" sample_format="double" sample_dimension="11" />
        <SimpleTimebase frequency="10000"/>
        <Parent local_id="4294967294"/>
        <Property name = "Frequency">
            <ScalarValue>
                <Value>10000</Value>
                <Unit>Hz</Unit>
            </ScalarValue>
        </Property>
    </Channel>

    <ListTopology>
        <Group name="Test">
            <Channel local_id="4294967294">
                <Channel local_id="2"/>
            </Channel>
        </Group>
    </ListTopology>

</UpdatePluginChannels>
            )xxx"
            ;

    using namespace odk;
    UpdateChannelsTelegram telegram;
    BOOST_CHECK(telegram.parse(xml_content));

    BOOST_REQUIRE_EQUAL(telegram.m_channels.size(), 2);

    BOOST_CHECK_EQUAL(telegram.m_channels[0].m_local_id, 4294967294);
    BOOST_CHECK(telegram.m_channels[0].m_dataformat_info.m_sample_occurrence == ChannelDataformat::SampleOccurrence::INVALID);
    BOOST_CHECK(telegram.m_channels[0].m_dataformat_info.m_sample_format == ChannelDataformat::SampleFormat::INVALID);
    BOOST_CHECK_EQUAL(telegram.m_channels[0].m_dataformat_info.m_sample_dimension, -1);
    BOOST_CHECK(telegram.m_channels[0].m_timebase.m_type == Timebase::TimebaseType::NONE);
    BOOST_CHECK(std::isnan(telegram.m_channels[0].m_timebase.m_frequency));
    BOOST_CHECK(!telegram.m_channels[0].m_deletable);
    BOOST_CHECK(!telegram.m_channels[0].m_valid);
    auto default_channel = UpdateChannelsTelegram::PluginChannelInfo(4294967294);
    BOOST_CHECK(telegram.m_channels[0] == default_channel);

    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_local_id, 2);
    BOOST_CHECK(telegram.m_channels[1].m_dataformat_info.m_sample_occurrence == ChannelDataformat::SampleOccurrence::SYNC);
    BOOST_CHECK(telegram.m_channels[1].m_dataformat_info.m_sample_format == ChannelDataformat::SampleFormat::DOUBLE);
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_dataformat_info.m_sample_dimension, 11);
    BOOST_CHECK(telegram.m_channels[1].m_timebase.m_type == Timebase::TimebaseType::SIMPLE);
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_timebase.m_frequency, 10000.0);
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_default_name, "Ch1");
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_domain, "UnitTests");
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_local_parent_id, 4294967294);
    BOOST_CHECK(telegram.m_channels[1].m_deletable);
    BOOST_CHECK(telegram.m_channels[1].m_valid);

    BOOST_REQUIRE_EQUAL(telegram.m_channels[1].m_channel_config.m_properties.size(), 1);
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_channel_config.m_properties[0].getName(), "Frequency");
    BOOST_CHECK_EQUAL(telegram.m_channels[1].m_channel_config.m_properties[0].getType(), odk::Property::SCALAR);
    BOOST_CHECK(telegram.m_channels[1].m_channel_config.m_properties[0].getScalarValue() == odk::Scalar(10000.0, "Hz"));

    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children.size(), 1);
    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children[0].m_group_name, "Test");
    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children[0].m_children.size(), 1);
    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children[0].m_children[0].m_channel_id, 4294967294);

    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children[0].m_children[0].m_children.size(), 1);
    BOOST_REQUIRE_EQUAL(telegram.m_list_topology.m_children[0].m_children[0].m_children[0].m_channel_id, 2);
}

BOOST_AUTO_TEST_CASE(CreateRequestXML)
{
    using namespace odk;
    UpdateChannelsTelegram input;
    input.addChannel(2)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::SYNC,
            ChannelDataformat::SampleFormat::SINT32)
        .m_channel_config.addProperty(odk::Property("IP", "127.0.0.1"));

    input.addChannel(5)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::ASYNC,
            ChannelDataformat::SampleFormat::DOUBLE, 5)
        .setSimpleTimebase(12345)
        .setDeletable(true)
        .setLocalParent(2)
        .setDefaultName("DoubleVector")
        .setDomain("Tests");

    input.addChannel(8)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::SINGLE_VALUE,
            ChannelDataformat::SampleFormat::COMPLEX_FLOAT, 10)
        .setLocalParent(5);

    input.addChannel(11)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::NEVER,
            ChannelDataformat::SampleFormat::NONE, 0)
        .setLocalParent(2);

    UpdateChannelsTelegram expected_output;
    expected_output.addChannel(2)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::SYNC,
            ChannelDataformat::SampleFormat::SINT32)
        .m_channel_config.addProperty(odk::Property("IP", "127.0.0.1"));

    expected_output.addChannel(5)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::ASYNC,
            ChannelDataformat::SampleFormat::DOUBLE, 5)
        .setSimpleTimebase(12345)
        .setDeletable(true)
        .setLocalParent(2)
        .setDefaultName("DoubleVector")
        .setDomain("Tests");

    expected_output.addChannel(11)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::NEVER,
            ChannelDataformat::SampleFormat::NONE, 0)
        .setLocalParent(2);

    expected_output.addChannel(8)
        .setSampleFormat(
            ChannelDataformat::SampleOccurrence::SINGLE_VALUE,
            ChannelDataformat::SampleFormat::COMPLEX_FLOAT, 10)
        .setLocalParent(5);

    auto xml = input.generate();

    odk::UpdateChannelsTelegram output;

    BOOST_CHECK(!(input == output));
    BOOST_CHECK(output.parse(xml.c_str()));
    BOOST_CHECK(expected_output == output);

    BOOST_CHECK(getRootChannels(output) == std::vector<std::uint32_t>(std::initializer_list<std::uint32_t>({ 2u })));
    BOOST_CHECK(getChildrenOfChannel(output, 2, false) == std::vector<std::uint32_t>({5, 11}));
    BOOST_CHECK(getChildrenOfChannel(output, 5, false) == std::vector<std::uint32_t>(std::initializer_list<std::uint32_t>({ 8u })));
    BOOST_CHECK(getChildrenOfChannel(output, 8, false) == std::vector<std::uint32_t>());
    BOOST_CHECK(getChildrenOfChannel(output, 2, true) == std::vector<std::uint32_t>({5, 8, 11}));
}

BOOST_AUTO_TEST_CASE(CreateRequestXML2)
{
    using namespace odk;
    UpdateChannelsTelegram input;
    auto& g = input.m_list_topology.appendGroup("Plugin Group");
    auto& gc = g.appendChannel(1);
    gc.appendChannel(2);
    g.appendChannel(11);
    BOOST_CHECK_EQUAL(input.m_list_topology.m_children[0].m_group_name, "Plugin Group");
    BOOST_CHECK_EQUAL(input.m_list_topology.m_children[0].m_children[0].m_channel_id, 1);

    auto xml = input.generate();

    odk::UpdateChannelsTelegram output;

    BOOST_CHECK(input != output);
    BOOST_CHECK(output.parse(xml.c_str()));
    BOOST_CHECK(input == output);
}

BOOST_AUTO_TEST_SUITE_END()
