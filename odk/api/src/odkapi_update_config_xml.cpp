// Copyright DEWETRON GmbH 2019
#include "odkapi_update_config_xml.h"

#include "odkapi_version_xml.h"
#include "odkbase_basic_values.h"
#include "odkuni_assert.h"

#include "odkuni_xpugixml.h"

#include <limits>
#include <set>

namespace odk
{
    UpdateConfigTelegram::ChannelConfig& UpdateConfigTelegram::addChannel(std::uint32_t local_id)
    {
        auto ch = PluginChannelInfo();
        ch.m_local_id = local_id;

        m_channel_configs.push_back(ChannelConfig(ch));
        return m_channel_configs.back();
    }

    void UpdateConfigTelegram::removeChannel(std::uint32_t local_id)
    {
        for (auto it = m_channel_configs.begin(); it != m_channel_configs.end(); ++it)
        {
            if (it->m_channel_info.m_local_id == local_id)
            {
                m_channel_configs.erase(it);
                break;
            }
        }
    }

    const UpdateConfigTelegram::ChannelConfig* UpdateConfigTelegram::getChannel(std::uint32_t local_id) const
    {
        for (size_t i = 0; i < m_channel_configs.size(); ++i)
        {
            if (m_channel_configs[i].m_channel_info.m_local_id == local_id)
            {
                return &m_channel_configs[i];
            }
        }
        return nullptr;
    }

    std::vector<UpdateConfigTelegram::ChannelConfig> UpdateConfigTelegram::getAllChannels() const
    {
        return m_channel_configs;
    }

    UpdateConfigTelegram::ChannelConfig* UpdateConfigTelegram::getChannel(std::uint32_t local_id)
    {
        for (size_t i = 0; i < m_channel_configs.size(); ++i)
        {
            if (m_channel_configs[i].m_channel_info.m_local_id == local_id)
            {
                return &m_channel_configs[i];
            }
        }
        return nullptr;
    }

