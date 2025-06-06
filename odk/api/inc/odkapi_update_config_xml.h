// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_property_xml.h"
#include "odkapi_property_list_xml.h"

#include "odkbase_if_host.h"
#include "odkuni_defines.h"

#include <map>
#include <string>
#include <string_view>
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
            std::uint32_t m_local_id = static_cast<std::uint32_t>(-1);

            explicit PluginChannelInfo(std::uint32_t local_id = -1)
                : m_local_id(local_id)
            {
            }

            ODK_NODISCARD bool operator==(const PluginChannelInfo& other) const
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
                FILE_PATH,
                ITEM_HINT,
            };

            ODK_NODISCARD static Constraint makeOption(const odk::Property& value)
            {
                Constraint r(OPTIONS);
                auto prop = value;
                prop.setName("option");
                r.m_params.append(prop);
                return r;
            }

            ODK_NODISCARD static Constraint makeOptions(const std::vector<odk::Property>& options)
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

            ODK_NODISCARD static Constraint makeRange(const odk::Scalar& minv, const odk::Scalar& maxv)
            {
                Constraint r(RANGE);
                r.m_params.setProperty(odk::Property("min", minv));
                r.m_params.setProperty(odk::Property("max", maxv));
                return r;
            }

            ODK_NODISCARD static Constraint makeRange(odk::Property minv, odk::Property maxv)
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

            ODK_NODISCARD static Constraint makeRegEx(const std::string& regex)
            {
                Constraint r(REGEX);
                r.m_params.setProperty(odk::Property("regex", regex));
                return r;
            }

            ODK_NODISCARD static Constraint makeSimpleConstraint(Type t)
            {
                switch (t)
                {
                    case ARBITRARY_STRING:
                        return Constraint(t);
                    default:
                        throw std::runtime_error("type does not indicate a simple constraint");
                }
            }

            // max_items == 1 means single channel id item for now
            ODK_NODISCARD static Constraint makeChannelIds(std::uint32_t max_items, int max_dimension, const std::string& type)
            {
                Constraint r(CHANNEL_IDS);
                r.m_params.setProperty(odk::Property("max_items", max_items));
                r.m_params.setProperty(odk::Property("max_dimension", max_dimension));
                r.m_params.setProperty(odk::Property("channel_type", type));
                return r;
            }

            ODK_NODISCARD static Constraint makeVisibility(const std::string& visibility)
            {
                Constraint r(VISIBLITY);
                r.m_params.setProperty(odk::Property("visibility", visibility));
                return r;
            }

            ODK_NODISCARD static Constraint makeItemHint(const std::string& item_hint)
            {
                Constraint r(ITEM_HINT);
                r.m_params.setProperty(odk::Property("item_hint", item_hint));
                return r;
            }

            ODK_NODISCARD static Constraint makeFilePathConstraint(std::string& file_type,
                const std::string& dialog_title, const std::string& default_path,
                const std::vector<std::string>& name_filters, bool multi_select)
            {
                Constraint r(FILE_PATH);
                r.m_params.setProperty(odk::Property("file_type", file_type, "DialogFileType"));
                r.m_params.setProperty(odk::Property("dialog_title", dialog_title));
                r.m_params.setProperty(odk::Property("default_path", default_path));
                StringList sl(name_filters);
                r.m_params.setProperty(odk::Property("name_filters", sl));
                r.m_params.setProperty(odk::Property("multi_select", multi_select));
                return r;
            }

            ODK_NODISCARD static Constraint fromXML(const pugi::xml_node& tree);

            void appendTo(pugi::xml_node node) const;

            ODK_NODISCARD Type getType() const
            {
                return m_type;
            }

            ODK_NODISCARD odk::Property getRangeMin() const
            {
                if (m_type != RANGE)
                {
                    throw std::runtime_error("constraint is not of type range");
                }
                return m_params.getPropertyByName("min");
            }

            ODK_NODISCARD odk::Property getRangeMax() const
            {
                if (m_type != RANGE)
                {
                    throw std::runtime_error("constraint is not of type range");
                }
                return m_params.getPropertyByName("max");
            }

            ODK_NODISCARD std::vector<odk::Property> getOptions() const
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

            ODK_NODISCARD odk::Property getOption(size_t n) const
            {
                auto i = n;
                for (size_t j = 0; j < m_params.size(); ++j)
                {
                    auto prop = m_params.getProperty(j);
                    if (prop.getName() == "option")
                    {
                        if (i > 0)
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

            ODK_NODISCARD std::string getRegEx() const
            {
                if (m_type != REGEX)
                {
                    throw std::runtime_error("constraint is not of type regex");
                }
                return m_params.getPropertyByName("regex").getStringValue();
            }

            // supported values: HIDDEN, PUBLIC
            ODK_NODISCARD std::string getVisibility() const
            {
                if (m_type != VISIBLITY)
                {
                    throw std::runtime_error("constraint is not of type visibility");
                }
                return m_params.getPropertyByName("visibility").getStringValue();
            }

            ODK_NODISCARD std::string getFileType() const
            {
                if (m_type != FILE_PATH)
                {
                    throw std::runtime_error("constraint is not of type file path");
                }
                return m_params.getPropertyByName("file_type").getStringValue();
            }

            ODK_NODISCARD std::string getDialogTitle() const
            {
                if (m_type != FILE_PATH)
                {
                    throw std::runtime_error("constraint is not of type file path");
                }
                return m_params.getPropertyByName("dialog_title").getStringValue();
            }

            ODK_NODISCARD std::string getDefaultPath() const
            {
                if (m_type != FILE_PATH)
                {
                    throw std::runtime_error("constraint is not of type file path");
                }
                return m_params.getPropertyByName("default_path").getStringValue();
            }

            ODK_NODISCARD StringList getNameFilters() const
            {
                if (m_type != FILE_PATH)
                {
                    throw std::runtime_error("constraint is not of type file path");
                }
                return m_params.getPropertyByName("name_filters").getStringListValue();
            }

            ODK_NODISCARD bool getMultiSelect() const
            {
                if (m_type != FILE_PATH)
                {
                    throw std::runtime_error("constraint is not of type file path");
                }
                return m_params.getPropertyByName("multi_select").getBoolValue();
            }

            ODK_NODISCARD std::string getItemHint() const
            {
                if (m_type != ITEM_HINT)
                {
                    throw std::runtime_error("constraint is not of type item_hint");
                }
                return m_params.getPropertyByName("item_hint").getStringValue();
            }

            Constraint()
                : m_type(UNKNOWN)
            {
            }

            ODK_NODISCARD std::uint32_t getMaxItems() const
            {
                if (m_type != CHANNEL_IDS)
                {
                    throw std::runtime_error("constraint is not of type channel_ids");
                }
                return m_params.getPropertyByName("max_items").getUnsignedIntValue();
            }

            ODK_NODISCARD int getMaxDimension() const
            {
                if (m_type != CHANNEL_IDS)
                {
                    throw std::runtime_error("constraint is not of type channel_ids");
                }
                return m_params.getPropertyByName("max_dimension").getIntValue();
            }

            ODK_NODISCARD std::string getChannelType() const
            {
                if (m_type != CHANNEL_IDS)
                {
                    throw std::runtime_error("constraint is not of type channel_ids");
                }
                return m_params.getPropertyByName("channel_type").getStringValue();
            }

            ODK_NODISCARD bool operator==(const Constraint& other) const
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

            ODK_NODISCARD const odk::Property* getProperty(const std::string& name) const
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

            ODK_NODISCARD odk::Property* getProperty(const std::string& name)
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

            ODK_NODISCARD ConstraintVec_t getConstraints(const std::string& name) const
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

            ODK_NODISCARD bool operator==(const ChannelConfig& other) const
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
        ODK_NODISCARD UpdateConfigTelegram::ChannelConfig* getChannel(std::uint32_t local_id);
        ODK_NODISCARD const UpdateConfigTelegram::ChannelConfig* getChannel(std::uint32_t local_id) const;
        ODK_NODISCARD std::vector<UpdateConfigTelegram::ChannelConfig> getAllChannels() const;

        bool parse(const std::string_view& xml_string);
        ODK_NODISCARD std::string generate() const;
        ODK_NODISCARD bool operator==(const UpdateConfigTelegram& other) const;

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

    ODK_NODISCARD UpdateConfigTelegram::Constraint makeRangeConstraint(double low, double high);
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeArbitraryStringConstraint();
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeRegExConstraint(const std::string& regex);
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeChannelIdsConstraint(std::uint32_t max_items, int max_dimension = -1, const std::string& type_filter = "ALL");
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeVisiblityConstraint(const std::string& vis);
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeFilePathConstraint(std::string& file_type,
        const std::string& dialog_title, const std::string& path,
        const std::vector<std::string>& name_filters, bool multi_select);
    ODK_NODISCARD UpdateConfigTelegram::Constraint makeItemHintConstraint(const std::string& item_hint);
}
