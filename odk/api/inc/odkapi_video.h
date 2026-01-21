// Copyright DEWETRON GmbH 2026

#include <cstdint>

namespace odk
{
    struct VideoFrameHeader
    {
        char fourcc[4];
        uint32_t width;
        uint32_t height;
    };
}
