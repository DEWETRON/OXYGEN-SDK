// Copyright DEWETRON GmbH 2020

#include "odkfw_export_plugin.h"
#include "odkfw_properties.h"
#include "odkfw_property_list_utils.h"
#include "odkbase_message_return_value_holder.h"
#include "odkapi_utils.h"

#include "wav_writer.h"

#include "qml.rcc.h"

#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include <chrono>
#include <ios>
#include <thread>

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_WAV_EXPORT" version="1.0" uuid="60e4627b-8964-4259-a9e0-54d38847ae68">
  <Info name="Example Plugin: WAV export">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin implementing WAV file export.</Description>
  </Info>
  <Host minimum_version="5.3"/>
  <UsesUIExtensions/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ODK_WAV_EXPORT/</name>
    </context>
</TS>
)XML";


using namespace odk::framework;

class WavExport : public ExportInstance
{
public:

    WavExport()
    {
    }

    static odk::RegisterExport getExportInfo()
    {
        odk::RegisterExport telegram;
        telegram.m_format_name = "WAV";
        telegram.m_format_id = "WAV";
        telegram.m_file_extension = "wav";
        telegram.m_ui_item_small = "ExportSettings";
        telegram.m_ui_item_full = "ExportSettings";
        return telegram;
    }

    void validate(const ValidationContext& context, odk::ValidateExportResponse& response) const
    {
        bool no_export_possible = true;
        for (auto& channel_id : context.m_properties.m_channels)
        {
            auto channel = context.m_channels.at(channel_id);
            odk::ChannelDataformat::SampleValueType value_type = channel->getDataFormat().m_sample_value_type;
            odk::ChannelDataformat::SampleOccurrence occurrence_type = channel->getDataFormat().m_sample_occurrence;
            if (value_type != odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR)
            {
                response.m_channel_warnings.emplace_back(channel_id, odk::error_codes::UNSUPPORTED_CHANNEL_TYPE, "Only scalar channels supported.");
            }
            else if(occurrence_type != odk::ChannelDataformat::SampleOccurrence::SYNC)
            {
                response.m_channel_warnings.emplace_back(channel_id, odk::error_codes::UNSUPPORTED_CHANNEL_TYPE, "Only sync channels supported.");
            }
            else
            {
                no_export_possible = false;
            }
        }

        response.m_success = !no_export_possible;
    }

    bool exportData(const ProcessingContext& context)
    {
        WavFormatTag type = WavFormatTag::WAV_FORMAT_FLOAT;
        std::size_t sample_size = sizeof(float)*8;

        if(context.m_properties.m_custom_properties.getString("Format") == "PCM")
        {
            type = WavFormatTag::WAV_FORMAT_PCM;
            sample_size = 16;
        }

        if(!context.m_properties.m_channels.empty())
        {
            auto first_channel = context.m_channels.at(context.m_properties.m_channels.front());

            const double sample_rate = first_channel->getSampleRate().m_val;
            const std::size_t num_channels = context.m_properties.m_channels.size();
            const auto first_interval = context.m_properties.m_export_intervals.at(0);
            const double interval_length = first_interval.m_end - first_interval.m_begin;

            const std::size_t num_samples = static_cast<std::size_t>(interval_length * sample_rate);
            std::map<uint64_t, double> scaling_factors;
            for(const auto& channel : context.m_channels)
            {
                auto range = channel.second->getRange();
                scaling_factors[channel.first] = 1 / std::max(range.m_max, range.m_min);
            }

            try
            {
                WavWriter writer(context.m_properties.m_filename.c_str());
                writer.writeHeader(type, sample_size, static_cast<std::uint32_t>(sample_rate), num_channels, num_samples);

                for(int i = 0; i < num_samples; ++i)
                {
                    if(i % 1000 == 0)
                    {
                        notifyProgress(100 * i / num_samples);
                    }

                    for(const auto& channel : context.m_channels)
                    {
                        auto& iterator = *context.m_channel_iterators.at(channel.first);

                        if(iterator.valid())
                        {
                            float value = static_cast<float>(iterator.value<double>() * scaling_factors[channel.first]);
                            if(type == WavFormatTag::WAV_FORMAT_FLOAT)
                            {
                                writer.appendSamples(&value, sizeof(float));
                            }
                            else if(type == WavFormatTag::WAV_FORMAT_PCM)
                            {
                                int16_t pcm_value = static_cast<int16_t>(value * 32768);
                                writer.appendSamples(&pcm_value, sizeof(int16_t));
                            }

                            ++iterator;
                        }
                    }
                }
                return true;
            }
            catch (const std::ios_base::failure&)
            {
                return false;
            }
        }
        return false;
    }

    void cancel()
    {

    }

private:

};

class WavExportPlugin : public ExportPlugin<WavExport>
{
public:
    WavExportPlugin()
    {
    }

    void registerResources() final
    {
        addTranslation(TRANSLATION_EN);
        addQtResources(plugin_resources::QML_RCC_data, plugin_resources::QML_RCC_size);
    }
};

OXY_REGISTER_PLUGIN1("ODK_WAV_EXPORT", PLUGIN_MANIFEST, WavExportPlugin);