    bool UpdateConfigTelegram::parse(const boost::string_view& xml_string)
    {
        m_channel_configs.clear();

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            auto request_node = doc.document_element();
            if (std::strcmp(request_node.name(), "UpdateConfig") != 0)
                return false;
            auto version = odk::getProtocolVersion(request_node);
            if (version != odk::Version(1, 0))
                return false;

            for (const auto channel_node : request_node.children())
            {
                if (std::strcmp(channel_node.name(), "Channel") == 0)
                {
                    std::uint32_t local_id = channel_node.attribute("local_id").as_uint(std::numeric_limits<uint32_t>::max());
                    if (local_id != std::numeric_limits<uint32_t>::max())
                    {
                        auto& ch = addChannel(local_id);
                        ch.readProperties(channel_node);
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    std::string UpdateConfigTelegram::generate() const
    {
        pugi::xml_document doc;
        auto request_node = doc.append_child("UpdateConfig");
        odk::setProtocolVersion(request_node, odk::Version(1, 0));

        for (const auto& ch : m_channel_configs)
        {
            auto channel_node = request_node.append_child("Channel");

            channel_node.append_attribute("local_id").set_value(ch.m_channel_info.m_local_id);

            ch.appendProperties(channel_node);
        }
        return xpugi::toXML(doc);
    }

    bool UpdateConfigTelegram::operator==(const UpdateConfigTelegram& other) const
    {
        return m_channel_configs == other.m_channel_configs;
    }

    void UpdateConfigTelegram::update(const UpdateConfigTelegram &updates)
    {
        for(const auto& channel_update : updates.m_channel_configs)
        {
            auto org_channel = getChannel(channel_update.m_channel_info.m_local_id);

            if(!org_channel)
            {
                continue;
            }

            for(const auto& property_update : channel_update.m_properties)
            {
                if(auto org_property = org_channel->getProperty(property_update.getName()))
                {
                    *org_property = property_update;
                }
            }
        }
    }

/*
    ConfigHelper::ConfigHelper(IfHost *host)
        : m_host(host)
    {
    }

    uint64_t ConfigHelper::pluginMessage(PluginMessageId id, uint64_t key, const IfValue *param, const IfValue **ret)
    {
        switch (id)
        {
        case odk::plugin_msg_sync::PLUGIN_CONFIGURATION_CHANGE_REQUEST:
            handleConfigChangeRequest(param);
            return odk::error_codes::OK;
        default:
            return odk::error_codes::NOT_IMPLEMENTED;
        }
    }

    void ConfigHelper::setConfigToHost()
    {
        auto xml_msg = m_host->createValue<odk::IfXMLValue>();
        xml_msg->set(m_info.generate().c_str());
        m_host->messageSync(odk::host_msg::SET_PLUGIN_CONFIGURATION, 0, xml_msg.get(), nullptr);
    }

    void ConfigHelper::handleConfigChangeRequest(const IfValue *param)
    {
        if (auto change_xml = value_cast<const odk::IfXMLValue>(param))
        {
            UpdateConfigTelegram update_telegram;
            if (update_telegram.parse(change_xml->getValue()))
            {
                m_info.update(update_telegram);
                onConfigChanged(update_telegram);
            }
            setConfigToHost();
        }
    }
*/

    void UpdateConfigTelegram::ChannelConfig::appendProperties(pugi::xml_node channel_node) const
    {
        std::set<std::string> saved_props;
        for (const auto& prop : m_properties)
        {
            saved_props.insert(prop.getName());
            auto prop_node = prop.appendTo(channel_node);
            auto it = std::find_if(m_constraints.begin(), m_constraints.end(),
                [&prop](const ConstraintsMap_t::value_type& c)
                {
                    return c.first == prop.getName();
                }
            );
            if (it != m_constraints.end())
            {
                auto constraints_node = prop_node.append_child("Constraints");
                const auto& constraints = it->second;
                for (const auto& constraint : constraints)
                {
                    constraint.appendTo(constraints_node);
                }
            }
        }

        for (const auto& constraint : m_constraints)
        {
            if (saved_props.find(constraint.first) == saved_props.end())
            {
                ODK_ASSERT_FAIL("Orphaned constraints are not supported");
            }
        }
    }

    UpdateConfigTelegram::Constraint UpdateConfigTelegram::Constraint::fromXML(const pugi::xml_node& tree)
    {
        const std::string element_name(tree.name());

        if (element_name == "DoubleRangeConstraint")
        {
            odk::Property minv, maxv;
            minv.setValue(tree.attribute("min").as_double());
            maxv.setValue(tree.attribute("max").as_double());

            return makeRange(minv, maxv);
        }
        else if (element_name == "OptionConstraint")
        {
            std::vector<odk::Property> options;
            for (auto child : tree.children())
            {
                if (child.type() == pugi::node_element)
                {
                    odk::Property prop;
                    if (prop.readValue(child, {}))
                    {
                        options.push_back(prop);
                    }
                }
            }
            return makeOptions(options);
        }
        else if (element_name == "StringConstraint")
        {
            return makeSimpleConstraint(Constraint::ARBITRARY_STRING);
        }
        else if (element_name == "ChannelIdsConstraint")
        {
            auto max_items = tree.attribute("max_items").as_uint(0);
            int max_dimension = tree.attribute("max_dimension").as_int(-1);
            const std::string type = tree.attribute("channel_type").as_string("ALL");
            return makeChannelIds(max_items, max_dimension, type);
        }
        else if (element_name == "RegularExpressionConstraint")
        {
            std::string expr = tree.attribute("expression").as_string();
            return makeRegEx(expr);
        }
        else if (element_name == "VisibilityConstraint")
        {
            std::string vis = tree.attribute("visibility").as_string();
            return makeVisibility(vis);
        }
        else if (element_name == "FilePathConstraint")
        {
            std::string ft = tree.attribute("file_type").as_string();
            std::string dialog_title = tree.attribute("dialog_title").as_string();
            std::string default_path = tree.attribute("default_path").as_string();

            auto child = tree.child("NameFilters");
            std::vector<std::string> filters;
            if (child != nullptr)
            {
                for (auto& filter_node : child.children("Filter"))
                {
                    std::string filter = filter_node.attribute("name").as_string();
                    filters.emplace_back(filter);
                }
            }
            bool multi_select = tree.attribute("multi_select").as_bool();
            return makeFilePathConstraint(ft, dialog_title, default_path, filters, multi_select);
        }

        return {};
    }

    void UpdateConfigTelegram::Constraint::appendTo(pugi::xml_node parent) const
    {
        switch (getType())
        {
            case Constraint::OPTIONS:
            {
                auto constraint_node = parent.append_child("OptionConstraint");

                const auto& options = getOptions();

                for (const auto& option : options)
                {
                    option.appendValue(constraint_node);
                }
            } break;
            case Constraint::RANGE:
            {
                auto pmin = getRangeMin();
                auto pmax = getRangeMax();
                if (pmin.getType() == odk::Property::FLOATING_POINT_NUMBER)
                {
                    auto constraint_node = parent.append_child("DoubleRangeConstraint");
                    constraint_node.append_attribute("min").set_value(pmin.getDoubleValue());
                    constraint_node.append_attribute("max").set_value(pmax.getDoubleValue());
                }
                else
                {
                    ODK_ASSERT_FAIL("Unimplemented range type");
                }
            } break;
            case Constraint::ARBITRARY_STRING:
            {
                parent.append_child("StringConstraint");
            } break;
            case Constraint::CHANNEL_IDS:
            {
                auto constraint_node = parent.append_child("ChannelIdsConstraint");
                constraint_node.append_attribute("max_items").set_value(getMaxItems());
                constraint_node.append_attribute("max_dimension").set_value(getMaxDimension());
                constraint_node.append_attribute("channel_type").set_value(getChannelType().c_str());
            } break;
            case Constraint::REGEX:
            {
                auto constraint_node = parent.append_child("RegularExpressionConstraint");
                constraint_node.append_attribute("expression").set_value(getRegEx().c_str());
            } break;
            case Constraint::VISIBLITY:
            {
                auto constraint_node = parent.append_child("VisibilityConstraint");
                constraint_node.append_attribute("visibility").set_value(getVisibility().c_str());
            } break;
            case Constraint::FILE_PATH:
            {
                auto constraint_node = parent.append_child("FilePathConstraint");
                constraint_node.append_attribute("file_type").set_value(getFileType().c_str());
                constraint_node.append_attribute("dialog_title").set_value(getDialogTitle().c_str());
                constraint_node.append_attribute("default_path").set_value(getDefaultPath().c_str());

                auto nam_flt_node = constraint_node.append_child("NameFilters");
                for (const auto& filter : getNameFilters().m_values)
                {
                    auto flt_node = nam_flt_node.append_child("Filter");
                    flt_node.append_attribute("name").set_value(filter.c_str());
                }
                constraint_node.append_attribute("multi_select").set_value(getMultiSelect());
            } break;
            default:
                ODK_ASSERT_FAIL("Unimplemented constraint type");
                break;
        }
    }

    bool UpdateConfigTelegram::ChannelConfig::readProperties(pugi::xml_node channel_node)
    {

        for (const auto property_node : channel_node.children("Property"))
        {
            std::string prop_name = property_node.attribute("name").as_string();
            if (prop_name.empty())
            {
                return false;
            }

            for (const auto property_child_node : property_node.children())
            {
                if (property_child_node.type() != pugi::node_element)
                {
                    //skip
                }
                if (std::strcmp(property_child_node.name(), "Constraints") == 0)
                {
                    ConstraintVec_t entries;
                    for (pugi::xml_node constraint_node : property_child_node.children())
                    {
                        auto constraint = Constraint::fromXML(constraint_node);
                        entries.push_back(constraint);
                    }
                    m_constraints[prop_name] = entries;
                }
                else
                {
                    Property prop(prop_name);
                    if (prop.readValue(property_child_node, Version()))
                    {
                         addProperty(prop);
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    UpdateConfigTelegram::Constraint makeRangeConstraint(double low, double high)
    {
        //return UpdateConfigTelegram::Constraint::makeRange(neoncfg::Scalar(low, ""), neoncfg::Scalar(high, ""));
        odk::Property pmin, pmax;
        pmin.setValue(low);
        pmax.setValue(high);
        return UpdateConfigTelegram::Constraint::makeRange(pmin, pmax);
    }

    UpdateConfigTelegram::Constraint makeArbitraryStringConstraint()
    {
        return UpdateConfigTelegram::Constraint::makeSimpleConstraint(UpdateConfigTelegram::Constraint::ARBITRARY_STRING);
    }

    UpdateConfigTelegram::Constraint makeRegExConstraint(const std::string& regex)
    {
        return UpdateConfigTelegram::Constraint::makeRegEx(regex);
    }

    UpdateConfigTelegram::Constraint makeChannelIdsConstraint(std::uint32_t max_items, int max_dimension, const std::string& type)
    {
        return UpdateConfigTelegram::Constraint::makeChannelIds(max_items, max_dimension, type);
    }

    UpdateConfigTelegram::Constraint makeVisiblityConstraint(const std::string& vis)
    {
        return UpdateConfigTelegram::Constraint::makeVisibility(vis);
    }

    UpdateConfigTelegram::Constraint makeFilePathConstraint(std::string& file_type,
        const std::string& dialog_title, const std::string& default_path,
        const std::vector<std::string>& name_filters, bool multi_select)
    {
        return UpdateConfigTelegram::Constraint::makeFilePathConstraint(
            file_type, dialog_title, default_path, name_filters, multi_select);
    }
}
