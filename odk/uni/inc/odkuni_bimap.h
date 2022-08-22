// Copyright (c) DEWETRON GmbH 2022
#pragma once

#include <map>
#include <stdexcept>

namespace odk
{

    /// SimpleBiMap - bidirectional mapping
    template <typename L, typename R>
    class SimpleBiMap
    {
    public:
        using value_type = std::pair<L, R>;

        SimpleBiMap()
        {
        }

        SimpleBiMap(std::initializer_list<value_type> elements)
        {
            for (const auto& element : elements)
            {
                m_l2r[element.first] = element.second;
                m_r2l[element.second] = element.first;
            }
        }

        bool insert(const value_type& element)
        {
            if (m_l2r.count(element.first) == 0 && m_r2l.count(element.second) == 0)
            {
                m_l2r[element.first] = element.second;
                m_r2l[element.second] = element.first;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool hasLeft(const L& val) const
        {
            return m_l2r.count(val) != 0;
        }

        bool hasRight(const R& val) const
        {
            return m_r2l.count(val) != 0;
        }

        const L& getLeftOr(const R& key, const L& def) const
        {
            auto it = m_r2l.find(key);
            if (it != m_r2l.end())
            {
                return it->second;
            }
            else
            {
                return def;
            }
        }

        const R& getRightOr(const L& key, const R& def) const
        {
            auto it = m_l2r.find(key);
            if (it != m_l2r.end())
            {
                return it->second;
            }
            else
            {
                return def;
            }
        }

        const L& getLeft(const R& key) const
        {
            auto it = m_r2l.find(key);
            if (it != m_r2l.end())
            {
                return it->second;
            }
            else
            {
                throw std::out_of_range("Right side key not found");
            }
        }

        const R& getRight(const L& key) const
        {
            auto it = m_l2r.find(key);
            if (it != m_l2r.end())
            {
                return it->second;
            }
            else
            {
                throw std::out_of_range("Right side key not found");
            }
        }

        bool getLeft(const R& key, L& value) const
        {
            auto it = m_r2l.find(key);
            if (it != m_r2l.end())
            {
                value = it->second;
                return true;
            }
            else
            {
                return false;
            }
        }

        bool getRight(const L& key, R& value) const
        {
            auto it = m_l2r.find(key);
            if (it != m_l2r.end())
            {
                value = it->second;
                return true;
            }
            else
            {
                return false;
            }
        }

        const std::map<L, R>& left = m_l2r;
        const std::map<R, L>& right = m_r2l;

    private:
        //std::deque<value_type> m_items;
        std::map<L, R> m_l2r;
        std::map<R, L> m_r2l;
    };

}
