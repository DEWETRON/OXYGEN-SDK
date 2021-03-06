// Copyright DEWETRON GmbH 2020

#include "odkfw_software_channel_plugin.h"

// Manifest constains necessary metadata for oxygen plugins
//   OxygenPlugin.name: unique plugin identifier; please use your (company) name to avoid name conflicts. This name is also used as a prefix in all custom config item keys.
//   OxygenPlugin.uuid: unique number (generated by a GUID/UUID generator tool) that stored in configuration files to match channels etc. to the correct plugin
static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="$ODK_REPLACE_ME$$UNIQUE_PLUGIN_NAME$" version="1.0" uuid="$ODK_REPLACE_ME$$PLUGIN_GUID$">
  <Info name="$ODK_REPLACE_ME$ Example Plugin: An empty plugin">
    <Vendor name="$ODK_REPLACE_ME$$VENDOR_NAME$"/>
    <Description>$ODK_REPLACE_ME$$PLUGIN_DESCRIPTION$</Description>
  </Info>
  <Host minimum_version="$ODK_REPLACE_ME$$MIN_HOST_VERSION$"/>
</OxygenPlugin>
)XML";

// A minimal translation file that maps the internal ConfigItem key to a nicer text for the user
static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
</TS>
)XML";


// Keys for ConfigItems that are used to store channel settings

// Custom key (prefixed by plugin name) to store path to the input file

// Value of common Oxygen key "SampleRate" determines the sample frequency and the playback rate for the channel

using namespace odk::framework;

class EmptySoftwareChannel : public SoftwareChannelInstance
{
public:

    EmptySoftwareChannel()
    {
    }

    // Describe how the software channel should be shown in the "Add Channel" dialog
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "$ODK_REPLACE_ME$$ADD_INSTANCE_DISPLAY_NAME$";
        telegram.m_service_name = "CreateChannel";
        telegram.m_display_group = "$ODK_REPLACE_ME$$ADD_INSTANCE_DISPLAY_GROUP$";
        telegram.m_description = "$ODK_REPLACE_ME$$ADD_INSTANCE_DISPLAY_DESCRIPTION$";
        return telegram;
    }

    bool update() override
    {
        return true;

    }

    void create(odk::IfHost* host) override
    {
    }

    bool configure(
        const odk::UpdateChannelsTelegram& request,
        std::map<std::uint32_t, std::uint32_t>& channel_id_map) override
    {
        configureFromTelegram(request, channel_id_map);
        return true;
    }

    void prepareProcessing(odk::IfHost* host) override
    {
    }

    void process(ProcessingContext& context, odk::IfHost *host) override
    {
    }

private:
};

class EmptySoftwareChannelPlugin : public SoftwareChannelPlugin<EmptySoftwareChannel>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
    }
};

OXY_REGISTER_PLUGIN1("$ODK_REPLACE_ME$$UNIQUE_PLUGIN_NAME$", PLUGIN_MANIFEST, EmptySoftwareChannelPlugin);

