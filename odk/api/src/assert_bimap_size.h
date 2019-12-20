// Copyright DEWETRON GmbH 2019

#pragma once

#include "odkuni_assert.h"

#include <boost/bimap.hpp>
#include <boost/format.hpp>


namespace
{
    struct AssertBimapSize
    {
        template <class L, class R>
        AssertBimapSize(const boost::bimap<L,R>& map, size_t asserted_size, const char* const map_name)
        {
            if (map.size() != asserted_size)
            {
                const auto e_msg = boost::str(
                    boost::format("Assertion failed: %s size %u != %u (actual != expected)")
                        % map_name
                        % map.size()
                        % asserted_size
                );

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
