// Copyright DEWETRON GmbH 2019

#pragma once

#include "odkuni_assert.h"
#include "odkuni_bimap.h"

#include <sstream>


namespace
{
    struct AssertBimapSize
    {
        template <class L, class R>
        AssertBimapSize(const odk::SimpleBiMap<L,R>& map, size_t asserted_size, const char* const map_name)
        {
            if (map.left.size() != asserted_size || map.right.size() != asserted_size)
            {
                std::ostringstream ostream;
                ostream << "Assertion failed: " << map_name << " size " << map.left.size() << " != " << asserted_size << " (actual != expected)";
                const auto e_msg = ostream.str();

                //Make sure this does not go unnoticed
                ODK_ASSERT_FAIL(e_msg.c_str());
                throw std::runtime_error(e_msg.c_str());
            }
        }
    };
}

#define ASSERT_BIMAP_SIZE(M,S)\
        namespace {\
            static const AssertBimapSize assert_size##M(M, S, #M);\
        }
