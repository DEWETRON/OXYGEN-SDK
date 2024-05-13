// Copyright DEWETRON GmbH 2019-2021

#include "odkfw_properties.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_utils.h"

#include <array>
#include <cmath>
#include <string.h>

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_SAMPLE_INTERPOLATOR" version="1.0" uuid="8A08FFE5-2E71-4A14-8BF5-334873504A8A">
  <Info name="Example Plugin: Sample Interpolator">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that reads a SYNC or ASYNC channel and outputing an interpolated version of it</Description>
  </Info>
  <Host minimum_version="3.7"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SAMPLE_INTERPOLATOR/InputChannel</source><translation>Input channel</translation></message>
        <message><source>ODK_SAMPLE_INTERPOLATOR/UpsampleFactor</source><translation>Upsample factor</translation></message>
    </context>
</TS>
)XML";

static const char* TRANSLATION_DE =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="de" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SAMPLE_INTERPOLATOR/InputChannel</source><translation>Eingangskanal</translation></message>
        <message><source>ODK_SAMPLE_INTERPOLATOR/UpsampleFactor</source><translation>Ueberabtastungsfaktor</translation></message>
    </context>
</TS>
)XML";

/**
 * Define the names for config keys
 */
static const char* KEY_INPUT_CHANNEL = "ODK_SAMPLE_INTERPOLATOR/InputChannel";
static const char* KEY_UPSAMPLE_FACTOR = "ODK_SAMPLE_INTERPOLATOR/UpsampleFactor";

class SampleInterpolatorChannelInstance : public odk::framework::SoftwareChannelInstance
{
public:

    SampleInterpolatorChannelInstance()
        : m_input_channel(std::make_shared<odk::framework::EditableChannelIDProperty>())
        , m_upsample_factor(std::make_shared<odk::framework::EditableUnsignedProperty>(1, 1, 100)) // Set to 1 initially, allow values from 1 to 100
    {
        // make property m_input_channels visible in the GUI
        m_input_channel->setVisiblity("PUBLIC");
        m_upsample_factor->setVisiblity("PUBLIC");
    }

