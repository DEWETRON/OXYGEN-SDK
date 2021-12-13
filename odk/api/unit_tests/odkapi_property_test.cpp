// Copyright DEWETRON GmbH 2021
#include "odkapi_property_xml.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(property_test_suite)

BOOST_AUTO_TEST_CASE(ScalarProperty)
{
    odk::Scalar default_constructed;
    BOOST_CHECK_EQUAL(default_constructed.m_val, 0);
    BOOST_CHECK_EQUAL(default_constructed.m_unit, "");

    odk::Scalar val_constructed(123, "Unit");
    BOOST_CHECK_EQUAL(val_constructed.m_val, 123);
    BOOST_CHECK_EQUAL(val_constructed.m_unit, "Unit");

    odk::Property p;
    p.setValue(val_constructed);
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::SCALAR);
    const auto& p_val = p.getScalarValue();
    BOOST_CHECK_EQUAL(p_val.m_val, 123);
    BOOST_CHECK_EQUAL(p_val.m_unit, "Unit");
}

BOOST_AUTO_TEST_SUITE_END()
