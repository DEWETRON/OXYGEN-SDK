// Copyright DEWETRON GmbH 2019

#include "odkfw_properties.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_utils.h"

#include <array>
#include <cmath>
#include <string.h>

#define DONT_ENFORCE_SYNC_AND_ASYNC

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_SUM_CHANNELS" version="1.0" uuid="D9C295C0-CBB9-4412-9B4A-0C5B1ACF3EB6">
  <Info name="Example Plugin: Sum channels">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that sums up the values of two input channels and writes it to the output channel</Description>
  </Info>
  <Host minimum_version="3.7"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SUM_CHANNELS/MyInputChannels</source><translation>Input Channels</translation></message>
    </context>
</TS>
)XML";

static const char* TRANSLATION_DE =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="de" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SUM_CHANNELS/MyInputChannels</source><translation>Eingangskan<byte value="xe4"/>le</translation></message>
    </context>
</TS>
)XML";

static const char* KEY_INPUT_CHANNELS = "ODK_SUM_CHANNELS/MyInputChannels";

using namespace odk::framework;

class MyExampleSoftwareChannelInstance : public SoftwareChannelInstance
{
public:

    MyExampleSoftwareChannelInstance()
        : m_input_channels(new EditableChannelIDListProperty())
    {
        m_input_channels->setVisiblity("PUBLIC");
    }

    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Sum channels";
        telegram.m_service_name = "AddSyncAsync";
        telegram.m_display_group = "Basic Math";
        telegram.m_description = "Adds a channel that calculates the sum of two input channels.";
        return telegram;
    }

    void init(const std::vector<InputChannel::InputChannelData>& input_channel_data) override
    {
        std::vector<std::uint64_t> channel_ids;
        for (const auto an_input_channel : input_channel_data)
        {
            channel_ids.push_back(an_input_channel.m_channel_id);
        }
        m_input_channels->setValue(odk::ChannelIDList(channel_ids));
    }

    void updatePropertyTypes(const PluginChannelPtr& output_channel) override
    {
        ODK_UNUSED(output_channel);
    }

    void updateStaticPropertyConstraints(const PluginChannelPtr& channel) override
    {
        ODK_UNUSED(channel);
    }

    bool update() override
    {
        std::string unit;
        double range_min = 0.0;
        double range_max = 0.0;
        double sample_rate_max = 0.0;
        for(const auto& input_channel : getInputChannelProxies())
        {
            const auto& input_range = input_channel->getRange();
            range_min = range_min + input_range.m_min;
            range_max = range_max + input_range.m_max;
            unit = input_channel->getUnit();

            const auto sampe_rate = input_channel->getSampleRate();
            sample_rate_max = std::max(sample_rate_max, sampe_rate.m_val);
        }

        if (sample_rate_max == 0.0)
        {
            sample_rate_max = 100.0;
        }

        auto channel = getOutputChannels().at(0);
        channel->setRange({range_min, range_max, unit, unit})
            .setUnit(unit)
            .setSimpleTimebase(sample_rate_max)
            ;

        m_timebase_frequency = sample_rate_max;
        updateNeedsResampling();

        const auto current_channel_ids(m_input_channels->getValue().m_values);
        auto is_valid(current_channel_ids.size() == 2);

        auto all_input_channels(getInputChannelProxies());

        int sync_input_cnt = 0;
        int async_input_cnt = 0;

        for (auto input_channel_id : current_channel_ids)
        {
            if (is_valid)
            {
                auto an_input_channel = getInputChannelProxy(input_channel_id);
                if (an_input_channel->updateDataFormat())
                {
                    const auto dataformat = an_input_channel->getDataFormat();

                    is_valid &= (dataformat.m_sample_value_type ==
                        odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR);

                    switch (dataformat.m_sample_occurrence)
                    {
                        case odk::ChannelDataformat::SampleOccurrence::SYNC:
                            ++sync_input_cnt;
                            break;

                        case odk::ChannelDataformat::SampleOccurrence::ASYNC:
                            ++async_input_cnt;
                            break;

                        default:
                            is_valid = false;
                    }
                }
            }

            if (!is_valid)
            {
                break;
            }
        }

#ifndef DONT_ENFORCE_SYNC_AND_ASYNC
        is_valid &= (sync_input_cnt == 1);
        is_valid &= (async_input_cnt == 1);
#endif
        return is_valid;
    }

    void updateInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& channel_mapping) override
    {
        ODK_UNUSED(channel_mapping);

        //the channels have already been mapped by base-class
        //remove all channel_ids that are invalid (not done by base-class)
        const auto current_channel_ids(m_input_channels->getValue().m_values);
        std::vector<std::uint64_t> channel_ids;
        for (auto input_channel_id : current_channel_ids)
        {
            if (input_channel_id != -1)
            {
                channel_ids.push_back(input_channel_id);
            }
        }
        m_input_channels->setValue(odk::ChannelIDList(channel_ids));
    }

    void create(odk::IfHost* host) override
    {
        ODK_UNUSED(host);

        getRootChannel()->setDefaultName("SumChannel")
            .setSampleFormat(
                odk::ChannelDataformat::SampleOccurrence::SYNC,
                odk::ChannelDataformat::SampleFormat::DOUBLE,
                1)
            .setDeletable(true)
            .addProperty(KEY_INPUT_CHANNELS, m_input_channels)
            ;
    }

    bool configure(
        const odk::UpdateChannelsTelegram& request,
        std::map<std::uint32_t, std::uint32_t>& channel_id_map) override
    {
        configureFromTelegram(request, channel_id_map);
        return true;
    }

    void prepareProcessing(odk::IfHost *host) override
    {
        ODK_UNUSED(host);

        m_current_values.fill(std::numeric_limits<double>::quiet_NaN());
    }

    std::uint64_t convertTimeToTickAtOrAfter(double time, double frequency)
    {
        if (time == 0.0)
        {
            return 0;
        }
        return static_cast<std::uint64_t>(std::nextafter(std::nextafter(time, 0.0) * frequency, std::numeric_limits<double>::lowest())) + 1;
    }

    double convertTickToTime(std::uint64_t tick, double frequency)
    {
        return std::nextafter(tick / frequency, std::numeric_limits<double>::max());
    }


    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        std::array<std::pair<odk::framework::StreamIterator, odk::Timebase>, 2> iterators;
        {
            const auto channel_id = m_input_channels->getValue().m_values.at(0);
            iterators[0] = { context.m_channel_iterators[channel_id], getInputChannelProxy(channel_id)->getTimeBase() };
        }
        {
            const auto channel_id = m_input_channels->getValue().m_values.at(1);
            iterators[1] = { context.m_channel_iterators[channel_id], getInputChannelProxy(channel_id)->getTimeBase() };
        }

        std::uint64_t start_sample = convertTimeToTickAtOrAfter(context.m_window.first, m_timebase_frequency);
        std::uint64_t end_sample = convertTimeToTickAtOrAfter(context.m_window.second, m_timebase_frequency);

        auto sync_out_channel = getRootChannel();
        if (sync_out_channel->getUsedProperty()->getValue())
        {
            std::uint64_t sample_index = 0;

            std::vector<double> samples(end_sample - start_sample);

            if (m_resampling_enabled)
            {
                while ((start_sample + sample_index) < end_sample)
                {
                    double output_time = convertTickToTime((start_sample + sample_index), m_timebase_frequency);
                    int channel_index = 0;
                    for (auto& channel_iterator : iterators)
                    {
                        auto& iterator = channel_iterator.first;
                        const auto& timebase = channel_iterator.second;
                        while (iterator.valid() &&
                           iterator.timestamp() / timebase.m_frequency <= output_time)
                        {
                            m_current_values[channel_index] = iterator.value<double>();
                            ++iterator;
                        }
                        ++channel_index;
                    }
                    samples[sample_index] = m_current_values[0] + m_current_values[1];
                    ++sample_index;
                }

                int channel_index = 0;
                for (auto& channel_iterator : iterators)
                {
                    auto& iterator = channel_iterator.first;
                    while (iterator.valid())
                    {
                        m_current_values[channel_index] = iterator.value<double>();
                        ++iterator;
                    }
                    ++channel_index;
                }
            }
            else
            {
                while ((start_sample + sample_index) < end_sample)
                {
                    samples[sample_index] = iterators[0].first.value<double>() + iterators[1].first.value<double>();
                    ++iterators[0].first;
                    ++iterators[1].first;
                    ++sample_index;
                }
            }

            if (sample_index > 0)
            {
                samples.resize(sample_index);
                addSamples(host, sync_out_channel->getLocalId(), start_sample, samples.data(), sizeof(double) * samples.size());
            }
        }

    }

private:
    void updateNeedsResampling()
    {
        m_resampling_enabled = false;

        for (auto& input_channel : getInputChannelProxies())
        {
            const auto sample_format = input_channel->getDataFormat();
            const auto timebase = input_channel->getTimeBase();

            if (!m_resampling_enabled)
            {
                m_resampling_enabled = sample_format.m_sample_occurrence != odk::ChannelDataformat::SampleOccurrence::SYNC
                    || timebase.m_frequency != m_timebase_frequency;

                if (m_resampling_enabled)
                {
                    break;
                }
            }
        }
    }

    // move to base?
    std::shared_ptr<EditableChannelIDListProperty> m_input_channels;
    std::array<double, 2> m_current_values;
    bool m_resampling_enabled = false;
    double m_timebase_frequency = 0.0;
};

class MyExamplePlugin : public SoftwareChannelPlugin<MyExampleSoftwareChannelInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
        addTranslation(TRANSLATION_DE);
    }

};

OXY_REGISTER_PLUGIN1("ODK_SUM_CHANNELS", PLUGIN_MANIFEST, MyExamplePlugin);

