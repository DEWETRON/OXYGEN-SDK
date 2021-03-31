// Copyright DEWETRON GmbH 2019

#include "odkfw_properties.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_software_channel_xml.h"
#include "odkapi_utils.h"

#include <array>
#include <cmath>
#include <string.h>

//Parser should support an optional "DewetronPluginManifest" and only parse the OxygenPlugin child (if present).
static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_BIN_DETECTOR" version="1.0" uuid="DCDF634A-377B-4E9F-89BD-09D54C9DFCD3">
  <Info name="Example Plugin: Bin detector">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that extracts specific elements from vector channels into scalar channels</Description>
  </Info>
  <Host minimum_version="3.7"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_BIN_DETECTOR/MyInputChannel</source><translation>Input Channel</translation></message>
        <message><source>ODK_BIN_DETECTOR/EnableMin</source><translation>Enable Minimum</translation></message>
        <message><source>ODK_BIN_DETECTOR/EnableMax</source><translation>Enable Maximum</translation></message>
    </context>
</TS>
)XML";

static const char* KEY_INPUT_CHANNEL = "ODK_BIN_DETECTOR/MyInputChannel";
static const char* ENABLE_MIN_CHANNELS = "ODK_BIN_DETECTOR/EnableMin";
static const char* ENABLE_MAX_CHANNELS = "ODK_BIN_DETECTOR/EnableMax";

using namespace odk::framework;

class BinDetectorInstance : public SoftwareChannelInstance
{
public:

