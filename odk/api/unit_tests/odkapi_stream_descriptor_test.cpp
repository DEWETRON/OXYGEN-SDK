// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_descriptor_xml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

BOOST_AUTO_TEST_SUITE(stream_descriptor)

BOOST_AUTO_TEST_CASE(stream_descriptor_parse)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <DataSetDescriptor data_set_key="418">
                <StreamDescriptor stream_id="1">
                    <Channel channel_id="1" size="64" type="Double" dimension="1" stride="64">
                        <Scaling>
                            <Linear factor="1.0" offset="0.0"/>>
                        </Scaling>
                    </Channel>
                    <Channel channel_id="2" size="32" type="24bit_sint" dimension="4" stride="32">
                        <Scaling>
                            <Linear factor="1.2" offset="3.4"/>
                        </Scaling>
                    </Channel>
                </StreamDescriptor>

                <StreamDescriptor stream_id="2">
                    <Channel channel_id="3" size="8" type="Blob" dimension="1" stride="64">
                        <Scaling>
                        </Scaling>
                        <TimeStamp position="-8"/>
                        <SampleSize position="-4"/>
                    </Channel>
                </StreamDescriptor>
            </DataSetDescriptor>
            )xxx"
            ;

    DataSetDescriptor data_set_descriptor;
    BOOST_CHECK(data_set_descriptor.parse(xml_content));

    BOOST_CHECK_EQUAL(data_set_descriptor.m_id, 418);

    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors.size(), 2);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_stream_id, 1);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors.size(), 2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_channel_id, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_size, 64);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_dimension, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_stride, 64);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_type == odk::SampleType::DOUBLE);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling[0].m_factor, 1.0);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling[0].m_offset, 0.0);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_channel_id, 2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_size, 32);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_dimension, 4);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_stride, 32);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_type == odk::SampleType::SIGNED24BIT);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling[0].m_factor, 1.2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling[0].m_offset, 3.4);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_stream_id, 2);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_channel_id, 3);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_size, 8);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_dimension, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_stride, 64);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_type == odk::SampleType::BLOB);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_scaling.size(), 0);

    BOOST_CHECK(data_set_descriptor.parse(data_set_descriptor.generate().c_str()));

    BOOST_CHECK_EQUAL(data_set_descriptor.m_id, 418);

    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors.size(), 2);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_stream_id, 1);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors.size(), 2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_channel_id, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_size, 64);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_dimension, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_stride, 64);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_type == odk::SampleType::DOUBLE);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling[0].m_factor, 1.0);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[0].m_scaling[0].m_offset, 0.0);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_channel_id, 2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_size, 32);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_dimension, 4);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_stride, 32);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_type == odk::SampleType::SIGNED24BIT);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling[0].m_factor, 1.2);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[0].m_channel_descriptors[1].m_scaling[0].m_offset, 3.4);

    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_stream_id, 2);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors.size(), 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_channel_id, 3);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_size, 8);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_dimension, 1);
    BOOST_CHECK_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_stride, 64);
    BOOST_CHECK(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_type == odk::SampleType::BLOB);
    BOOST_REQUIRE_EQUAL(data_set_descriptor.m_stream_descriptors[1].m_channel_descriptors[0].m_scaling.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
