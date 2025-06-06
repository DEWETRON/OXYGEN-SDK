// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkapi_export_xml.h"
#include "odkbase_if_host_fwd.h"
#include "odkfw_input_channel.h"
#include "odkfw_stream_iterator.h"
#include "odkuni_defines.h"

#include <atomic>
#include <map>
#include <memory>
#include <thread>
#include <stdint.h>

namespace odk
{
namespace framework
{
    class DataRequester;

    class ExportInstance
    {
    public:
        class ValidationContext
        {
        public:
            std::map<uint64_t, std::shared_ptr<InputChannel>> m_channels;
            odk::ExportProperties m_properties;
        };

        class ProcessingContext : public ValidationContext
        {
        public:
            std::map<uint64_t, std::shared_ptr<odk::framework::StreamIterator>> m_channel_iterators;
            std::map<uint64_t, std::shared_ptr<odk::framework::StreamIterator>> m_reduced_channel_iterators;
        };

        ExportInstance();
        virtual ~ExportInstance();

        void handleValidate(const odk::ValidateExport& validate_telegram, odk::ValidateExportResponse& response);
        void handleStartExport(const odk::StartExport& start_telegram);

        void setCanceled();

        void waitForDone();

        ODK_NODISCARD uint64_t getID() const;

        virtual void validate(const ValidationContext& context, odk::ValidateExportResponse& response) const = 0;
        virtual bool exportData(const ProcessingContext& context) = 0;
        virtual void cancel() = 0;

    private:
        template<class ExportInstance>
        friend class ExportPlugin;

        void initInstance(odk::IfHost* host);
        void notifyDone() const;
        void notifyError() const;

    protected:
        ODK_NODISCARD odk::IfHost* getHost() const noexcept;

        void notifyProgress(uint64_t progress, const std::string& extra_info = {}) const;

    private:
        odk::IfHost* m_host = nullptr;
        std::thread m_worker_thread;
        std::atomic<bool> m_canceled = false;
        std::vector<std::unique_ptr<DataRequester>> m_data_requester;
        std::vector<std::unique_ptr<DataRequester>> m_reduced_requester;
        ProcessingContext m_context;
        odk::StartExport m_telegram;
    };
}
}
