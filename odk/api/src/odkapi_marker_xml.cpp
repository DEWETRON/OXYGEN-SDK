// Copyright DEWETRON GmbH 2020

#include "odkapi_marker_xml.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <algorithm>
#include <map>

namespace odk
{
    static const std::map<std::string, MarkerType> g_marker_type_map = {
        { "UNKNOWN",                UNKNOWN             },
        { "RECORDING_START",        RECORDING_START     },
        { "RECORDING_STOP",         RECORDING_STOP      },
        { "RECORDING_PAUSE",        RECORDING_PAUSE     },
        { "RECORDING_RESUME",       RECORDING_RESUME    },
        { "TRIGGER_START",          TRIGGER_START       },
        { "TRIGGER_STOP",           TRIGGER_STOP        },
        { "TRIGGER_PRETIME",        TRIGGER_PRETIME     },
        { "TRIGGER_POSTTIME",       TRIGGER_POSTTIME    },
        { "TRIGGER_ARMED",          TRIGGER_ARMED       },
        { "TRIGGER_DISARMED",       TRIGGER_DISARMED    },
        { "TEXT",                   TEXT                },
        { "SYNC_SIGNAL_LOST",       SYNC_SIGNAL_LOST    },
        { "SYNC_TIME_ERROR",        SYNC_TIME_ERROR     },
        { "SYNC_SYNC_LOST",         SYNC_SYNC_LOST      },
        { "SYNC_SYNC_ERROR",        SYNC_SYNC_ERROR     },
        { "SYNC_RESYNCED",          SYNC_RESYNCED       },
        { "TOPOLOGY_NODE_LOST",     TOPOLOGY_NODE_LOST  },
        { "STORING_START",          STORING_START       },
        { "STORING_STOP",           STORING_STOP        },
        { "ALARM",                  ALARM               },
        { "ALARM_ACK",              ALARM_ACK           },
        { "EVENT",                  EVENT               },
        { "SPLIT_START",            SPLIT_START         },
        { "SPLIT_STOP",             SPLIT_STOP          },
    };

    MarkerType getMarkerType(const std::string& marker_type)
    {
        auto it = g_marker_type_map.find(marker_type);
        return it != g_marker_type_map.end() ? it->second : UNKNOWN;
    }

    std::string toTypeString(const MarkerType tp)
    {
        const auto it = std::find_if(g_marker_type_map.begin(), g_marker_type_map.end(),
            [tp]
            (const std::pair<std::string, MarkerType> p)
            {
                return p.second == tp;
            });
        return (it != g_marker_type_map.end()) ? it->first : "";
    }

    PluginMarkerRequest::PluginMarkerRequest()
        : m_start(std::numeric_limits<double>::max())
        , m_stop(std::numeric_limits<double>::max())
    {}

    PluginMarkerRequest::PluginMarkerRequest(double start, double stop)
        : m_start(start)
        , m_stop(stop)
    {}

    bool PluginMarkerRequest::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto marker_request_node = doc.document_element();
                auto window_node = marker_request_node.select_node("Window");
                m_start = odk::from_string<double>(window_node.node().attribute("start").value());
                m_stop = odk::from_string<double>(window_node.node().attribute("end").value());
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginMarkerRequest::generate() const
    {
        pugi::xml_document doc;
        auto marker_request_node = doc.append_child("MarkerRequest");

        auto window_node = marker_request_node.append_child("Window");

        window_node.append_attribute("start").set_value(m_start);
        window_node.append_attribute("end").set_value(m_stop);

        return xpugi::toXML(doc);
    }


    Marker::Marker()
        : m_timestamp()
        , m_type("UNKNOWN")
        , m_msg{}
        , m_desc{}
        , m_group_id{}
        , m_timebase{}
        , m_is_mutable(true)
    {
    }

