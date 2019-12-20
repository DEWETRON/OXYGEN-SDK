// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_property_xml.h"
#include "odkapi_property_list_xml.h"

#include "odkbase_if_host.h"

#include <map>
#include <string>
#include <vector>

namespace odk
{
    /**
     * Helper class for generating config-item and constraints plugin-messages
     */
    struct UpdateConfigTelegram
    {
    public:
        struct PluginChannelInfo
        {
            std::uint32_t m_local_id = -1;

            explicit PluginChannelInfo(std::uint32_t local_id = -1)
                : m_local_id(local_id)
            {
            }

            bool operator==(const PluginChannelInfo& other) const
            {
                return m_local_id == other.m_local_id;
            }

        };

        struct Constraint
        {
        public:
            enum Type
            {
                UNKNOWN,
                OPTIONS,
                RANGE,
                REGEX,
                ARBITRARY_STRING,
                CHANNEL_IDS,
                VISIBLITY,
            };

            static Constraint makeOption(const odk::Property& value)
            {
                Constraint r(OPTIONS);
                auto prop = value;
                prop.setName("option");
                r.m_params.append(prop);
                return r;
            }

            static Constraint makeOptions(const std::vector<odk::Property>& options)
            {
                Constraint r(OPTIONS);
                for (const auto& option : options)
                {
                    auto prop = option;
                    prop.setName("option");
                    r.m_params.append(prop);
                }
                return r;
            }

            static Constraint makeRange(const odk::Scalar& minv, const odk::Scalar& maxv)
            {
                Constraint r(RANGE);
                r.m_params.setProperty(odk::Property("min", minv));
                r.m_params.setProperty(odk::Property("max", maxv));
                return r;
            }

            static Constraint makeRange(odk::Property minv, odk::Property maxv)
            {
                if (minv.getType() != maxv.getType())
                {
                    return {};
                }
                Constraint r(RANGE);
                minv.setName("min");
                maxv.setName("max");
                r.m_params.setProperty(minv);
                r.m_params.setProperty(maxv);
                return r;
            }

            static Constraint makeRegEx(const std::string& regex)
            {
                Constraint r(REGEX);
                r.m_params.setProperty(odk::Property("regex", regex));
                return r;
            }

            static Constraint makeSimpleConstraint(Type t)
            {
                switch (t)
                {
                    case ARBITRARY_STRING:
                        return Constraint(t);
                    default:
                        throw std::runtime_error("type does not indicate a simple constraint");
                }
            }

            // max == 1 means single channel id item for now
            static Constraint makeChannelIds(std::uint32_t max)
            {
                Constraint r(CHANNEL_IDS);
                odk::Property max_items_prop;
                max_items_prop.setName("max_items");
                max_items_prop.setValue(max);
                r.m_params.setProperty(max_items_prop);
                return r;
            }

            static Constraint makeVisibility(const std::string& visibility)
            {
                Constraint r(VISIBLITY);
                r.m_params.setProperty(odk::Property("visibility", visibility));
                return r;
            }

            static Constraint fromXML(const pugi::xml_node& tree);

            void appendTo(pugi::xml_node node) const;

            Type getType() const
            {
                return m_type;
            }

            odk::Property getRangeMin() const
            {
                if (m_type != RANGE)
                {
                    throw std::runtime_error("constraint is not of type range");
                }
                return m_params.getPropertyByName("min");
            }

            odk::Property getRangeMax() const
            {
                if (m_type != RANGE)
                {
                    throw std::runtime_error("constraint is not of type range");
                }
                return m_params.getPropertyByName("max");
            }

            std::vector<odk::Property> getOptions() const
            {
                std::vector<odk::Property> r;
                for (size_t i = 0; i < m_params.size(); ++i)
                {
                    auto prop = m_params.getProperty(i);
                    if (prop.getName() == "option")
                    {
                        r.push_back(prop);
                    }
                }
                return r;
            }

            odk::Property getOption(size_t n) const
            {
                auto i = n;
                for (size_t i = 0; i < m_params.size(); ++i)
                {
                    auto prop = m_params.getProperty(i);
                    if (prop.getName() == "option")
                    {
                        if (i)
                        {
                            --i;
                        }
                        else
                        {
                            return prop;
                        }
                    }
                }
                return {};
            }

            std::string getRegEx() const
            {
                if (m_type != REGEX)
                {
                    throw std::runtime_error("constraint is not of type regex");
                }
                return m_params.getPropertyByName("regex").getStringValue();
            }

            // supported values: HIDDEN, PUBLIC
            std::string getVisibility() const
            {
                if (m_type != VISIBLITY)
                {
                    throw std::runtime_error("constraint is not of type visibility");
                }
                return m_params.getPropertyByName("visibility").getStringValue();
            }

