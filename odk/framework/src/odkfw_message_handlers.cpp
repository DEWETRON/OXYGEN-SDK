// Copyright DEWETRON GmbH 2019

#include "odkfw_message_handlers.h"

#include "odkbase_basic_values.h"

namespace odk
{
namespace framework
{

    AcquisitionTaskMessageHandler::AcquisitionTaskMessageHandler(std::uint32_t id, odk::IfHost* host)
        : m_host(host)
    {
        m_info.m_id = id;
    }

    std::uint64_t AcquisitionTaskMessageHandler::pluginMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret)
    {
        if (key == m_info.m_id)
        {
            switch (id)
            {
                case odk::plugin_msg::ACQUISITION_TASK_INIT_TIMEBASES:
                    onInitTimebases();
                    return odk::error_codes::OK;
                case odk::plugin_msg::ACQUISITION_TASK_START_PROCESSING:
                    onStartProcessing();
                    return odk::error_codes::OK;
                case odk::plugin_msg::ACQUISITION_TASK_STOP_PROCESSING:
                    onStopProcessing();
                    return odk::error_codes::OK;
                case odk::plugin_msg::ACQUISITION_TASK_PROCESS:
                    onProcess();
                    return odk::error_codes::OK;
                default:
                    return odk::error_codes::NOT_IMPLEMENTED;
            }
        }
        return odk::error_codes::NOT_IMPLEMENTED;
    }

    void AcquisitionTaskMessageHandler::registerTask(odk::IfHost* host)
    {
        auto xml_msg = host->createValue<odk::IfXMLValue>();
        xml_msg->set(m_info.generate().c_str());
        host->messageSync(odk::host_msg::ACQUISITION_TASK_ADD, 0, xml_msg.get());
    }

    void AcquisitionTaskMessageHandler::unregisterTask(odk::IfHost* host)
    {
        host->messageSync(odk::host_msg::ACQUISITION_TASK_REMOVE, m_info.m_id, nullptr);
    }

}
}
