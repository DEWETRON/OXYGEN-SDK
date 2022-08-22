// Copyright DEWETRON GmbH 2020

#include "odkapi_data_set_descriptor_xml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

// MSVC warning https://lists.boost.org/boost-users/2014/11/83281.php
#if defined(_MSC_VER) && BOOST_VERSION == 105700
#pragma warning(disable:4003)
#endif

using namespace odk;

BOOST_AUTO_TEST_SUITE(data_set_descriptor)

BOOST_AUTO_TEST_CASE(parse_generate)
{

    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='utf-8'?>
            <DataSetDescriptor data_set_key="1234567">
                <StreamDescriptor stream_id="0">
                </StreamDescriptor>
                <StreamDescriptor stream_id="42">
                    <Channel channel_id="1234" size="2" stride="2" dimension="1" type="16bit_sint">
                        <Scaling>
                            <Linear factor="4" offset="16"/>
                        </Scaling>
                    </Channel>
                    <Channel channel_id="78" size="16" stride="24" dimension="2" type="double">
                        <Timestamp position="-8"/>
                    </Channel>
                </StreamDescriptor>
            </DataSetDescriptor>
            )xxx"
            ;

    DataSetDescriptor data_set;
    BOOST_CHECK(data_set.parse(xml_content));

    BOOST_CHECK_EQUAL(data_set.m_id, 1234567);
    BOOST_REQUIRE_EQUAL(data_set.m_stream_descriptors.size(), 2);

    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(0).m_stream_id, 0);
    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(0).m_channel_descriptors.size(), 0);

    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(1).m_stream_id, 42);

    BOOST_REQUIRE_EQUAL(data_set.m_stream_descriptors.at(1).m_channel_descriptors.size(), 2);

    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(0).m_channel_id, 1234);
    BOOST_CHECK(data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(0).m_scaling.at(0).m_type == odk::ScalingType::LINEAR);
    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(0).m_scaling.at(0).m_factor, 4);
    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(0).m_scaling.at(0).m_offset, 16);

    BOOST_CHECK_EQUAL(data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(1).m_channel_id, 78);
    BOOST_CHECK_EQUAL(*data_set.m_stream_descriptors.at(1).m_channel_descriptors.at(1).m_timestamp_position, -8);

    DataSetDescriptor data_set2;
    BOOST_CHECK(data_set2.parse(data_set.generate().c_str()));

    BOOST_CHECK(data_set2 == data_set);

    //VERIFY data_set2
}

BOOST_AUTO_TEST_SUITE_END()
