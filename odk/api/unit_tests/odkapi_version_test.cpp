// Copyright DEWETRON GmbH 2021
#include "odkapi_version_xml.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(version_test_suite)

BOOST_AUTO_TEST_CASE(Constructor)
{
    odk::Version default_version;
    BOOST_CHECK_EQUAL(default_version.m_major, 0);
    BOOST_CHECK_EQUAL(default_version.m_minor, 0);
    BOOST_CHECK_EQUAL(false, default_version.isValid());

    odk::Version major_version(123);
    BOOST_CHECK_EQUAL(major_version.m_major, 123);
    BOOST_CHECK_EQUAL(major_version.m_minor, 0);
    BOOST_CHECK_EQUAL(true, major_version.isValid());

    odk::Version major_minor_version(123, 456);
    BOOST_CHECK_EQUAL(major_minor_version.m_major, 123);
    BOOST_CHECK_EQUAL(major_minor_version.m_minor, 456);
}

BOOST_AUTO_TEST_CASE(Comparison)
{
    BOOST_CHECK_EQUAL(false, odk::Version(1, 3) == odk::Version(1, 2));
    BOOST_CHECK_EQUAL(false, odk::Version(1, 3) == odk::Version(2, 3));
    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) == odk::Version(1, 3));

    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) != odk::Version(1, 2));
    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) != odk::Version(2, 3));
    BOOST_CHECK_EQUAL(false, odk::Version(1, 3) != odk::Version(1, 3));

    BOOST_CHECK_EQUAL(false, odk::Version(1, 0) <= odk::Version(0, 99));
    BOOST_CHECK_EQUAL(true, odk::Version(2, 0) <= odk::Version(2, 0));
    BOOST_CHECK_EQUAL(false, odk::Version(3, 0) <= odk::Version(2, 0));
    BOOST_CHECK_EQUAL(true, odk::Version(3, 0) <= odk::Version(3, 1));

    BOOST_CHECK_EQUAL(false, odk::Version(1, 3) <= odk::Version(1, 2));
    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) <= odk::Version(1, 3));
    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) <= odk::Version(1, 4));

    BOOST_CHECK_EQUAL(true, odk::Version(1, 0) >= odk::Version(0, 99));
    BOOST_CHECK_EQUAL(true, odk::Version(2, 0) >= odk::Version(2, 0));
    BOOST_CHECK_EQUAL(true, odk::Version(3, 0) >= odk::Version(2, 0));
    BOOST_CHECK_EQUAL(false, odk::Version(3, 0) >= odk::Version(3, 1));

    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) >= odk::Version(1, 2));
    BOOST_CHECK_EQUAL(true, odk::Version(1, 3) >= odk::Version(1, 3));
    BOOST_CHECK_EQUAL(false, odk::Version(1, 3) >= odk::Version(1, 4));
}

BOOST_AUTO_TEST_CASE(Serialization)
{
    odk::Version version(1,3);
    BOOST_CHECK_EQUAL(version.generate(), "1.3");
    
    BOOST_CHECK(version.parse("5.2") == odk::Version(5, 2));
}

BOOST_AUTO_TEST_SUITE_END()
