// Copyright DEWETRON GmbH 2019-2021

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
        , m_current_values({ std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() })
    {
        // make property m_input_channels visible in the GUI
        m_input_channels->setVisiblity("PUBLIC");
    }

    /**
     * Return the information that is used to display the software channel in the calculation list in the GUI
     */
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Sum channels";
        telegram.m_service_name = "AddSyncAsync";
        telegram.m_display_group = "Basic Math";
        telegram.m_description = "Adds a channel that calculates the sum of two input channels.";
        telegram.m_analysis_capable = true;
        return telegram;
    }

    /**
     * This method is called when the user creates an instance of this calculation
     * In this case, copy the channel-ids from the selected channels and store them in m_input_channels
     */
    InitResult init(const InitParams& params) override
    {
        odk::ChannelIDList channel_ids;
        for (const auto& an_input_channel : params.m_input_channels)
        {
            channel_ids.m_values.push_back(an_input_channel.m_channel_id);
        }
        m_input_channels->setValue(channel_ids);
        return { true };
    }

    /**
     * This method is called when the configuration changes. Evaluate the input channels and update the output channel and return if the configuration is valid
     */
    bool update() override
    {
        std::string unit;
        double range_min = 0.0;
        double range_max = 0.0;
        double sample_rate_max = 0.0;
        for (const auto& input_channel : getInputChannelProxies())
        {
            const odk::Range input_range = input_channel->getRange();
            range_min = range_min + input_range.m_min;
            range_max = range_max + input_range.m_max;

            if (unit.empty())
            {
                // Store the unit of the first channel (ignore the unit of other channels)
                unit = input_channel->getUnit();
            }

            const odk::Scalar sample_rate = input_channel->getSampleRate();
            sample_rate_max = std::max(sample_rate_max, sample_rate.m_val);
        }

        if (sample_rate_max == 0.0)
        {
            // if no valid sample rate was set, assume 100 Hz
            sample_rate_max = 100.0;
        }

        // Configure the output channel (we only have one output, so the root channel is our output channel)
        auto channel = getRootChannel();
        channel->setRange(odk::Range(range_min, range_max, unit, unit))
            .setUnit(unit)
            .setSimpleTimebase(sample_rate_max)
            ;

        m_timebase_frequency = sample_rate_max;
        updateNeedsResampling();

        // Verify that all configured input channels can be used to compute an output
        const auto current_channel_ids = m_input_channels->getValue().m_values;
        bool is_valid = true;
        if (current_channel_ids.size() != 2)
        {
            is_valid = false;
        }
        else
        {
            std::size_t sync_input_cnt = 0;
            std::size_t async_input_cnt = 0;
            for (auto input_channel_id : current_channel_ids)
            {
                auto an_input_channel = getInputChannelProxy(input_channel_id);
                if (an_input_channel->updateDataFormat())
                {
                    const auto dataformat = an_input_channel->getDataFormat();

                    is_valid &= (dataformat.m_sample_value_type == odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR);

                    switch (dataformat.m_sample_occurrence)
                    {
                        case odk::ChannelDataformat::SampleOccurrence::SYNC:
                            ++sync_input_cnt;
                            break;

                        case odk::ChannelDataformat::SampleOccurrence::ASYNC:
                            ++async_input_cnt;
                            break;

                        default:
                            // Any channel that is neither SYNC nor ASYNC is currently not supported
                            is_valid = false;
                            break;
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
        }

        return is_valid;
    }

    void updateInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& channel_mapping) override
    {
        ODK_UNUSED(channel_mapping);

        //the channels have already been mapped by base-class
        //remove all channel_ids that are invalid (not done by base-class)
        const auto current_channel_ids(m_input_channels->getValue());
        odk::ChannelIDList channel_ids;
        for (auto input_channel_id : current_channel_ids.m_values)
        {
            if (input_channel_id != std::numeric_limits<uint64_t>::max())
            {
                channel_ids.m_values.push_back(input_channel_id);
            }
        }
        m_input_channels->setValue(channel_ids);
    }

    /**
     * Initialize the configuration and format of this calculation
     */
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

    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        auto sync_out_channel = getRootChannel();
        if (!sync_out_channel->getUsedProperty()->getValue())
        {
            return;
        }

        auto prepare_iterator = [this, &context](std::size_t index) -> std::pair<odk::framework::StreamIterator, double>
        {
            const std::uint64_t channel_id = m_input_channels->getValue().m_values.at(index);
            context.m_channel_iterators[channel_id].setSkipGaps(false);
            return std::make_pair(context.m_channel_iterators[channel_id], getInputChannelProxy(channel_id)->getTimeBase().m_frequency);
        };

        std::array<std::pair<odk::framework::StreamIterator, double>, 2> iterators = {
            prepare_iterator(0),
            prepare_iterator(1)
        };

        std::uint64_t start_sample = odk::convertTimeToTickAtOrAfter(context.m_window.first,  m_timebase_frequency);
        std::uint64_t end_sample =   odk::convertTimeToTickAtOrAfter(context.m_window.second, m_timebase_frequency);

        std::vector<double> samples(end_sample - start_sample);
        std::size_t output_sample_index = 0;

        if (m_resampling_enabled)
        {
            for (auto sample_index = start_sample; sample_index < end_sample; ++sample_index)
            {
                // Compute output time in seconds
                const double output_time = odk::convertTickToTime(sample_index, m_timebase_frequency);

                // Read all input channels up until output_time (channels with slower sample rate might reuse the old value when no newer sample is available)
                for (std::size_t channel_index = 0; channel_index < 2; ++channel_index)
                {
                    auto& iterator = iterators[channel_index].first;
                    const double timebase_frequency = iterators[channel_index].second;
                    while (iterator.valid() &&
                        iterator.timestamp() / timebase_frequency <= output_time)
                    {
                        m_current_values[channel_index] = iterator.value<double>();
                        ++iterator;
                    }
                }

                samples[output_sample_index++] = computeOutput();
            }

            // Read remaining samples to prevent missing samples in the next call of process
            for (std::size_t channel_index = 0; channel_index < 2; ++channel_index)
            {
                auto& iterator = iterators[channel_index].first;
                while (iterator.data())
                {
                    m_current_values[channel_index] = iterator.value<double>();
                    ++iterator;
                }
            }
        }
        else
        {
            // no resampling required, read every sample from both input channels and compute an output sample
            for (auto sample_index = start_sample; sample_index < end_sample; ++sample_index)
            {
                for (std::size_t channel_index = 0; channel_index < 2; ++channel_index)
                {
                    auto& iterator = iterators[channel_index].first;
                    m_current_values[channel_index] = iterator.value<double>();
                    ++iterator;
                }

                samples[output_sample_index++] = computeOutput();
            }
        }

        if (output_sample_index > 0)
        {
            // write "output_sample_index" samples to the output channel
            addSamples(host, sync_out_channel->getLocalId(), start_sample, samples.data(), sizeof(double) * output_sample_index);
        }
    }

    void initTimebases(odk::IfHost* host) override
    {
        ODK_UNUSED(host);
        updateNeedsResampling();
    }

private:
    /**
     * Perform the actual calculation with the current sample values
     */
    ODK_NODISCARD inline double computeOutput() const
    {
        return m_current_values[0] + m_current_values[1];
    }

    /**
     * If all channels are synchronous and have the same sample rate, we can directly sum the input values. Otherwise, we need to resample. 
     */
    void updateNeedsResampling()
    {
        for (auto& input_channel : getInputChannelProxies())
        {
            const auto sample_format = input_channel->getDataFormat();
            const auto timebase = input_channel->getTimeBase();

            if (sample_format.m_sample_occurrence != odk::ChannelDataformat::SampleOccurrence::SYNC ||
                timebase.m_frequency != m_timebase_frequency)
            {
                m_resampling_enabled = true;
                return;
            }
        }

        m_resampling_enabled = false;
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
