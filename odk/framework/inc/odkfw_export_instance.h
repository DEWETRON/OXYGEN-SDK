// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include "odkapi_export_xml.h"
#include "odkapi_timestamp_xml.h"
#include "odkbase_basic_values.h"
#include "odkfw_data_requester.h"
#include "odkfw_input_channel.h"
#include "odkfw_interfaces.h"
#include "odkfw_stream_iterator.h"

#include <atomic>
#include <set>
#include <thread>

namespace odk
{
namespace framework
{
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
        };


        ExportInstance();
        virtual ~ExportInstance();

        void handleValidate(const odk::ValidateExport& validate_telegram, odk::ValidateExportResponse& response);
        void handleStartExport(const odk::StartExport& start_telegram);

        void setCanceled();

        void waitForDone();

        uint64_t getID() const;

        virtual void validate(const ValidationContext& context, odk::ValidateExportResponse& response) const = 0;
        virtual bool exportData(const ProcessingContext& context) const = 0;
        virtual void cancel() = 0;

    private:
        template<class ExportInstance>
        friend class ExportPlugin;

        void initInstance(odk::IfHost* host);
        void notifyDone() const;
        void notifyError() const;

    protected:
        odk::IfHost* getHost() const;

        void notifyProgress(uint64_t progress) const;

    private:
        odk::IfHost* m_host = nullptr;
        std::thread m_worker_thread;
        std::atomic<bool> m_canceled;
        std::vector<std::shared_ptr<DataRequester>> m_data_requester;
        ProcessingContext m_context;
        odk::StartExport m_telegram;
    };

}
}

