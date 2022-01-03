// Copyright DEWETRON GmbH 2018

#include "odkapi_timestamp_xml.h"

#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

using namespace odk;

BOOST_AUTO_TEST_SUITE(timestamp_parse)

BOOST_AUTO_TEST_CASE(parse_generate_parse)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
            <Timestamp ticks="123" frequency = "1000" />
            )xxx";

    Timestamp event;
    BOOST_CHECK(event.parse(xml_content));

    BOOST_CHECK_EQUAL(event.m_ticks, 123);
    BOOST_CHECK_EQUAL(event.m_frequency, 1000.0);

    BOOST_CHECK(event.parse(event.generate()));

    BOOST_CHECK_EQUAL(event.m_ticks, 123);
    BOOST_CHECK_EQUAL(event.m_frequency, 1000.0);
}

BOOST_AUTO_TEST_CASE(generate_parse)
{
    Timestamp event(2711, 1e6);

    Timestamp event2;
    BOOST_CHECK(event2.parse(event.generate()));

    BOOST_CHECK_EQUAL(event2.m_ticks, 2711);
    BOOST_CHECK_EQUAL(event2.m_frequency, 1e6);
}

BOOST_AUTO_TEST_SUITE_END()
