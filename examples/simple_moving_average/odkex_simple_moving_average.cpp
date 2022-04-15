// Copyright DEWETRON GmbH 2022

#include "odkfw_properties.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_utils.h"

#include <numeric>

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_SIMPLE_MOVING_AVERAGE" version="1.0" uuid="E9698711-6DC4-4391-8DD0-F3455D64AA98">
  <Info name="Example Plugin: Simple Moving Average">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that computes the moving average in a moving, configurable window</Description>
  </Info>
  <Host minimum_version="3.7"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SIMPLE_MOVING_AVERAGE/InputChannel</source><translation>Input Channel</translation></message>
        <message><source>ODK_SIMPLE_MOVING_AVERAGE/WindowSize</source><translation>Window Size</translation></message>
    </context>
</TS>
)XML";

// German translation with special characters encoded as utf-8 raw codes
static const char* TRANSLATION_DE =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="de" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SIMPLE_MOVING_AVERAGE/InputChannel</source><translation>Eingangskanal</translation></message>
        <message><source>ODK_SIMPLE_MOVING_AVERAGE/WindowSize</source><translation>Fenstergr<byte value="xf6"/><byte value="xdf"/>e</translation></message>
    </context>
</TS>
)XML";

static const char* KEY_INPUT_CHANNEL = "ODK_SIMPLE_MOVING_AVERAGE/InputChannel";
static const char* KEY_WINDOW_SIZE = "ODK_SIMPLE_MOVING_AVERAGE/WindowSize";

using namespace odk::framework;

class SmaExampleSoftwareChannelInstance : public SoftwareChannelInstance
{
public:

    SmaExampleSoftwareChannelInstance()
        : m_input_channel(std::make_shared<EditableChannelIDProperty>())
        , m_window_size(std::make_shared<EditableUnsignedProperty>(3, 1, 99))
    {
        // make properties visible in the GUI
        m_input_channel->setVisiblity("PUBLIC");
        m_window_size->setVisiblity("PUBLIC");
    }

