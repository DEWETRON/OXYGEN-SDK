// Copyright DEWETRON GmbH 2022

#include "odkfw_properties.h"
#include "odkfw_resampler.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_utils.h"

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_SYNC_RESAMPLE_SOURCE" version="1.0" uuid="295f85c6-070c-43a9-bde5-adc3c472451e">
  <Info name="Example Plugin: Sync Resample Source">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that resamples input samples that do not match the nominal output sample rate</Description>
  </Info>
  <Host minimum_version="3.7"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_SYNC_RESAMPLE_SOURCE/TrueSampleRate</source><translation>True Sample Rate</translation></message>
    </context>
</TS>
)XML";

/**
 * Define the names for config keys
 */
static const char* KEY_TRUE_SAMPLE_RATE = "ODK_SYNC_RESAMPLE_SOURCE/TrueSampleRate";

class SyncResampleSourceInstance : public odk::framework::SoftwareChannelInstance
{
public:

    SyncResampleSourceInstance()
    {
        m_true_sample_rate = std::make_shared<odk::framework::EditableScalarProperty>(10500, "Hz");
        m_true_sample_rate->setVisiblity("PUBLIC");
        m_true_sample_rate->setMinMaxConstraint(100, 100000);
    }

    /**
     * Return the information that is used to display the software channel in the calculation list in the GUI
     */
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Sync Resample Source";
        telegram.m_service_name = "SyncResampleSource";
        telegram.m_display_group = "Data Sources";
        telegram.m_description = "Plugin that resamples input samples that do not match the nominal output sample rate";
        return telegram;
    }

    /**
     * This method is called when the user creates an instance of this calculation
     * In this case, copy the channel-ids from the selected channels and store them in m_input_channels
     */
    InitResult init(const InitParams& params) override
    {
        return { true };
    }

    void initTimebases(odk::IfHost* host) override
    {
        auto sr = getRootChannel()->getSamplerateProperty();
        m_timebase_frequency = sr->getValue().m_val;
        m_output_channels[0]->setSimpleTimebase(m_timebase_frequency);
        m_resampler.setNominalSampleRate(m_timebase_frequency);
    }

    /**
     * This method is called when the configuration changes. Evaluate the input channels and update the output channel and return if the configuration is valid
     */
    bool update() override
    {
        return true;
    }

    void updateInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& channel_mapping) override
    {
        ODK_UNUSED(channel_mapping);
    }

    /**
     * Initialize the configuration and format of this calculation
     */
    void create(odk::IfHost* host) override
    {
        ODK_UNUSED(host);

        getRootChannel()->setDefaultName("ResampledChannel")
            .setSampleFormat(
                odk::ChannelDataformat::SampleOccurrence::SYNC,
                odk::ChannelDataformat::SampleFormat::DOUBLE,
                1)
            .setDeletable(true)
            .setSamplerate(odk::Scalar(10000, "Hz"))
            .addProperty(KEY_TRUE_SAMPLE_RATE, m_true_sample_rate);
            ;
        getRootChannel()->getSamplerateProperty()->setMinMaxConstraint(100, 100000);
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
        m_resampler.reset();
        m_t_prev = 0;
    }

    std::vector<double> generateSignalRamp(std::size_t num_samples)
    {
        std::vector<double> samples(num_samples);
        for (std::size_t n = 0; n < num_samples; ++n)
        {
            if (n < num_samples / 4)
            {
                samples[n] = n;
            }
            else if (n < 3 * num_samples / 4)
            {
                samples[n] = static_cast<double>(num_samples / 2.0) - n;
            }
            else
            {
                samples[n] = n - static_cast<double>(num_samples);
            }
        }
        return samples;
    }

    void process(ProcessingContext& context, odk::IfHost *host) override
    {
        auto out_channel = getRootChannel();

        if (!out_channel->getUsedProperty()->getValue())
        {
            // Do not output samples when the channel is not used
            return;
        }

        // expected output timestamp
        const auto& master_timestamp = context.m_master_timestamp;
        const std::size_t block_size = 1000;
        
        // only generate samples up until the master timestamp (not into the future)
        if (master_timestamp.m_ticks > (m_resampler.getSampleCount() + block_size) / m_timebase_frequency * master_timestamp.m_frequency)
        {
            auto samples = generateSignalRamp(block_size);
            double t = m_t_prev + samples.size() / m_true_sample_rate->getValue().m_val;
            m_resampler.addSamples(host, out_channel->getLocalId(), t, samples.data(), samples.size());
            m_t_prev = t;
        }
    }

private:
    std::shared_ptr<odk::framework::EditableScalarProperty> m_true_sample_rate;
    double m_timebase_frequency = 0.0;
    double m_t_prev = 0;
    odk::framework::Resampler m_resampler;
};

class SyncResampleSourcePlugin : public odk::framework::SoftwareChannelPlugin<SyncResampleSourceInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
    }
};

OXY_REGISTER_PLUGIN1("ODK_SYNC_RESAMPLE_SOURCE", PLUGIN_MANIFEST, SyncResampleSourcePlugin);
