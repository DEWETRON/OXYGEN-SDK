// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkfw_fwd.h"

#include "odkapi_update_config_xml.h"

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
    protected:
        ~IfPluginChannelChangeListener() {}
    };

    class IfChannelPropertyChangeListener
    {
    public:
        virtual void onChannelPropertyChanged(const IfChannelProperty* channel) = 0;
    protected:
        ~IfChannelPropertyChangeListener() {}
    };

    class IfPluginTaskChangeListener
    {
    public:
        virtual void onTaskChannelAdded(PluginTask* task, const PluginChannel* channel) = 0;
        virtual void onTaskChannelRemoved(PluginTask* task, const PluginChannel* channel) = 0;
        virtual void onTaskInputChannelsChanged(PluginTask* task) = 0;
    protected:
        ~IfPluginTaskChangeListener() {}
    };


    class IfChannelProperty
    {
    public:
        virtual void setChangeListener(IfChannelPropertyChangeListener* l) = 0;
        virtual void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const = 0;
        virtual bool update(const odk::Property& value) = 0;
    protected:
        ~IfChannelProperty() {}
    };

    class IfTaskWorker
    {
    public:
        virtual void onInitTimebases(odk::IfHost* host, std::uint64_t token) {}
        virtual void onStartProcessing(odk::IfHost* host, std::uint64_t token) {}
        virtual void onProcess(odk::IfHost* host, std::uint64_t token) = 0;
        virtual void onStopProcessing(odk::IfHost* host, std::uint64_t token) {}
        virtual void onChannelConfigChanged(odk::IfHost* host, std::uint64_t token) {};

        virtual void onChannelDataformatChanged(std::uint64_t channel_id) {};
    protected:
        ~IfTaskWorker() {}
    };

}
}
