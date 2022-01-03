// Copyright DEWETRON GmbH 2019

#include "odkapi_update_config_xml.h"
#include "odkuni_xpugixml.h"

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>

#include <memory>

using namespace odk;

namespace std
{
    std::ostream& operator<<(std::ostream& os, const odk::Scalar& rhs)
    {
        return os
            << boost::lexical_cast<std::string>(rhs.m_val)
            << " "
            << rhs.m_unit;
    }

    std::ostream& operator<<(std::ostream& os, const odk::IfValue::Type& rhs)
    {
        return os
            << boost::lexical_cast<std::string>(static_cast<int>(rhs));
    }
}

BOOST_AUTO_TEST_SUITE(update_plugin_config_test)

BOOST_AUTO_TEST_CASE(ParseXML)
{
    const char* const xml_content =
        R"xxx(<?xml version='1.0' encoding='UTF-8'?>
<UpdateConfig protocol_version="1.0">

    <Channel local_id="2">
        <Property name = "Frequency">
            <ScalarValue>
                <Value>10000</Value>
                <Unit>Hz</Unit>
            </ScalarValue>
            <Constraints>
                <DoubleRangeConstraint min = "1" max = "50000"/>
                <OptionConstraint>
                    <ScalarValue>
                        <Value>1000</Value>
                        <Unit>Hz</Unit>
                    </ScalarValue>
                </OptionConstraint>
            </Constraints>
        </Property>
        <Property name = "Waveform">
            <StringValue>Square</StringValue>
            <Constraints>
                <OptionConstraint>
                    <StringValue>"Sine"</StringValue>
                </OptionConstraint>
                <OptionConstraint>
                    <StringValue>"Square"</StringValue>
                </OptionConstraint>
                <OptionConstraint>
                    <StringValue>"Triangle"</StringValue>
                </OptionConstraint>
                <OptionConstraint>
                    <StringValue>"Sawtooth"</StringValue>
                </OptionConstraint>
            </Constraints>
        </Property>

    </Channel>
</UpdateConfig>
            )xxx"
            ;

    using namespace odk;
    UpdateConfigTelegram telegram;
    BOOST_CHECK(telegram.parse(xml_content));

    BOOST_REQUIRE_EQUAL(telegram.m_channel_configs.size(), 1);
    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_channel_info.m_local_id, 2);
    BOOST_REQUIRE_EQUAL(telegram.m_channel_configs[0].m_properties.size(), 2);

    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[0].getName(), "Frequency");
    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[0].getType(), odk::Property::SCALAR);
    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[0].getScalarValue() , odk::Scalar(10000.0, "Hz"));

    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[1].getName(), "Waveform");
    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[1].getType(), odk::Property::STRING);
    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_properties[1].getStringValue(), "Square");

    BOOST_CHECK_EQUAL(telegram.m_channel_configs[0].m_constraints.size(), 2);

    {
        const auto& freq_constraints = telegram.m_channel_configs[0].m_constraints.at("Frequency");
        BOOST_REQUIRE_EQUAL(freq_constraints.size(), 2);
        BOOST_CHECK_EQUAL(freq_constraints[0].getType(), UpdateConfigTelegram::Constraint::RANGE);
        const auto range_constraint = freq_constraints[0];
        BOOST_CHECK_EQUAL(range_constraint.getRangeMin().getDoubleValue(), 1);
        BOOST_CHECK_EQUAL(range_constraint.getRangeMax().getDoubleValue(), 50000);
        BOOST_REQUIRE_EQUAL(freq_constraints[1].getOptions().size(), 1);
        BOOST_CHECK_EQUAL(freq_constraints[1].getOptions()[0].getType(), odk::Property::SCALAR);
        BOOST_CHECK_EQUAL(freq_constraints[1].getOptions()[0].getScalarValue(), Scalar(1000.0, "Hz"));
    }

    {
        const auto& wave_constraints = telegram.m_channel_configs[0].m_constraints.at("Waveform");
        BOOST_REQUIRE_EQUAL(wave_constraints.size(), 4);

        BOOST_CHECK_EQUAL(wave_constraints[0].getType(), UpdateConfigTelegram::Constraint::OPTIONS);
        BOOST_REQUIRE_EQUAL(wave_constraints[0].getOptions().size(), 1);
        BOOST_CHECK_EQUAL(wave_constraints[0].getOptions()[0].getType(), odk::Property::STRING);
        BOOST_CHECK_EQUAL(wave_constraints[0].getOption(0).getStringValue(), "Sine");

        BOOST_CHECK_EQUAL(wave_constraints[1].getType(), UpdateConfigTelegram::Constraint::OPTIONS);
        BOOST_REQUIRE_EQUAL(wave_constraints[1].getOptions().size(), 1);
        BOOST_CHECK_EQUAL(wave_constraints[1].getOptions()[0].getType(), odk::Property::STRING);
        BOOST_CHECK_EQUAL(wave_constraints[1].getOption(0).getStringValue(), "Square");

        BOOST_CHECK_EQUAL(wave_constraints[2].getType(), UpdateConfigTelegram::Constraint::OPTIONS);
        BOOST_REQUIRE_EQUAL(wave_constraints[2].getOptions().size(), 1);
        BOOST_CHECK_EQUAL(wave_constraints[2].getOptions()[0].getType(), odk::Property::STRING);
        BOOST_CHECK_EQUAL(wave_constraints[2].getOption(0).getStringValue(), "Triangle");

        BOOST_CHECK_EQUAL(wave_constraints[3].getType(), UpdateConfigTelegram::Constraint::OPTIONS);
        BOOST_REQUIRE_EQUAL(wave_constraints[3].getOptions().size(), 1);
        BOOST_CHECK_EQUAL(wave_constraints[3].getOptions()[0].getType(), odk::Property::STRING);
        BOOST_CHECK_EQUAL(wave_constraints[3].getOption(0).getStringValue(), "Sawtooth");
    }
}