    BinDetectorInstance()
        : m_input_channel(new EditableChannelIDProperty())
        , m_enable_min(new EditableStringProperty("On"))
        , m_enable_max(new EditableStringProperty("On"))
    {
        m_enable_min->setArbitraryString(false);
        m_enable_min->addOption("On");
        m_enable_min->addOption("Off");

        m_enable_max->setArbitraryString(false);
        m_enable_max->addOption("On");
        m_enable_max->addOption("Off");
    }

    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Bin detector";
        telegram.m_service_name = "DetectMinMaxBins";
        telegram.m_display_group = "Basic Math";
        telegram.m_description = "Detect min/max values and corresponding bins of a vector channel";
        telegram.m_analysis_capable = true;
        return telegram;
    }

    void updatePropertyTypes(const PluginChannelPtr& output_channel) override
    {
        ODK_UNUSED(output_channel);
    }

    void updateStaticPropertyConstraints(const PluginChannelPtr& channel) override
    {
        ODK_UNUSED(channel);
    }

    InitResult init(const InitParams& params) override
    {
        if (params.m_input_channels.size() >= 1)
        {
            m_input_channel->setValue(params.m_input_channels[0].m_channel_id);
        }
        InitResult r { true };
        r.m_channel_list_action = InitResult::ChannelListAction::SHOW_DETAILS_OF_FIRST_CHANNEL;
        return r;
    }

    void updateMyOutputChannels(InputChannelPtr input_channel)
    {
        //check min config item and create/remove min channels
        if ((m_enable_min->getValue() == "On") && (!m_min_channels.m_value_channel))
        {
            m_min_channels.m_value_channel = addOutputChannel("min_value");
            m_min_channels.m_bin_channel = addOutputChannel("min_bin");
        }
        else if ((m_enable_min->getValue() == "Off") && m_min_channels.m_value_channel)
        {
            removeOutputChannel(m_min_channels.m_value_channel);
            removeOutputChannel(m_min_channels.m_bin_channel);
            m_min_channels.m_value_channel.reset();
            m_min_channels.m_bin_channel.reset();
        }

        //check max config item and create/remove max channels
        if ((m_enable_max->getValue() == "On") && (!m_max_channels.m_value_channel))
        {
            m_max_channels.m_value_channel = addOutputChannel("max_value");
            m_max_channels.m_bin_channel = addOutputChannel("max_bin");
        }
        else if ((m_enable_max->getValue() == "Off") && m_max_channels.m_value_channel)
        {
            removeOutputChannel(m_max_channels.m_value_channel);
            removeOutputChannel(m_max_channels.m_bin_channel);
            m_max_channels.m_value_channel.reset();
            m_max_channels.m_bin_channel.reset();
        }

        //configure outputchannels if we have an inputchannel
        if (input_channel)
        {
            const auto& input_range = input_channel->getRange();
            const auto& unit = input_channel->getUnit();
            //const auto& dimension = input_channel->getDataFormat().m_sample_dimension;

            if (m_min_channels.m_value_channel)
            {
                m_min_channels.m_value_channel->setDefaultName(input_channel->getName() + "_min_Value")
                    .setSampleFormat(
                        odk::ChannelDataformat::SampleOccurrence::ASYNC,
                        odk::ChannelDataformat::SampleFormat::DOUBLE,
                        1)
                    .setDeletable(true)
                    .setRange(input_range)
                    .setUnit(unit)
                    ;
            }

            if (m_min_channels.m_bin_channel)
            {
                m_min_channels.m_bin_channel->setDefaultName(input_channel->getName() + "_min_Bin")
                    .setSampleFormat(
                        odk::ChannelDataformat::SampleOccurrence::ASYNC,
                        odk::ChannelDataformat::SampleFormat::FLOAT,
                        1)
                    .setDeletable(true)
                    ;
            }

            if (m_max_channels.m_value_channel)
            {
                m_max_channels.m_value_channel->setDefaultName(input_channel->getName() + "_max_Value")
                    .setSampleFormat(
                        odk::ChannelDataformat::SampleOccurrence::ASYNC,
                        odk::ChannelDataformat::SampleFormat::DOUBLE,
                        1)
                    .setDeletable(true)
                    .setRange(input_range)
                    .setUnit(unit)
                    ;
            }

            if (m_max_channels.m_bin_channel)
            {
                m_max_channels.m_bin_channel->setDefaultName(input_channel->getName() + "_max_Bin")
                    .setSampleFormat(
                        odk::ChannelDataformat::SampleOccurrence::ASYNC,
                        odk::ChannelDataformat::SampleFormat::FLOAT,
                        1)
                    .setDeletable(true)
                    ;
            }
        }
    }

    bool update() override
    {
        auto all_input_channels(getInputChannelProxies());
        auto is_valid((all_input_channels.size() > 0));

        auto an_input_channel = getInputChannelProxy(m_input_channel->getValue());

        const auto dataformat = an_input_channel->getDataFormat();
        //check for a valid input channel type
        is_valid &= (dataformat.m_sample_value_type ==
            odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_VECTOR);
        //check for a valid sample size
        is_valid &= dataformat.m_sample_dimension > 0;

        if (!is_valid)
        {
            return is_valid;
        }

        m_dimension = dataformat.m_sample_dimension;
        updateMyOutputChannels(an_input_channel);

        return is_valid;
    }

    void updateInputChannelIDs(const std::map<uint64_t, uint64_t>& channel_mapping) override
    {
        ODK_UNUSED(channel_mapping);

        //the channels have already been mapped by base-class
        //remove all channel_ids that are invalid (not done by base-class)
    }

    void create(odk::IfHost *host) override
    {
        ODK_UNUSED(host);

        //configure my group channel
        //add properties for user interface
        getRootChannel()->setDefaultName("Bin Detector Group")
            .setDeletable(true)
            .addProperty(KEY_INPUT_CHANNEL, m_input_channel)
            .addProperty(ENABLE_MIN_CHANNELS, m_enable_min)
            .addProperty(ENABLE_MAX_CHANNELS, m_enable_max);
    }

    bool configure(
        const odk::UpdateChannelsTelegram& request,
        std::map<uint32_t, uint32_t>& channel_id_map) override
    {
        //restore my output channels after loading configuration
        configureFromTelegram(request, channel_id_map);

        m_min_channels.m_value_channel = getOutputChannelByKey("min_value");
        m_min_channels.m_bin_channel = getOutputChannelByKey("min_bin");
        m_max_channels.m_value_channel = getOutputChannelByKey("max_value");
        m_max_channels.m_bin_channel = getOutputChannelByKey("max_bin");

        return true;
    }

    void initTimebases(odk::IfHost* host) override
    {
        const auto master_timestamp = getMasterTimestamp(host);

        m_timebase_frequency = 0.0;

        for(auto& input_channel : getInputChannelProxies())
        {
            const auto timebase = input_channel->getTimeBase();
            m_timebase_frequency = std::max(m_timebase_frequency, timebase.m_frequency);
        }

        for(auto& output_channel : m_output_channels)
        {
            output_channel->setSimpleTimebase(m_timebase_frequency);
        }
    }

    uint64_t getTickAtOrAfter(double time, double frequency)
    {
        if (time == 0.0)
        {
            return 0;
        }
        return static_cast<uint64_t>(std::nextafter(std::nextafter(time, 0.0) * frequency, std::numeric_limits<double>::lowest())) + 1;
    }

    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        const auto channel_id = m_input_channel->getValue();
        auto channel_iterator = context.m_channel_iterators[channel_id];
        auto timebase = getInputChannelProxy(channel_id)->getTimeBase();

        uint64_t start_sample = getTickAtOrAfter(context.m_window.first, m_timebase_frequency);
        uint64_t end_sample = getTickAtOrAfter(context.m_window.second, m_timebase_frequency);

        uint64_t sample_index = 0;
        while ((start_sample + sample_index) < end_sample)
        {
            auto data = static_cast<const double*> (channel_iterator.data());
            auto result = std::minmax_element(data, data+m_dimension);
            if (m_min_channels.m_value_channel && (m_min_channels.m_value_channel->getUsedProperty()->getValue()))
            {
                addSample(host, m_min_channels.m_value_channel->getLocalId(), start_sample + sample_index, result.first, sizeof(double));
            }
            if (m_min_channels.m_bin_channel && (m_min_channels.m_bin_channel->getUsedProperty()->getValue()))
            {
                float offset = static_cast<float>(result.first - data);
                addSample(host, m_min_channels.m_bin_channel->getLocalId(), start_sample + sample_index, &offset, sizeof(float));
            }
            if (m_max_channels.m_value_channel && (m_max_channels.m_value_channel->getUsedProperty()->getValue()))
            {
                addSample(host, m_max_channels.m_value_channel->getLocalId(), start_sample + sample_index, result.second, sizeof(double));
            }
            if (m_max_channels.m_value_channel && (m_max_channels.m_bin_channel->getUsedProperty()->getValue()))
            {
                float offset = static_cast<float>(result.second - data);
                addSample(host, m_max_channels.m_bin_channel->getLocalId(), start_sample + sample_index, &offset, sizeof(float));
            }

            ++channel_iterator;
            ++sample_index;
        }
    }

private:
    std::shared_ptr<EditableChannelIDProperty> m_input_channel;
    std::shared_ptr<EditableStringProperty> m_enable_min;
    std::shared_ptr<EditableStringProperty> m_enable_max;

    double m_timebase_frequency;
    uint32_t m_dimension = 0;

    struct OutputChannelStruct
    {
        PluginChannelPtr m_value_channel;
        PluginChannelPtr m_bin_channel;
    };
    OutputChannelStruct m_min_channels;
    OutputChannelStruct m_max_channels;
};

class MyDemuxVectorPlugin : public SoftwareChannelPlugin<BinDetectorInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
    }

};

OXY_REGISTER_PLUGIN1("ODK_BIN_DETECTOR", PLUGIN_MANIFEST, MyDemuxVectorPlugin);