            Constraint()
                : m_type(UNKNOWN)
            {
            }

            std::uint32_t getMaxItems() const
            {
                if (m_type != CHANNEL_IDS)
                {
                    throw std::runtime_error("constraint is not of type channel_ids");
                }
                return m_params.getPropertyByName("max_items").getUnsignedIntValue();
            }

            bool operator==(const Constraint& other) const
            {
                return m_type == other.m_type
                    && m_params == other.m_params;
            }

        protected:

            Constraint(Type type)
                : m_type(type)
            {
            }

        private:
            Type m_type;
            odk::PropertyList m_params;
        };

        typedef std::vector<Constraint> ConstraintVec_t;
        typedef std::vector<odk::Property> ConfigItemVec_t;
        typedef std::map<std::string, ConstraintVec_t> ConstraintsMap_t;

        struct ChannelConfig
        {
            ChannelConfig(const PluginChannelInfo& p)
                : m_channel_info(p)
                , m_properties()
            {}

            ChannelConfig& addProperty(const odk::Property& prop)
            {
                m_properties.push_back(prop);
                return *this;
            }

            template <class T>
            ChannelConfig& addProperty(const std::string& name, T value)
            {
                m_properties.push_back(odk::Property(name, value));
                return *this;
            }

            const odk::Property* getProperty(const std::string& name) const
            {
                for (size_t i = 0; i < m_properties.size(); ++i)
                {
                    if (m_properties[i].getName() == name)
                    {
                        return &m_properties[i];
                    }
                }
                return nullptr;
            }

            odk::Property* getProperty(const std::string& name)
            {
                return const_cast<odk::Property*>(
                    const_cast<const ChannelConfig*>(this)->getProperty(name));
            }

            ChannelConfig& updateProperty(const odk::Property& prop)
            {
                auto our_prop = getProperty(prop.getNodeName());
                if (our_prop)
                {
                    *our_prop = prop;
                }
                return *this;
            }

            template <class T>
            ChannelConfig& updateProperty(const std::string& name, odk::Property::Type type, T value)
            {
                auto our_prop = getProperty(name);
                if (our_prop)
                {
                    *our_prop = odk::Property(name, type, value);
                }
                return *this;
            }

            ChannelConfig& addConstraint(const std::string& name, const Constraint& constraint)
            {
                m_constraints[name].push_back(constraint);
                return *this;
            }

            ChannelConfig& addOptionConstraint(const std::string& name, const odk::Property& value)
            {
                addConstraint(name, Constraint::makeOption(value));
                return *this;
            }

            ConstraintVec_t getConstraints(const std::string& name) const
            {
                auto it = m_constraints.find(name);
                if (it != m_constraints.end())
                {
                    return it->second;
                }
                else
                {
                    return {};
                }
            }

            void appendProperties(pugi::xml_node channel_node) const;
            bool readProperties(pugi::xml_node channel_node);

            bool operator==(const ChannelConfig& other) const
            {
                return m_channel_info == other.m_channel_info
                    && m_properties == other.m_properties
                    && m_constraints == other.m_constraints;
            }

            PluginChannelInfo m_channel_info;
            ConfigItemVec_t m_properties;
            ConstraintsMap_t m_constraints;
        };

        std::vector<ChannelConfig> m_channel_configs;

        UpdateConfigTelegram::ChannelConfig& addChannel(std::uint32_t local_id);
        void removeChannel(std::uint32_t local_id);
        UpdateConfigTelegram::ChannelConfig* getChannel(std::uint32_t local_id);
        const UpdateConfigTelegram::ChannelConfig* getChannel(std::uint32_t local_id) const;
        std::vector<UpdateConfigTelegram::ChannelConfig> getAllChannels() const;

        bool parse(const char* xml_string);
        std::string generate() const;
        bool operator==(const UpdateConfigTelegram& other) const;

        void update(const UpdateConfigTelegram& updates);
    };

    template <>
    inline UpdateConfigTelegram::ChannelConfig& UpdateConfigTelegram::ChannelConfig::addProperty(const std::string& name, unsigned int value)
    {
        odk::Property p(name);
        p.setValue(value);
        m_properties.push_back(p);
        return *this;
    }

    UpdateConfigTelegram::Constraint makeRangeConstraint(double low, double high);
    UpdateConfigTelegram::Constraint makeArbitraryStringConstraint();
    UpdateConfigTelegram::Constraint makeRegExConstraint(const std::string& regex);
    UpdateConfigTelegram::Constraint makeChannelIdsConstraint(std::uint32_t max);
    UpdateConfigTelegram::Constraint makeVisiblityConstraint(const std::string& vis);
}
