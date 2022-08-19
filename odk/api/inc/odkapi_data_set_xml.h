// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_types.h"
#include "odkuni_defines.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
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

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

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

            SingleValue(double timestamp) noexcept
                : m_timestamp(timestamp)
            {}

            double m_timestamp;
        };

        class DataWindow
        {
        public:
            
            DataWindow(double start, double stop) noexcept
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

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

        std::uint64_t m_id;

        std::optional<DataWindow> m_data_window;
        std::optional<SingleValue> m_single_value;
        std::optional<DataStream> m_data_stream;

    };

    class PluginDataStartRequest
    {
    public:
        PluginDataStartRequest();
        explicit PluginDataStartRequest(std::uint64_t id, double duration);
        explicit PluginDataStartRequest(std::uint64_t id, double start, double duration);

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

        std::uint64_t m_id;

        std::optional<double> m_start;
        std::optional<double> m_block_duration;
        std::optional<bool>   m_ignore_regions;
        StreamType m_stream_type;

    };

    class PluginDataStopRequest
    {
    public:
        PluginDataStopRequest();
        explicit PluginDataStopRequest(std::uint64_t id);

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

        std::uint64_t m_id;

    };


    class PluginDataRegionsRequest
    {
    public:
        class DataWindow
        {
        public:

            DataWindow(double start, double stop) noexcept
                : m_start(start)
                , m_stop(stop)
            {}

            double m_start;
            double m_stop;
        };

        PluginDataRegionsRequest();

        explicit PluginDataRegionsRequest(std::uint64_t id);

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

        std::uint64_t m_id;
        std::optional<DataWindow> m_data_window;
    };
}

