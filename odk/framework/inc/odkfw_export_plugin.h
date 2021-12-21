// Copyright DEWETRON GmbH 2020
#pragma once

#define ODK_EXTENSION_FUNCTIONS

#include "odkfw_plugin_base.h"
#include "odkfw_export_instance.h"

#include "odkapi_channel_list_xml.h"
#include "odkapi_event_ids.h"
#include "odkapi_export_xml.h"
#include "odkapi_utils.h"
#include "odkuni_defines.h"

namespace odk
{
namespace framework
{

    class ExportPluginBase : public PluginBase
    {
    public:

        virtual void registerTranslations() {};
        virtual void registerResources() { registerTranslations(); }

        std::uint64_t init(std::string& error_message) final;
        bool deinit() final;

    protected:

        void registerExport();
        void unregisterExport();

        virtual odk::RegisterExport getExportInfo() = 0;

        bool checkOxygenCompatibility();
    };

    template<class ExportInstance>
    class ExportPlugin : public ExportPluginBase
    {

    private:
        odk::RegisterExport getExportInfo() final
        {
            return ExportInstance::getExportInfo();
        }

        std::shared_ptr<ExportInstance> getTransaction(uint64_t id)
        {
            auto match = std::find_if(m_instances.begin(), m_instances.end(),
                                      [&id](std::shared_ptr<ExportInstance>& instance)
                                      {
                                          return instance->getID() == id;
                                      });

            if(match != m_instances.end())
            {
                return *match;
            }
            return nullptr;
        }

        bool handleMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret, std::uint64_t& ret_code) override
        {
            switch (id)
            {

            case odk::plugin_msg::EXPORT_VALIDATE_SETTINGS:
            {

                ValidateExport telegram;

                if (parseXMLValue(param, telegram))
                {
                    odk::ValidateExportResponse response;

                    ExportInstance instance;
                    instance.initInstance(getHost());
                    instance.handleValidate(telegram, response);

                    if (ret)
                    {
                        auto error_xml = response.generate();
                        auto result = getHost()->template createValue<odk::IfXMLValue>();
                        result->set(error_xml.c_str());
                        *ret = result.detach();
                    }
                }

                ret_code = odk::error_codes::OK;
                return true;
            }
            case odk::plugin_msg::EXPORT_GENERATE_NAME:
            {
                return true;
            }
            case odk::plugin_msg::EXPORT_START:
            {
                ret_code = odk::error_codes::INVALID_INPUT_PARAMETER;

                odk::StartExport telegram;
                if (parseXMLValue(param, telegram))
                {
                    if(getTransaction(telegram.m_transaction_id))
                    {
                        ret_code = odk::error_codes::INTERNAL_ERROR;
                        return true;
                    }

                    auto instance = std::make_shared<ExportInstance>();

                    instance->initInstance(getHost());
                    m_instances.push_back(instance);
                    instance->handleStartExport(telegram);

                    ret_code = odk::error_codes::OK;
                }
                return true;
            }
            case odk::plugin_msg::EXPORT_CANCEL:
            {
                if(auto transaction = getTransaction(key))
                {
                    transaction->setCanceled();
                    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), transaction), m_instances.end());
                }

                return true;
            }
            case odk::plugin_msg::EXPORT_FINALIZE:
            {
                if(auto transaction = getTransaction(key))
                {
                    transaction->waitForDone();
                    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), transaction), m_instances.end());
                }
                return true;
            }
            default:
                return false;
            }
        }

    private:
        std::vector<std::shared_ptr<ExportInstance>> m_instances;
    };

}
}

