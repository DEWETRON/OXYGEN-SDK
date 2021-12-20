// Copyright DEWETRON GmbH 2021
#include "odkapi_property_xml.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(property_test_suite)

BOOST_AUTO_TEST_CASE(Property)
{
    const odk::Property p_default;
    BOOST_CHECK(p_default.getType() == odk::Property::UNKNOWN);
    BOOST_CHECK_EQUAL(p_default.getName(), "");
    BOOST_CHECK_EQUAL(p_default.getNodeName(), "Property");
    BOOST_CHECK_EQUAL(p_default.isValid(), false);

    const odk::Property p_named("TestName");
    BOOST_CHECK(p_named.getType() == odk::Property::UNKNOWN);
    BOOST_CHECK_EQUAL(p_named.getName(), "TestName");
}

BOOST_AUTO_TEST_CASE(StringProperty)
{
    odk::Property p;
    p.setValue("TestString Value");
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::STRING);
    BOOST_CHECK_EQUAL(p.valueToString(), "TestString Value");
    BOOST_CHECK_EQUAL(p.getStringValue(), "TestString Value");
    BOOST_CHECK_EQUAL(p.isValid(), true);

    const odk::Property p2("Name", "TestString Value");
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::STRING);
    BOOST_CHECK_EQUAL(p2.valueToString(), "TestString Value");
    BOOST_CHECK_EQUAL(p2.getStringValue(), "TestString Value");

    const odk::Property p3("Name", std::string("TestString Value-string"));
    BOOST_REQUIRE_EQUAL(p3.getType(), odk::Property::STRING);
    BOOST_CHECK_EQUAL(p3.valueToString(), "TestString Value-string");
    BOOST_CHECK_EQUAL(p3.getStringValue(), "TestString Value-string");
}

BOOST_AUTO_TEST_CASE(IntProperty)
{
    odk::Property p;
    p.setValue(123);
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::INTEGER);
    BOOST_CHECK_EQUAL(p.valueToString(), "123");
    BOOST_CHECK_EQUAL(p.getIntValue(), 123);
    BOOST_CHECK_EQUAL(p.isValid(), true);

    const odk::Property p2("Name", 123);
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::INTEGER);
    BOOST_CHECK_EQUAL(p2.valueToString(), "123");
    BOOST_CHECK_EQUAL(p2.getIntValue(), 123);

    const odk::Property p3("Name", -123);
    BOOST_REQUIRE_EQUAL(p3.getType(), odk::Property::INTEGER);
    BOOST_CHECK_EQUAL(p3.valueToString(), "-123");
    BOOST_CHECK_EQUAL(p3.getIntValue(), -123);
}

BOOST_AUTO_TEST_CASE(UIntProperty)
{
    odk::Property p;
    p.setValue(123u);
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::UNSIGNED_INTEGER);
    BOOST_CHECK_EQUAL(p.valueToString(), "123");
    BOOST_CHECK_EQUAL(p.getUnsignedIntValue(), 123);
    BOOST_CHECK_EQUAL(p.isValid(), true);

    const odk::Property p2("Name", 123u);
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::UNSIGNED_INTEGER);
    BOOST_CHECK_EQUAL(p2.valueToString(), "123");
    BOOST_CHECK_EQUAL(p2.getUnsignedIntValue(), 123);
}

BOOST_AUTO_TEST_CASE(BoolProperty)
{
    odk::Property p;
    p.setValue(true);
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::BOOLEAN);
    BOOST_CHECK_EQUAL(p.valueToString(), "True");
    BOOST_CHECK_EQUAL(p.getBoolValue(), true);

    const odk::Property p2("Name", false);
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::BOOLEAN);
    BOOST_CHECK_EQUAL(p2.valueToString(), "False");
    BOOST_CHECK_EQUAL(p2.getBoolValue(), false);
}

BOOST_AUTO_TEST_CASE(ScalarProperty)
{
    const odk::Scalar default_constructed;
    BOOST_CHECK_EQUAL(default_constructed.m_val, 0);
    BOOST_CHECK_EQUAL(default_constructed.m_unit, "");

    const odk::Scalar val_constructed(123, "Unit");
    BOOST_CHECK_EQUAL(val_constructed.m_val, 123);
    BOOST_CHECK_EQUAL(val_constructed.m_unit, "Unit");

    odk::Property p;
    p.setValue(val_constructed);
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::SCALAR);
    BOOST_CHECK_EQUAL(p.valueToString(), "123 Unit");
    const auto& p_val = p.getScalarValue();
    BOOST_CHECK_EQUAL(p_val.m_val, 123);
    BOOST_CHECK_EQUAL(p_val.m_unit, "Unit");

    const odk::Property p2("Name", val_constructed);
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::SCALAR);
    const auto& p2_val = p2.getScalarValue();
    BOOST_CHECK_EQUAL(p2_val.m_val, 123);
    BOOST_CHECK_EQUAL(p2_val.m_unit, "Unit");
}

BOOST_AUTO_TEST_CASE(EnumProperty)
{
    odk::Property p;
    p.setEnumValue("TestEnum Value", "Typestring");
    BOOST_REQUIRE_EQUAL(p.getType(), odk::Property::ENUM);
    BOOST_CHECK_EQUAL(p.valueToString(), "TestEnum Value");
    BOOST_CHECK_EQUAL(p.getEnumType(), "Typestring");
    BOOST_CHECK_EQUAL(p.getEnumValue(), "TestEnum Value");

    const odk::Property p2("Name", "Value", "EnumType");
    BOOST_REQUIRE_EQUAL(p2.getType(), odk::Property::ENUM);
    BOOST_CHECK_EQUAL(p2.valueToString(), "Value");
    BOOST_CHECK_EQUAL(p2.getEnumType(), "EnumType");
    BOOST_CHECK_EQUAL(p2.getStringValue(), "Value");

    odk::Property p_notanenum;
    BOOST_CHECK_THROW(auto t = p_notanenum.getEnumType(), std::runtime_error);
    BOOST_CHECK_THROW(auto v = p_notanenum.getEnumValue(), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
