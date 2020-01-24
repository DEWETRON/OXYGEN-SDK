// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkapi_acquisition_task_xml.h"
#include "odkfw_if_message_handler.h"
#include "odkapi_update_config_xml.h"

namespace odk
{
namespace framework
{

    class ConfigMessageHandler : public oxy::IfMessageHandler
    {
    public:
        ConfigMessageHandler(odk::IfHost* host);

        std::uint64_t pluginMessage(
            odk::PluginMessageId id,
            std::uint64_t key,
            const odk::IfValue* param,
            const odk::IfValue** ret) final;

        void setConfigToHost();

        void handleConfigChangeRequest(const odk::IfValue* param);

        virtual void onConfigChanged(const UpdateConfigTelegram& update_telegram) {};

        odk::IfHost* m_host;
        UpdateConfigTelegram m_info;
    };

    class AcquisitionTaskMessageHandler : public oxy::IfMessageHandler
    {
    public:
        AcquisitionTaskMessageHandler(std::uint32_t id, odk::IfHost* host);

        virtual ~AcquisitionTaskMessageHandler() = default;

        std::uint64_t pluginMessage(
            odk::PluginMessageId id,
            std::uint64_t key,
            const odk::IfValue* param,
            const odk::IfValue** ret) final;

        void registerTask(odk::IfHost* host);
        void unregisterTask(odk::IfHost* host);

        virtual void onInitTimebases() {}
        virtual void onStartProcessing() {}
        virtual void onProcess() {}
        virtual void onStopProcessing() {}

        odk::AddAcquisitionTaskTelegram m_info;
        odk::IfHost* m_host;
    };

}
}