    /**
     * Return the information that is used to display the software channel in the calculation list in the GUI
     */
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Sample Interpolator";
        telegram.m_service_name = "SampleInterpolator";
        telegram.m_display_group = "Basic Math";
        telegram.m_description = "Read a scalar channel and outputs an upsampled/interpolated version of it.";
        telegram.m_analysis_capable = true;
        return telegram;
    }

    /**
     * This method is called when the user creates an instance of this calculation
     * In this case, copy the channel-ids from the selected channels and store them in m_input_channels
     */
    InitResult init(const InitParams& params) override
    {
        if (params.m_input_channels.size() == 1)
        {
            m_input_channel->setValue(params.m_input_channels.front().m_channel_id);
        }
        return { true };
    }

    void initTimebases(odk::IfHost*) override
    {
        m_timebase_frequency = 0.0;

        const auto upsample_factor = m_upsample_factor->getValue();

        const auto channel_id = m_input_channel->getValue();
        if (auto input_channel = getInputChannelProxy(channel_id))
        {
            const auto timebase = input_channel->getTimeBase();
            m_timebase_frequency = std::max(m_timebase_frequency, timebase.m_frequency);
        }

        // The output channel has a higher timebase frequency due to upsampling
        m_output_channels[0]->setSimpleTimebase(m_timebase_frequency * upsample_factor);
    }

    /**
     * This method is called when the configuration changes. Evaluate the input channels and update the output channel and return if the configuration is valid
     */
    bool update() override
    {
        auto input_channels = getInputChannelProxies();
        if (input_channels.size() != 1)
        {
            return false;
        }

        const auto& input_channel = input_channels.front();
        const std::string unit = input_channel->getUnit();
        const odk::Range input_range = input_channel->getRange();
        const odk::Scalar sample_rate = input_channel->getSampleRate();

        // Configure the output channel (we only have one output, so the root channel is our output channel)
        auto channel = getRootChannel();
        channel->setRange(input_range)
            .setDefaultName(input_channel->getName() + "_Upsampled")
            .setUnit(unit)
            ;

        const auto dataformat = input_channel->getDataFormat();
        if (dataformat.m_sample_occurrence == odk::ChannelDataformat::SampleOccurrence::SYNC)
        {
            m_is_sync = true;
        }
        else if (dataformat.m_sample_occurrence == odk::ChannelDataformat::SampleOccurrence::ASYNC)
        {
            m_is_sync = false;
        }
        else
        {
            return false;
        }

        if (dataformat.m_sample_dimension != 1)
        {
            return false;
        }

        channel->setSampleFormat(dataformat.m_sample_occurrence,
            odk::ChannelDataformat::SampleFormat::DOUBLE,
            1);

        return true;
    }

    void updateInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& channel_mapping) override
    {
        ODK_UNUSED(channel_mapping);

        //the channels have already been mapped by base-class
        //remove all channel_ids that are invalid (not done by base-class)
    }

    /**
     * Initialize the configuration and format of this calculation
     */
    void create(odk::IfHost* host) override
    {
        ODK_UNUSED(host);

        getRootChannel()->setDefaultName("InterpolatedChannel")
            .setSampleFormat(
                odk::ChannelDataformat::SampleOccurrence::ASYNC,
                odk::ChannelDataformat::SampleFormat::DOUBLE,
                1)
            .setDeletable(true)
            .addProperty(KEY_INPUT_CHANNEL, m_input_channel)
            .addProperty(KEY_UPSAMPLE_FACTOR, m_upsample_factor)
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
        m_last_timestamp = 0;
        m_last_sample = 0;
        m_has_last = false;
    }

    /**
     * Linear Interpolation function
     */
    static double lerp(double a, double b, double t)
    {
        return a + (b - a) * t;
    }

    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        auto out_channel = getRootChannel();

        if (!out_channel->getUsedProperty()->getValue())
        {
            // Do not output samples when the channel is not used
            return;
        }

        // sample timestamps of the input channel
        const std::uint64_t start_sample = odk::convertTimeToTickAtOrAfter(context.m_window.first, m_timebase_frequency);
        const std::uint64_t end_sample =   odk::convertTimeToTickAtOrAfter(context.m_window.second, m_timebase_frequency);

        const std::uint64_t channel_id = m_input_channel->getValue();
        odk::framework::StreamIterator& iterator = context.m_channel_iterators[channel_id];
        iterator.setSkipGaps(false);

        const auto upsample_factor = m_upsample_factor->getValue();

        if (m_is_sync)
        {
            // Process a sync channel: Store the samples in a buffer and output them in one batch
            const std::size_t num_output_samples = end_sample - start_sample;
            std::vector<double> samples(num_output_samples * upsample_factor);
            std::size_t output_sample_index = 0;
            uint64_t output_start_sample = start_sample * upsample_factor;

            if (m_has_last)
            {
                output_start_sample = m_last_timestamp * upsample_factor;
            }

            for (auto sample_index = start_sample; sample_index < end_sample; ++sample_index)
            {
                const double current_value = iterator.value<double>();
                ++iterator;

                if (upsample_factor == 1)
                {
                    // no upsampling, just write the value to the output
                    samples[output_sample_index++] = current_value;
                }
                else
                {
                    // interpolate as soon as we have a previous sample
                    if (m_has_last)
                    {
                        for (unsigned int n = 0; n < upsample_factor; ++n)
                        {
                            double t = static_cast<double>(n) / upsample_factor;
                            samples[output_sample_index++] = lerp(m_last_sample, current_value, t);
                        }
                    }
                    m_last_timestamp = sample_index;
                    m_last_sample = current_value;
                    m_has_last = true;
                }
            }

            if (output_sample_index > 0)
            {
                // write "output_sample_index" samples to the output channel
                addSamples(host, out_channel->getLocalId(), output_start_sample, samples.data(), sizeof(double) * output_sample_index);
            }
        }
        else
        {
            // Process an async channel
            while (iterator.valid() && iterator.timestamp() < end_sample)
            {
                const uint64_t timestamp = iterator.timestamp();
                const double current_value = iterator.value<double>();
                ++iterator;

                if (upsample_factor == 1)
                {
                    // write a single async sample
                    addSample(host, out_channel->getLocalId(), timestamp, current_value);
                }
                else
                {
                    // interpolate as soon as we have a previous sample
                    if (m_has_last)
                    {
                        for (unsigned int n = 0; n < upsample_factor; ++n)
                        {
                            double t = static_cast<double>(n) / upsample_factor;
                            double value = lerp(m_last_sample, current_value, t);
                            uint64_t time = static_cast<uint64_t>(lerp(static_cast<double>(m_last_timestamp * upsample_factor),
                                static_cast<double>(timestamp * upsample_factor), t));
                            addSample(host, out_channel->getLocalId(), time, value);
                        }
                    }
                    m_last_timestamp = timestamp;
                    m_last_sample = current_value;
                    m_has_last = true;
                }
            }
        }

    }

private:
    std::shared_ptr<odk::framework::EditableChannelIDProperty> m_input_channel;
    std::shared_ptr<odk::framework::EditableUnsignedProperty> m_upsample_factor;
    double m_timebase_frequency = 0.0;
    bool m_is_sync = true;
    uint64_t m_last_timestamp = 0;
    double m_last_sample = 0;
    bool m_has_last = false;
};

class SampleInterpolatorPlugin : public odk::framework::SoftwareChannelPlugin<SampleInterpolatorChannelInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
        addTranslation(TRANSLATION_DE);
    }
};

OXY_REGISTER_PLUGIN1("ODK_SAMPLE_INTERPOLATOR", PLUGIN_MANIFEST, SampleInterpolatorPlugin);
