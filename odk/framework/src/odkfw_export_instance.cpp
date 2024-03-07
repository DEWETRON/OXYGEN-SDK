// Copyright DEWETRON GmbH 2020

#include "odkfw_export_instance.h"

#define ODK_EXTENSION_FUNCTIONS

#include "odkapi_error_codes.h"
#include "odkapi_message_ids.h"
#include "odkfw_data_requester.h"

namespace odk
{
namespace framework
{
    ExportInstance::ExportInstance()
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
        for (const auto& channel_id : validate_telegram.m_properties.m_channels)
        {
            auto new_input_channel = std::make_shared<InputChannel>(m_host, channel_id);
            new_input_channel->updateDataFormat();
            new_input_channel->updateTimeBase();
            context.m_channels.emplace(channel_id, std::move(new_input_channel));
        }

        validate(context, response);
    }

    void ExportInstance::initInstance(odk::IfHost* host)
    {
        m_host = host;
    }

    odk::IfHost* ExportInstance::getHost() const noexcept
    {
        return m_host;
    }

    void ExportInstance::handleStartExport(const odk::StartExport& start_telegram)
    {
        odk::ValidateExportResponse response;
        odk::ValidateExport validate;
        validate.m_properties = start_telegram.m_properties;
        handleValidate(validate, response);

        if (!response.m_success)
        {
            this->notifyError();
            return;
        }

        m_telegram = start_telegram;
        m_context.m_properties = start_telegram.m_properties;

        bool export_waveform = true;
        bool export_statistic = false;

        if (m_context.m_properties.m_custom_properties.containsProperty("WAVEFORM") ||
            m_context.m_properties.m_custom_properties.containsProperty("EXPORT_WAVEFORM"))
        {
            export_waveform = m_context.m_properties.m_custom_properties.getBool("WAVEFORM") ||
                m_context.m_properties.m_custom_properties.getBool("EXPORT_WAVEFORM");
        }
        if (m_context.m_properties.m_custom_properties.containsProperty("STATISTIC") ||
            m_context.m_properties.m_custom_properties.containsProperty("EXPORT_STATISTICS"))
        {
            export_statistic = m_context.m_properties.m_custom_properties.getBool("STATISTIC") ||
                m_context.m_properties.m_custom_properties.getBool("EXPORT_STATISTICS");
        }

        for(const auto& channel_id : start_telegram.m_properties.m_channels)
        {
            auto new_input_channel = std::make_shared<InputChannel>(m_host, channel_id);
            new_input_channel->updateDataFormat();
            new_input_channel->updateTimeBase();
            m_context.m_channels.insert_or_assign(channel_id, new_input_channel);

            auto& first_interval = start_telegram.m_properties.m_export_intervals.front();

            if (export_waveform)
            {
                auto raw_requester = std::make_unique<DataRequester>(getHost(), new_input_channel);
                try
                {
                    m_context.m_channel_iterators[channel_id] =
                        raw_requester->getIterator(first_interval.m_begin, first_interval.m_end);
                }
                catch (const std::exception&)
                {
                    // no valid data
                }
                m_data_requester.push_back(std::move(raw_requester));
            }

            if (export_statistic)
            {
                auto reduced_requester = std::make_unique<DataRequester>(getHost(), new_input_channel, true);
                try
                {
                    m_context.m_reduced_channel_iterators[channel_id] =
                        reduced_requester->getIterator(first_interval.m_begin, first_interval.m_end);
                }
                catch (const std::exception&)
                {
                    // no valid data
                }
                m_reduced_requester.push_back(std::move(reduced_requester));
            }
        }

        auto exportFunction = [this]()
        {
            bool success = true;
            try
            {
                success = this->exportData(this->m_context);
            }
            catch(const std::exception&)
            {
                this->cancel();
                success = false;
            }

            if (success)
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
        m_canceled.store(true, std::memory_order_release);
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
        if (m_canceled.load(std::memory_order_acquire))
        {
            throw std::runtime_error("transaction cancelled");
        }

        auto p = m_host->createValue<odk::IfUIntValue>();
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
