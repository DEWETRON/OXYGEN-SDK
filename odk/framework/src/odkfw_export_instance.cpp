// Copyright DEWETRON GmbH 2020

#include "odkfw_export_instance.h"

#define ODK_EXTENSION_FUNCTIONS

#include "odkapi_block_descriptor_xml.h"
#include "odkapi_data_set_xml.h"
#include "odkapi_message_ids.h"
#include "odkapi_utils.h"
#include "odkfw_channels.h"
#include "odkfw_properties.h"
#include "odkfw_stream_reader.h"

namespace odk
{
namespace framework
{
    ExportInstance::ExportInstance()
        : m_canceled(false)
    {
    }

    ExportInstance::~ExportInstance()
    {
        waitForDone();
    }

    void ExportInstance::handleValidate(const ValidateExport &validate_telegram, ValidateExportResponse &response)
    {
        ValidationContext context;
        context.m_properties = validate_telegram.m_properties;
        for(const auto& channel_id : validate_telegram.m_properties.m_channels)
        {
            auto new_input_channel = std::make_shared<InputChannel>(m_host, channel_id);
            new_input_channel->updateDataFormat();
            new_input_channel->updateTimeBase();
            context.m_channels.insert(std::make_pair(channel_id, new_input_channel));
        }

        validate(context, response);
    }

    void ExportInstance::initInstance(odk::IfHost* host)
    {
        m_host = host;
    }

    odk::IfHost* ExportInstance::getHost() const
    {
        return m_host;
    }

    void ExportInstance::handleStartExport(const odk::StartExport& start_telegram)
    {
        odk::ValidateExportResponse response;
        odk::ValidateExport validate;
        validate.m_properties = start_telegram.m_properties;
        handleValidate(validate, response);

        if(!response.m_success)
        {
            this->notifyError();
            return;
        }

        m_telegram = start_telegram;
        m_context.m_properties = start_telegram.m_properties;

        for(const auto& channel_id : start_telegram.m_properties.m_channels)
        {
            auto new_input_channel = std::make_shared<InputChannel>(m_host, channel_id);
            new_input_channel->updateDataFormat();
            new_input_channel->updateTimeBase();
            m_context.m_channels.insert(std::make_pair(channel_id, new_input_channel));

            auto requester = std::make_shared<DataRequester>(getHost(), channel_id);
            m_data_requester.push_back(requester);

            auto first_interval = start_telegram.m_properties.m_export_intervals[0];

            try
            {
                m_context.m_channel_iterators[channel_id] =
                    requester->getIterator(first_interval.m_begin, first_interval.m_end);
            }
            catch (const std::exception&)
            {
                // no valid data
            }
        }

        auto exportFunction = [this]()
        {
            bool success = true;
            try
            {
                success = this->exportData(this->m_context);
            }
            catch(std::runtime_error&)
            {
                this->cancel();
                success = false;
            }

            if(success)
            {
                this->notifyDone();
            }
            else
            {
                this->notifyError();
            }
        };
        m_worker_thread = std::thread(exportFunction);
    }

    void ExportInstance::setCanceled()
    {
        m_canceled.store(true);
        waitForDone();
    }

    void ExportInstance::waitForDone()
    {
        if (m_worker_thread.joinable())
        {
            m_worker_thread.join();
        }
    }

    uint64_t ExportInstance::getID() const
    {
        return m_telegram.m_transaction_id;
    }

    void ExportInstance::notifyProgress(uint64_t progress) const
    {
        if(m_canceled.load())
        {
            throw std::runtime_error("transaction canceled");
        }

        auto p = getHost()->createValue<odk::IfUIntValue>();
        p->set(progress);
        m_host->messageSync(odk::host_msg::EXPORT_PROGRESS, m_telegram.m_transaction_id, p.get(), nullptr);
    }

    void ExportInstance::notifyDone() const
    {
        m_host->messageSync(odk::host_msg::EXPORT_FINISHED, m_telegram.m_transaction_id, nullptr, nullptr);
    }

    void ExportInstance::notifyError() const
    {
        auto error_code = odk::error_codes::INTERNAL_ERROR;
        auto param = m_host->createValue<odk::IfUIntValue>();
        param->set(error_code);
        m_host->messageSync(odk::host_msg::EXPORT_FAILED, m_telegram.m_transaction_id, param.get(), nullptr);
    }
}
}

