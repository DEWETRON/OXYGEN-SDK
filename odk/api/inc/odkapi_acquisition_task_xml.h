// Copyright DEWETRON GmbH 2019
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace odk
{
    class AddAcquisitionTaskTelegram
    {
    public:
        AddAcquisitionTaskTelegram();

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_id;
        std::vector<std::uint64_t> m_input_channels;
        std::vector<std::uint64_t> m_output_channels;
    };

}

