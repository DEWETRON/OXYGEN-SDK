// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkfw_fwd.h"

#include "odkapi_update_config_xml.h"
#include "odkuni_defines.h"
#include "odkapi_timestamp_xml.h"
#include "odkbase_basic_values.h"

#include <string>
#include <cstdint>

namespace odk
{
namespace framework
{

    class IfPluginChannelChangeListener
    {
    public:
        virtual void onChannelSetupChanged(const PluginChannel* channel) = 0;
        virtual void onChannelPropertyChanged(const PluginChannel* channel, const std::string& name) = 0;
        virtual bool configChangeAllowed() const = 0;
    protected:
        virtual ~IfPluginChannelChangeListener() = default;
    };

    class IfChannelPropertyChangeListener
    {
    public:
        virtual void onChannelPropertyChanged(const IfChannelProperty* channel) = 0;
    protected:
        virtual ~IfChannelPropertyChangeListener() = default;
    };

    class IfPluginTaskChangeListener
    {
    public:
        virtual void onTaskChannelAdded(PluginTask* task, const PluginChannel* channel) = 0;
        virtual void onTaskChannelRemoved(PluginTask* task, const PluginChannel* channel) = 0;
        virtual void onTaskInputChannelsChanged(PluginTask* task) = 0;
    protected:
        virtual ~IfPluginTaskChangeListener() = default;
    };


    class IfChannelProperty
    {
    public:
        virtual void setChangeListener(IfChannelPropertyChangeListener* l) = 0;
        virtual void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const = 0;
        virtual bool update(const odk::Property& value) = 0;
    protected:
        virtual ~IfChannelProperty() = default;
    };

    class IfTaskWorker
    {
    public:
        virtual void onInitTimebases(odk::IfHost* host, std::uint64_t token) { ODK_UNUSED(host); ODK_UNUSED(token); }
        virtual void onStartProcessing(odk::IfHost* host, std::uint64_t token) { ODK_UNUSED(host); ODK_UNUSED(token); }
        virtual void onProcess(odk::IfHost* host, std::uint64_t token, const odk::IfXMLValue* param) = 0;
        virtual void onStopProcessing(odk::IfHost* host, std::uint64_t token) { ODK_UNUSED(host); ODK_UNUSED(token); }
        virtual void onChannelConfigChanged(odk::IfHost* host, std::uint64_t token) { ODK_UNUSED(host); ODK_UNUSED(token); };

        virtual void onChannelDataformatChanged(std::uint64_t channel_id) { ODK_UNUSED(channel_id); };
    protected:
        virtual ~IfTaskWorker() = default;
    };

}
}
