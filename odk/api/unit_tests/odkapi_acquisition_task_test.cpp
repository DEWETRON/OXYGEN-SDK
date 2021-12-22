// Copyright DEWETRON GmbH 2021
#include "odkapi_acquisition_task_xml.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(acquisition_task_test_suite)

BOOST_AUTO_TEST_CASE(AddAcquisitionTaskTelegramConstructor)
{
    odk::AddAcquisitionTaskTelegram default_ctor;
    BOOST_CHECK_EQUAL(default_ctor.m_id, 0);
    BOOST_CHECK(default_ctor.m_input_channels.empty());
    BOOST_CHECK(default_ctor.m_output_channels.empty());
}

BOOST_AUTO_TEST_CASE(AddAcquisitionTaskTelegramSerialization)
{
    odk::AddAcquisitionTaskTelegram orig;
    orig.m_id = 123;
    orig.m_input_channels = { 1, 2, 3 };
    orig.m_output_channels = { 3, 1, 4 };

    auto serialized = orig.generate();
    BOOST_CHECK(!serialized.empty());

    odk::AddAcquisitionTaskTelegram deserialized;
    BOOST_CHECK(deserialized.parse(serialized.c_str()));
    BOOST_CHECK_EQUAL(deserialized.m_id, orig.m_id);
    BOOST_CHECK_EQUAL_COLLECTIONS(deserialized.m_input_channels.begin(), deserialized.m_input_channels.end(), orig.m_input_channels.begin(), orig.m_input_channels.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(deserialized.m_output_channels.begin(), deserialized.m_output_channels.end(), orig.m_output_channels.begin(), orig.m_output_channels.end());

    deserialized = {};
    BOOST_CHECK(deserialized.parse(serialized.c_str(), serialized.size()));
    BOOST_CHECK_EQUAL(deserialized.m_id, orig.m_id);
    BOOST_CHECK_EQUAL_COLLECTIONS(deserialized.m_input_channels.begin(), deserialized.m_input_channels.end(), orig.m_input_channels.begin(), orig.m_input_channels.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(deserialized.m_output_channels.begin(), deserialized.m_output_channels.end(), orig.m_output_channels.begin(), orig.m_output_channels.end());
}

BOOST_AUTO_TEST_CASE(AcquisitionTaskProcessTelegramConstructor)
{
    odk::AcquisitionTaskProcessTelegram default_ctor;
    BOOST_CHECK_EQUAL(default_ctor.m_start.m_ticks, 0);
    BOOST_CHECK_EQUAL(default_ctor.m_end.m_ticks, 0);
}

BOOST_AUTO_TEST_CASE(AcquisitionTaskProcessTelegramSerialization)
{
    odk::AcquisitionTaskProcessTelegram orig;
    orig.m_start = odk::Timestamp(100, 10000);
    orig.m_end = odk::Timestamp(20300, 10000);

    auto serialized = orig.generate();
    BOOST_CHECK(!serialized.empty());

    odk::AcquisitionTaskProcessTelegram deserialized;
    BOOST_CHECK(deserialized.parse(serialized.c_str()));
    BOOST_CHECK_EQUAL(deserialized.m_start.m_ticks, orig.m_start.m_ticks);
    BOOST_CHECK_EQUAL(deserialized.m_start.m_frequency, orig.m_start.m_frequency);
    BOOST_CHECK_EQUAL(deserialized.m_end.m_ticks, orig.m_end.m_ticks);
    BOOST_CHECK_EQUAL(deserialized.m_end.m_frequency, orig.m_end.m_frequency);

    deserialized = {};
    BOOST_CHECK(deserialized.parse(serialized.c_str(), serialized.size()));
    BOOST_CHECK_EQUAL(deserialized.m_start.m_ticks, orig.m_start.m_ticks);
    BOOST_CHECK_EQUAL(deserialized.m_start.m_frequency, orig.m_start.m_frequency);
    BOOST_CHECK_EQUAL(deserialized.m_end.m_ticks, orig.m_end.m_ticks);
    BOOST_CHECK_EQUAL(deserialized.m_end.m_frequency, orig.m_end.m_frequency);
}

BOOST_AUTO_TEST_SUITE_END()