    /**
     * Return the information that is used to display the software channel in the calculation list in the GUI
     */
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Simple moving average";
        telegram.m_service_name = "SyncSma";
        telegram.m_display_group = "Basic Math";
        telegram.m_description = "Computes the moving average of a SYNC input channel.";
        telegram.m_analysis_capable = true;
        return telegram;
    }

    /**
     * This method is called when the user creates an instance of this calculation
     * In this case, copy the first channel-id from the selected channels and store it in m_input_channel (or do nothing if no channel is selected)
     */
    InitResult init(const InitParams& params) override
    {
        if (params.m_input_channels.size() >= 1)
        {
            m_input_channel->setValue(params.m_input_channels.front().m_channel_id);
        }

        InitResult result = { true };
        result.m_channel_list_action = InitResult::ChannelListAction::SHOW_DETAILS_OF_FIRST_CHANNEL;
        return result;
    }

    /**
     * This method is called when the configuration changes. Evaluate the input channels and update the output channel and return if the configuration is valid
     */
    bool update() override
    {
        std::string unit;
        double range_min = 0.0;
        double range_max = 0.0;
        double sample_rate = 0.0;
        std::string name;
        const auto& input_channels = getInputChannelProxies();
        if (input_channels.size() == 1)
        {
            const auto& input_channel = input_channels.front();
            const odk::Range input_range = input_channel->getRange();
            range_min = input_range.m_min;
            range_max = input_range.m_max;

            unit = input_channel->getUnit();
            sample_rate = input_channel->getSampleRate().m_val;
            name = input_channel->getLongName();
        }

        // if no valid sample rate was set, assume 100 Hz
        if (sample_rate == 0.0)
        {
            sample_rate = 100.0;
        }

        // Generate the default channel name
        name += "_MA" + std::to_string(m_window_size->getValue());

        // Configure the output channel (we only have one output, so the root channel is our output channel)
        auto channel = getRootChannel();
        channel->setDefaultName(name)
            .setRange(odk::Range(range_min, range_max, unit, unit))
            .setUnit(unit)
            .setSimpleTimebase(sample_rate)
            ;

        m_timebase_frequency = sample_rate;

        // Verify that the configured input channel can be used to compute an output
        if (!m_input_channel->isValid())
        {
            return false;
        }

        auto input_channel = getInputChannelProxy(m_input_channel->getValue());
        if (input_channel && input_channel->updateDataFormat())
        {
            const auto dataformat = input_channel->getDataFormat();

            if (dataformat.m_sample_value_type == odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR &&
                dataformat.m_sample_occurrence == odk::ChannelDataformat::SampleOccurrence::SYNC)
            {
                return true;
            }
        }

        return false;
    }

    /**
     * Initialize the configuration and format of this calculation
     */
    void create(odk::IfHost* host) override
    {
        ODK_UNUSED(host);

        getRootChannel()->setSampleFormat(
                odk::ChannelDataformat::SampleOccurrence::SYNC,
                odk::ChannelDataformat::SampleFormat::DOUBLE,
                1)
            .setDeletable(true)
            .addProperty(KEY_INPUT_CHANNEL, m_input_channel)
            .addProperty(KEY_WINDOW_SIZE, m_window_size)
            ;
    }

    bool configure(
        const odk::UpdateChannelsTelegram& request,
        std::map<std::uint32_t, std::uint32_t>& channel_id_map) override
    {
        configureFromTelegram(request, channel_id_map);
        return true;
    }

    /**
     * This method is called before the first sample is processed. Reset the input buffers here.
     */
    void prepareProcessing(odk::IfHost *host) override
    {
        ODK_UNUSED(host);

        m_input_buffer.clear();
        m_input_buffer.reserve(m_window_size->getValue());
        m_next_output_tick = 0; // set to uninitialized state
    }

    /**
     * This method is called for each data block/window. Read all samples and compute output values
     */
    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        // make sure our output channel exists and is turned on (used)
        auto sync_out_channel = getRootChannel();
        if (!sync_out_channel->getUsedProperty()->getValue())
        {
            return;
        }

        // read calculation settings
        const unsigned int window_size = m_window_size->getValue();

        ODK_ASSERT_EQUAL(context.m_channel_iterators.size(), 1);
        auto& iterator = context.m_channel_iterators.begin()->second;

        // We want a continuous stream even during gaps/pauses (in this case the iterator value is NaN)
        iterator.setSkipGaps(false);

        // Convert the time window to ticks in the timebase of the channel
        const std::uint64_t start_sample = odk::convertTimeToTickAtOrAfter(context.m_window.first, m_timebase_frequency);
        const std::uint64_t end_sample = odk::convertTimeToTickAtOrAfter(context.m_window.second, m_timebase_frequency);

        // Prepare output buffer (it can be a local variable since we emit all values to the output channel within one process() call)
        std::vector<double> output_buffer;
        output_buffer.reserve(end_sample - start_sample);

        // Read all samples from the channel iterator
        for (auto sample_index = start_sample; sample_index < end_sample; ++sample_index)
        {
            const double sample_value = iterator.value<double>();
            ++iterator;

            // add sample to input buffer (note that when reading across a gap, invalid (NaN) values are added and the output value will automatically be NaN when invalid values are present)
            m_input_buffer.push_back(sample_value);
            
            // check if we can compute an output
            if (m_input_buffer.size() == window_size)
            {
                // compute average over all samples in the window
                const double average = std::accumulate(m_input_buffer.begin(), m_input_buffer.end(), 0.0) / window_size;
                output_buffer.push_back(average);

                // erase first sample in input buffer
                m_input_buffer.erase(m_input_buffer.begin());
            }
        }

        // check if output_tick is uninitialized and compute the initial value
        if (m_next_output_tick == 0)
        {
            // The first output sample is emitted in the middle of the first window
            // Note that for SYNC output channels, it is allowed to emit the first output sample later than the first input sample (but no more gaps are allowed after the first written sample)
            m_next_output_tick = start_sample + window_size / 2;
        }

        // Write several consecutive output samples to the output channel
        if (!output_buffer.empty())
        {
            addSamples(host, sync_out_channel->getLocalId(), m_next_output_tick, output_buffer.data(), sizeof(double) * output_buffer.size());
            // advance the output tick for the next call of process()
            m_next_output_tick += output_buffer.size();
        }
    }

private:
    std::shared_ptr<EditableChannelIDProperty> m_input_channel;
    std::shared_ptr<EditableUnsignedProperty> m_window_size;
    std::vector<double> m_input_buffer;
    std::uint64_t m_next_output_tick = 0;
    double m_timebase_frequency = 0.0;
};

class SmaExamplePlugin : public SoftwareChannelPlugin<SmaExampleSoftwareChannelInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
        addTranslation(TRANSLATION_DE);
    }
};

OXY_REGISTER_PLUGIN1("ODK_SIMPLE_MOVING_AVERAGE", PLUGIN_MANIFEST, SmaExamplePlugin);
