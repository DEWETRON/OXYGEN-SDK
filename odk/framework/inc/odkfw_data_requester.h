// Copyright DEWETRON GmbH 2020
#pragma once

#define ODK_EXTENSION_FUNCTIONS

#include "odkapi_data_set_xml.h"
#include "odkapi_message_ids.h"
#include "odkbase_message_return_value_holder.h"
#include "odkbase_if_host.h"
#include "odkbase_basic_values.h"
#include "odkbase_api_object_ptr.h"
#include "odkfw_stream_reader.h"

#include "odkapi_data_set_descriptor_xml.h"
#include "odkfw_stream_iterator.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace odk
{
namespace framework
{
    class InputChannel;

    class DataRequestIDManager
    {
    public:

        static uint64_t getNextID()
        {
            return m_next_id++;
        }

    private:
        static uint64_t m_next_id;
    };

    class DataRequester : public IfIteratorUpdater
    {
        static constexpr uint64_t BLOCK_SIZE = 1000;
        static constexpr double DEFAULT_REQUEST_INTERVAL = 0.1;
        static constexpr uint64_t SAMPLES_PER_REQUEST = 10000;

    public:
        DataRequester(odk::IfHost *host, std::shared_ptr<InputChannel> channel, bool user_reduced = false);

        DataRequester(const DataRequester& ) = delete;

        ~DataRequester();

        void setupDataRequest();

        void fetchMoreData();

        void updateStreamIterator(StreamIterator* iterator) final;

        std::shared_ptr<StreamIterator> getIterator(double start, double end);

        std::vector<DataRegion> getDataRegions(double start, double end);

    private:
        odk::IfHost* m_host;
        double m_current_position;
        double m_end_position;
        std::shared_ptr<InputChannel> m_channel;
        DataSetDescriptor m_dataset_descriptor;
        odk::detail::ApiObjectPtr<const IfDataBlockList> m_data_block_list;
        odk::framework::StreamReader m_stream_reader;
        std::shared_ptr<StreamIterator> m_iterator;
        bool m_is_single_value;
        bool m_user_reduced;
        double m_data_request_interval;
        std::uint64_t m_ratio;
    };
}
}
