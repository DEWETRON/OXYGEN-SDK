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

BOOST_AUTO_TEST_CASE(AcquisitionTaskProcessTelegramConstructor)
{
    odk::AcquisitionTaskProcessTelegram default_ctor;
    BOOST_CHECK_EQUAL(default_ctor.m_start.m_ticks, 0);
    BOOST_CHECK_EQUAL(default_ctor.m_end.m_ticks, 0);
}

BOOST_AUTO_TEST_SUITE_END()
