// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkapi_timestamp_xml.h"
#include "odkapi_timebase_xml.h"
#include "odkapi_types.h"
#include "odkuni_xpugixml.h"

#include <string>
#include <vector>

namespace odk
{
    enum MarkerType
    {
        UNKNOWN = 0x000000,
        RECORDING_START,
        RECORDING_STOP,
        RECORDING_PAUSE,
        RECORDING_RESUME,
        TRIGGER_START,
        TRIGGER_STOP,
        TRIGGER_PRETIME,
        TRIGGER_POSTTIME,
        TRIGGER_ARMED,
        TRIGGER_DISARMED,
        TEXT,
        SYNC_SIGNAL_LOST,
        SYNC_TIME_ERROR,
        SYNC_SYNC_LOST,
        SYNC_SYNC_ERROR,
        SYNC_RESYNCED,
        TOPOLOGY_NODE_LOST,
        STORING_START,
        STORING_STOP,
        ALARM,
        ALARM_ACK,
        EVENT,
        SPLIT_START,
        SPLIT_STOP,
    };

    MarkerType getMarkerType(const std::string& marker_type);
    std::string toTypeString(const MarkerType);

    class PluginMarkerRequest
    {
    public:
        PluginMarkerRequest();
        explicit PluginMarkerRequest(double start, double stop);

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint32_t m_id;

        double m_start;
        double m_stop;

    };

    class Marker
    {
    public:
        Marker();
        Marker( std::uint64_t ticks,
                const odk::Timebase& m_timebase,
                const std::string& type,
                const std::string& message,
                const std::string& description,
                const std::string& group_id,
                bool is_mutable
                );

        bool parse(const char* xml_string);

        std::string generate() const;

        odk::Timestamp m_timestamp;
        std::string m_type;
        std::string m_msg;
        std::string m_desc;
        std::string m_group_id;
        odk::Timebase m_timebase;
        bool m_is_mutable;
        std::string m_recording_id;

        void generateMarker(pugi::xml_node& parent) const;
        bool parseMarker(const pugi::xml_node& parent);
    };

    class MarkerList
    {
    public:
        MarkerList();

        bool parse(const char* xml_string);

        std::string generate() const;

        std::vector<Marker> m_markers;
    };

}

