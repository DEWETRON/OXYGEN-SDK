// Copyright DEWETRON GmbH 2015

#pragma once

#include "odkuni_defines.h"
#include "odkuni_logger.h"
#include "odkuni_string_util.h"
#include "odkapi_version_xml.h"
#include "pugixml.hpp"

#include <string>
#include <vector>

namespace odk
{
    /**
     * Class to store a list of XML nodes
     * @sa neoncfg::Node
     */
    template<class T>
    class NodeList
    {
    public:
        ODK_NODISCARD size_t size() const noexcept
        {
            return m_nodes.size();
        }

        ODK_NODISCARD bool empty() const noexcept
        {
            return m_nodes.empty();
        }

        void append(const T& property)
        {
            m_nodes.push_back(property);
        }

        void append(T&& property)
        {
            m_nodes.push_back(std::move(property));
        }

        template<class... Args>
        void emplace_back(Args&&... args)
        {
            m_nodes.emplace_back(std::forward<Args>(args)...);
        }

        void insert(size_t idx, const T& property)
        {
            m_nodes.insert(m_nodes.begin()+idx, property);
        }

        T& getNode(size_t idx)
        {
            return m_nodes.at(idx);
        }

        const T& getNode(size_t idx) const
        {
            return m_nodes.at(idx);
        }

        void clear()
        {
            m_nodes.clear();
        }

        void reserve(std::size_t capacity)
        {
            m_nodes.reserve(capacity);
        }

        typedef T value_type;
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;

        iterator begin()
        {
            return m_nodes.begin();
        }
        const_iterator begin() const
        {
            return m_nodes.begin();
        }
        const_iterator cbegin() const
        {
            return m_nodes.cbegin();
        }

        iterator end()
        {
            return m_nodes.end();
        }
        const_iterator end() const
        {
            return m_nodes.end();
        }
        const_iterator cend() const
        {
            return m_nodes.end();
        }

        // FIXXXME iterators should be const_iterators, but Ubuntu 14.04 does not support that yet
        iterator erase(iterator first, iterator last)
        {
            return m_nodes.erase(first, last);
        }

        pugi::xml_node appendTo(pugi::xml_node parent) const
        {
            for (const auto& a_node : m_nodes)
            {
                a_node.appendTo(parent);
            }

            return pugi::xml_node{};
        }

        bool readFrom(const pugi::xml_node& tree, const Version& version)
        {
            m_nodes.clear();
            for (pugi::xml_node node : tree.children())
            {
                if (node.type() == pugi::node_element)
                {
                    T p;
                    if (odk::strequal(node.name(), p.getNodeName().c_str()))
                    {
                        bool ok = p.readFrom(node, version);
                        if (ok)
                        {
                            append(p);
                        }
                    }
                    else
                    {
                        std::string error = std::string("Warning: ") + tree.name() +
                            " has unknown node " + node.name();
                        ODKLOG_WARN(error.c_str())
                    }
                }
            }
            return true;
        }

        bool operator==(const NodeList<T>& other) const
        {
            return m_nodes == other.m_nodes;
        }

    private:
        std::vector<T> m_nodes;
    };

} // namespace neoncfg