    Marker::Marker( std::uint64_t ticks
                    , const odk::Timebase& m_timebase
                    , const std::string& type
                    , const std::string& message
                    , const std::string& description
                    , const std::string& group_id
                    , bool is_mutable
                                      )
        : m_timestamp(ticks, m_timebase.m_frequency)
        , m_type(type)
        , m_msg(message)
        , m_desc(description)
        , m_group_id(group_id)
        , m_is_mutable(is_mutable)
        , m_recording_id()
    {
    }

    bool Marker::parse(const char* xml_string)
    {
        m_timestamp = odk::Timestamp();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            if (!parseMarker(doc.document_element()))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    bool Marker::parseMarker(const pugi::xml_node& parent)
    {
        if (!odk::strequal(parent.name(), "Marker"))
        {
            return false;
        }

        if (auto type_attr = parent.attribute("type"))
        {
            m_type = type_attr.value();
        }

        if (auto ismutable_attr = parent.attribute("ismutable"))
        {
            m_is_mutable = ismutable_attr.as_bool();
        }

        auto timestamp_node = parent.child("Timestamp");
        if (timestamp_node)
        {
            if (!m_timestamp.parseTickFrequencyAttributes(timestamp_node))
            {
                return false;
            }
        }

        auto timebase_node = parent.child("SimpleTimebase");
        if (timebase_node)
        {
            if (!m_timebase.parse(timebase_node))
            {
                return false;
            }
        }
        auto with_offset_node = parent.child("TimebaseWithOffset");
        if (with_offset_node)
        {
            if (!m_timebase.parseWithOffset(with_offset_node))
            {
                return false;
            }
        }

        if(auto message_node = parent.child("Message"))
        {
            m_msg = message_node.child_value();
        }

        if(auto desc_node = parent.child("Description"))
        {
            m_desc = desc_node.child_value();
        }

        if(auto storage_node = parent.child("StorageGroup"))
        {
            m_group_id = storage_node.child_value();
        }

        if(auto recording_id_node = parent.child("RecordingId"))
        {
            m_recording_id = recording_id_node.child_value();
        }

        return true;
    }

    std::string Marker::generate() const
    {
        pugi::xml_document doc;
        generateMarker(doc);
        return xpugi::toXML(doc);
    }

    void Marker::generateMarker(pugi::xml_node& parent) const
    {
        auto marker_node = parent.append_child("Marker");
        marker_node.append_attribute("type").set_value(m_type.c_str());
        marker_node.append_attribute("ismutable").set_value(m_is_mutable);

        auto timestamp_node = marker_node.append_child("Timestamp");
        m_timestamp.writeTickFrequencyAttributes(timestamp_node);

        m_timebase.store(marker_node);

        if (!m_msg.empty())
        {
            auto message_node = marker_node.append_child("Message");
            message_node.append_child(pugi::node_pcdata).set_value(m_msg.c_str());
        }

        if (!m_desc.empty())
        {
            auto desc_node = marker_node.append_child("Description");
            desc_node.append_child(pugi::node_pcdata).set_value(m_desc.c_str());
        }
        if (!m_group_id.empty())
        {
            auto storage_group_node = marker_node.append_child("StorageGroup");
            storage_group_node.append_child(pugi::node_pcdata).set_value(m_group_id.c_str());
        }
        if (!m_recording_id.empty())
        {
            auto recording_id_node = marker_node.append_child("RecordingId");
            recording_id_node.append_child(pugi::node_pcdata).set_value(m_recording_id.c_str());
        }
    }

    MarkerList::MarkerList()
    {
    }

    bool MarkerList::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status != pugi::status_ok)
        {
            return false;
        }

        auto node = doc.document_element();
        if (!node)
        {
            return false;
        }

        if (!odk::strequal(node.name(), "Markers"))
        {
            return false;
        }

        for (const auto marker_node : node.children())
        {
            Marker marker;
            if(marker.parseMarker(marker_node))
            {
                m_markers.push_back(marker);
            }
        }

        return true;
    }

    std::string MarkerList::generate() const
    {
        pugi::xml_document doc;
        auto root = doc.append_child("Markers");

        for(const auto& marker : m_markers)
        {
            marker.generateMarker(root);
        }

        return xpugi::toXML(doc);
    }

}

