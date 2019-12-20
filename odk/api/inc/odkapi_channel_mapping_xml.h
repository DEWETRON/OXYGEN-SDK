// Copyright DEWETRON GmbH 2019
#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace odk
{
    template <class IdType>
    class ChannelMappingTelegram
    {
    public:
        ChannelMappingTelegram();

        bool parse(const char* xml_string);

        std::string generate() const;

        typedef std::map<IdType, IdType> MapType;
        MapType m_channel_id_map;
    };

    extern template class ChannelMappingTelegram<std::uint32_t>;
    extern template class ChannelMappingTelegram<std::uint64_t>;
}