BOOST_AUTO_TEST_CASE(CreateRequestXML)
{
    using namespace odk;

    UpdateConfigTelegram input;

    input.addChannel(2)
        .addProperty(Property("IP", "127.0.0.1"))
        .addProperty("Waveform", "Sawtooth")
        .addConstraint("Waveform", makeRangeConstraint(0.0, 1.21))
        .addOptionConstraint("Waveform", odk::Property("", "Sine"))
        .addOptionConstraint("Waveform", odk::Property("", "Square"))
        .addOptionConstraint("Waveform", odk::Property("", "Triangle"))
        .addOptionConstraint("Waveform", odk::Property("", "Sawtooth"));

    BOOST_CHECK(!input.getChannel(3));
    BOOST_CHECK(input.getChannel(2));

    input.getChannel(2)->addProperty("Frequency", Scalar(50.0, "Hz"));
    input.getChannel(2)->addConstraint("Frequency", makeRangeConstraint(1.0, 50000));
    input.getChannel(2)->addOptionConstraint("Frequency", odk::Property("", Scalar(1000.0, "Hz")));

    input.getChannel(2)->updateProperty("Waveform", Property::STRING, "Square");

    auto xml = input.generate();

    UpdateConfigTelegram output;

    BOOST_CHECK(!(input == output));
    BOOST_CHECK(output.parse(xml));
    BOOST_CHECK(input == output);

    BOOST_REQUIRE_EQUAL(output.m_channel_configs.size(), 1);
    BOOST_REQUIRE(output.m_channel_configs[0].m_properties.size() >= 3);
    BOOST_CHECK_EQUAL(output.m_channel_configs[0].m_properties[1].getStringValue(), "Square");
    BOOST_CHECK_EQUAL(output.m_channel_configs[0].m_properties[2].getScalarValue(), Scalar(50.0, "Hz"));
    BOOST_CHECK_EQUAL(output.m_channel_configs[0].getProperty("IP")->getStringValue(), "127.0.0.1");
    BOOST_CHECK_EQUAL(output.m_channel_configs[0].getProperty("Waveform")->getStringValue(), "Square");
    BOOST_CHECK_EQUAL(output.m_channel_configs[0].getProperty("Frequency")->getScalarValue(), Scalar(50.0, "Hz"));
    BOOST_CHECK_EQUAL(output.getChannel(2)->getProperty("IP")->getStringValue(), "127.0.0.1");
    BOOST_CHECK_EQUAL(output.getChannel(2)->getProperty("Waveform")->getStringValue(), "Square");
    BOOST_CHECK_EQUAL(output.getChannel(2)->getProperty("Frequency")->getScalarValue(), Scalar(50.0, "Hz"));

    input.getChannel(2)->updateProperty("Waveform", Property::STRING, "Sine");
    BOOST_CHECK(!(input == output));
}

BOOST_AUTO_TEST_CASE(UpdateByTelegram)
{
    using namespace odk;

    UpdateConfigTelegram config;
    config.addChannel(2)
        .addProperty(Property("ToChange", "Untouched"))
        .addProperty(Property("NotToChange", "Untouched"));
    config.addChannel(4)
        .addProperty(Property("NotToChange", "Untouched"));

    UpdateConfigTelegram update;
    update.addChannel(2)
        .addProperty(Property("ToChange", "Changed"))
        .addProperty(Property("NotAvailable", "Changed"));
    update.addChannel(3)
        .addProperty(Property("NotAvailable", "Changed"));

    UpdateConfigTelegram expected_result;
    expected_result.addChannel(2)
        .addProperty(Property("ToChange", "Changed"))
        .addProperty(Property("NotToChange", "Untouched"));
    expected_result.addChannel(4)
        .addProperty(Property("NotToChange", "Untouched"));

    config.update(update);

    BOOST_CHECK(config == expected_result);
}

BOOST_AUTO_TEST_SUITE_END()
