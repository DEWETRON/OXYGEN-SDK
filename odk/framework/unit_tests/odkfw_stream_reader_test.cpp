// Copyright DEWETRON GmbH 2017

#include "odkfw_stream_reader.h"
#include "odkapi_block_descriptor_xml.h"

#include <boost/test/unit_test.hpp>

using namespace odk;

BOOST_AUTO_TEST_SUITE(stream_reader)

BOOST_AUTO_TEST_CASE(stream_reader_test)
{
    BlockDescriptor bd1;
    bd1.m_stream_id = 123;
    {
        BlockChannelDescriptor bcd;
        bcd.m_channel_id = 1;
        bcd.m_count = 2;
        bcd.m_first_sample_index = 100;
        bcd.m_timestamp = 100;
        bcd.m_offset = 0;
        bd1.m_block_channels.push_back(bcd);
    }
    {
        BlockChannelDescriptor bcd;
        bcd.m_channel_id = 2;
        bcd.m_count = 2;
        bcd.m_first_sample_index = 100;
        bcd.m_timestamp = 100;
        bcd.m_offset = sizeof(double);
        bd1.m_block_channels.push_back(bcd);
    }
    BlockDescriptor bd2;
    bd2.m_stream_id = 123;
    {
        BlockChannelDescriptor bcd;
        bcd.m_channel_id = 1;
        bcd.m_count = 2;
        bcd.m_first_sample_index = 102;
        bcd.m_timestamp = 102;
        bcd.m_offset = 0;
        bd2.m_block_channels.push_back(bcd);
    }
    {
        BlockChannelDescriptor bcd;
        bcd.m_channel_id = 2;
        bcd.m_count = 2;
        bcd.m_first_sample_index = 102;
        bcd.m_timestamp = 102;
        bcd.m_offset = sizeof(double);
        bd2.m_block_channels.push_back(bcd);
    }

    std::vector<BlockDescriptor> bdv = {bd1, bd2};

    StreamDescriptor sd;
    sd.m_stream_id = 123;

    {
        ChannelDescriptor cd;
        cd.m_channel_id = 1;
        cd.m_dimension = 1;
        cd.m_stride = 2 * sizeof(double);
        cd.m_size = 64;
        cd.m_type = SampleType::DOUBLE;
        sd.m_channel_descriptors.push_back(cd);
    }
    {
        ChannelDescriptor cd;
        cd.m_channel_id = 2;
        cd.m_dimension = 1;
        cd.m_stride = 2 * sizeof(double);
        cd.m_size = 64;
        cd.m_type = SampleType::DOUBLE;
        sd.m_channel_descriptors.push_back(cd);
    }

    double data1[4] = {1, 10, 2, 20};
    double data2[4] = {3, 30, 4, 40};

    odk::framework::StreamReader stream_reader(sd);
    stream_reader.addDataBlock(bd1, data1);
    stream_reader.addDataBlock(bd2, data2);
    BOOST_REQUIRE(stream_reader.hasChannel(1));
    BOOST_CHECK(stream_reader.hasChannel(2));

    auto iterator1 = stream_reader.createChannelIterator(1);
    BOOST_CHECK(iterator1.valid());
    BOOST_CHECK_EQUAL(iterator1.data(), data1);
    BOOST_CHECK_EQUAL(iterator1.timestamp(), 100);

    ++iterator1;
    ++iterator1;
    BOOST_CHECK(iterator1.valid());
    BOOST_CHECK_EQUAL(iterator1.data(), data2);
    BOOST_CHECK_EQUAL(iterator1.timestamp(), 102);
}

BOOST_AUTO_TEST_SUITE_END()
