// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_types.h"

#include <boost/optional.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace odk
{
    /**
     * Generates the RegisterDataSet XML message used in the DATA_GROUP_ADD command
     */
    class PluginDataSet
    {
    public:
        PluginDataSet();
        explicit PluginDataSet(std::uint64_t id, const std::vector<std::uint64_t>& channels, DataSetType type = DataSetType::SCALED, DataSetMode mode = DataSetMode::NORMAL, StreamPolicy policy = StreamPolicy::EXACT);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;
        std::vector<std::uint64_t> m_channels;
        DataSetType m_data_set_type;
        DataSetMode m_data_mode;
        StreamPolicy m_policy;
    };

    class PluginDataRequest
    {
    public:

        class SingleValue
        {
        public:

            SingleValue() = delete;

            SingleValue(double timestamp)
                : m_timestamp(timestamp)
            {}

            double m_timestamp;
        };

        class DataWindow
        {
        public:

            DataWindow() = delete;

            DataWindow(double start, double stop)
                : m_start(start)
                , m_stop(stop)
            {}

            double m_start;
            double m_stop;
        };

        class DataStream
        {};

        PluginDataRequest();

        explicit PluginDataRequest(std::uint64_t id, DataWindow data_window);

        explicit PluginDataRequest(std::uint64_t id, SingleValue single_value);

        explicit PluginDataRequest(std::uint64_t id, DataStream data_stream);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;

        boost::optional<DataWindow> m_data_window;
        boost::optional<SingleValue> m_single_value;
        boost::optional<DataStream> m_data_stream;

    };

    class PluginDataStartRequest
    {
    public:
        PluginDataStartRequest();
        explicit PluginDataStartRequest(std::uint64_t id, double duration);
        explicit PluginDataStartRequest(std::uint64_t id, double start, double duration);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;

        boost::optional<double> m_start;
        boost::optional<double> m_block_duration;
        boost::optional<bool>   m_ignore_regions;
        StreamType m_stream_type;

    };

    class PluginDataStopRequest
    {
    public:
        PluginDataStopRequest();
        explicit PluginDataStopRequest(std::uint64_t id);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;

    };


    class PluginDataRegionsRequest
    {
    public:
        class DataWindow
        {
        public:

            DataWindow() = delete;

            DataWindow(double start, double stop)
                : m_start(start)
                  , m_stop(stop)
            {}

            double m_start;
            double m_stop;
        };

        PluginDataRegionsRequest();

        explicit PluginDataRegionsRequest(std::uint64_t id);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;
        boost::optional<DataWindow> m_data_window;
    };
}

