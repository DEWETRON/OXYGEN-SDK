// Copyright DEWETRON GmbH 2026

#include "odkfw_properties.h"
#include "odkapi_update_config_xml.h"

#include <boost/test/unit_test.hpp>

#include <limits>

namespace
{
    odk::Property reportedValue(const odk::framework::PropertyBase& property)
    {
        odk::UpdateConfigTelegram telegram;
        auto& channel = telegram.addChannel(1);
        property.addToTelegram(channel, "Test");
        const auto* reported = channel.getProperty("Test");
        BOOST_REQUIRE(reported);
        return *reported;
    }
}

BOOST_AUTO_TEST_SUITE(odkfw_properties_test)

BOOST_AUTO_TEST_CASE(EditableScalarPropertyStringOption)
{
    // "Auto" is an alias for the value 0
    odk::framework::EditableScalarProperty property(0.0, "Hz", 0.0, 1000.0);
    property.addStringOption("Auto", 0.0);

    // a value matching a string option is reported as that string
    auto reported = reportedValue(property);
    BOOST_CHECK(reported.getType() == odk::Property::STRING);
    BOOST_CHECK_EQUAL(reported.getStringValue(), "Auto");
    BOOST_CHECK(property.getValue() == odk::Scalar(0, "Hz"));

    // any other value is reported as scalar
    BOOST_CHECK(property.update(odk::Property("Test", odk::Scalar(500.0, "Hz"))));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 500.0);
    reported = reportedValue(property);
    BOOST_CHECK(reported.getType() == odk::Property::SCALAR);
    BOOST_CHECK_EQUAL(reported.getScalarValue().m_val, 500.0);

    // selecting the string option sets its value
    BOOST_CHECK(property.update(odk::Property("Test", "Auto")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 0.0);
    BOOST_CHECK_EQUAL(reportedValue(property).getStringValue(), "Auto");

    // plugin side: setting the aliased value is reported as the string option, too
    property.setValue(250.0, "Hz");
    BOOST_CHECK(reportedValue(property).getType() == odk::Property::SCALAR);
    property.setValue(0.0, "Hz");
    BOOST_CHECK_EQUAL(reportedValue(property).getStringValue(), "Auto");
    BOOST_CHECK_EQUAL(property.getValue().m_val, 0.0);
}

BOOST_AUTO_TEST_CASE(EditableScalarPropertyParsesNumericStrings)
{
    odk::framework::EditableScalarProperty property(0.0, "Hz", 0.0, 1000.0);
    property.addStringOption("Auto", 0.0);

    // numbers entered as text (e.g. in an editable combo box), optionally with the property's unit
    BOOST_CHECK(property.update(odk::Property("Test", "100")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 100.0);
    BOOST_CHECK(property.update(odk::Property("Test", "200.5 Hz")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 200.5);
    BOOST_CHECK(property.update(odk::Property("Test", "300Hz")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 300.0);

    // rejected strings keep the current value
    BOOST_CHECK(!property.update(odk::Property("Test", "Peach")));
    BOOST_CHECK(!property.update(odk::Property("Test", "400 V")));   // wrong unit
    BOOST_CHECK(!property.update(odk::Property("Test", "400abc")));  // trailing garbage
    BOOST_CHECK(!property.update(odk::Property("Test", "2000")));    // out of range
    BOOST_CHECK(!property.update(odk::Property("Test", "nan")));     // non-finite
    BOOST_CHECK(!property.update(odk::Property("Test", "inf")));
    BOOST_CHECK(!property.update(odk::Property("Test", odk::Scalar(2000.0, "Hz"))));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 300.0);
}

BOOST_AUTO_TEST_CASE(EditableScalarPropertyRejectsNonFiniteStrings)
{
    // unbounded range (e.g. transfer function stop frequency before the sample rate is known)
    odk::framework::EditableScalarProperty property(1.0, "Hz", 0.0, std::numeric_limits<double>::infinity());
    BOOST_CHECK(!property.update(odk::Property("Test", "inf")));
    BOOST_CHECK(!property.update(odk::Property("Test", "infinity")));
    BOOST_CHECK(!property.update(odk::Property("Test", "nan")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 1.0);
}

BOOST_AUTO_TEST_CASE(EditableScalarPropertyWithoutStringOption)
{
    odk::framework::EditableScalarProperty property(1.0, "V");

    // always reported as scalar
    BOOST_CHECK(reportedValue(property).getType() == odk::Property::SCALAR);

    // non-numeric strings are rejected, numeric ones are parsed
    BOOST_CHECK(!property.update(odk::Property("Test", "Auto")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 1.0);
    BOOST_CHECK(property.update(odk::Property("Test", "5 V")));
    BOOST_CHECK_EQUAL(property.getValue().m_val, 5.0);
}

BOOST_AUTO_TEST_SUITE_END()
