// Copyright DEWETRON GmbH 2020

#include "odkapi_export_xml.h"

#include "odkapi_error_codes.h"
#include "odkapi_version_xml.h"

#include "odkuni_bimap.h"
#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <cstring>
#include <map>
#include <vector>

namespace
{
    typedef odk::SimpleBiMap<odk::RegisterExport::StartExportAction, std::string> StartExportActionBiMap;
    static const StartExportActionBiMap START_EXPORT_OPTION_MAP = {
        {odk::RegisterExport::StartExportAction::SELECT_FILE, "SELECT_FILE"},
        {odk::RegisterExport::StartExportAction::SELECT_DIRECTORY, "SELECT_DIRECTORY"},
        {odk::RegisterExport::StartExportAction::NONE, "NONE"},
    };

    using ValidationMessageSeveityMap = odk::SimpleBiMap <odk::ValidationMessage::Severity, std::string>;
    static const ValidationMessageSeveityMap SEVERITY_MAP = {
        {odk::ValidationMessage::Severity::DEFAULT, "DEFAULT"},
        {odk::ValidationMessage::Severity::VALIDATION_INFO, "INFO"},
        {odk::ValidationMessage::Severity::VALIDATION_WARNING, "WARNING"},
        {odk::ValidationMessage::Severity::VALIDATION_ERROR, "ERROR"},
    };

    void appendMessagesTo(pugi::xml_node& parent, const std::vector<odk::ValidationMessage>& messages)
    {
        if (!messages.empty())
        {
            auto msgs_node = parent.append_child("Messages");
            for (const auto& msg : messages)
            {
                auto msg_node = msgs_node.append_child("Message");
                xpugi::setText(msg_node.append_child("Severity"), SEVERITY_MAP.left.at(msg.m_severity));
                xpugi::setText(msg_node.append_child("Text"), msg.m_message);
            }
        }
    }

    std::vector<odk::ValidationMessage>parseMessagesFrom(const pugi::xml_node& parent)
    {
        std::vector<odk::ValidationMessage> messages;
        for (auto msg : parent.select_nodes("Messages/Message"))
        {
            auto severity_txt = xpugi::getText(msg.node().child("Severity"));
            auto txt = xpugi::getText(msg.node().child("Text"));
            try
            {
                auto s = SEVERITY_MAP.right.at(severity_txt);
                messages.emplace_back() = { s, txt };
            }
            catch (const std::logic_error&)
            {
            }
        }
        return messages;
    }

}

namespace odk
{
    RegisterExport::RegisterExport()
        : m_start_export_action(SELECT_FILE)
    {}

    bool RegisterExport::parse(const std::string_view& xml_string)
    {
        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status != pugi::status_ok)
        {
            return false;
        }

        auto node = doc.document_element();
        if (!node)
        {
            return false;
        }

        if (!odk::strequal(node.name(), "RegisterExporter"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1,0))
        {
            return false;
        }

        m_format_name = xpugi::getText(xpugi::getChildNodeByTagName(node, "FormatName"));
        m_format_id = xpugi::getText(xpugi::getChildNodeByTagName(node, "FormatId"));
        m_file_extension = xpugi::getText(xpugi::getChildNodeByTagName(node, "FileExtension"));

        const auto action_node = xpugi::getChildElementByTagName(node, "StartAction");
        if (!action_node.empty())
        {
            m_start_export_action = START_EXPORT_OPTION_MAP.right.at(xpugi::getText(action_node));
        }

        auto ui_small_node = node.select_node("UISmall").node();
        if (ui_small_node)
        {
            m_ui_item_small = xpugi::getText(xpugi::getChildNodeByTagName(ui_small_node, "ItemName"));
        }
        else
        {
            m_ui_item_small.clear();
        }

        auto ui_full_node = node.select_node("UIFull").node();
        if (ui_full_node)
        {
            m_ui_item_full = xpugi::getText(xpugi::getChildNodeByTagName(ui_full_node, "ItemName"));
        }
        else
        {
            m_ui_item_full.clear();
        }

        return true;
    }

    std::string RegisterExport::generate() const
    {
        pugi::xml_document doc;

        auto register_elem = xpugi::xml_element(doc.append_child("RegisterExporter"));
        odk::setProtocolVersion(register_elem, odk::Version(1,0));


        xpugi::setText(xpugi::xml_element(register_elem.append_child("FormatName")), m_format_name);
        xpugi::setText(xpugi::xml_element(register_elem.append_child("FormatId")), m_format_id);
        xpugi::setText(xpugi::xml_element(register_elem.append_child("FileExtension")), m_file_extension);
        xpugi::setText(xpugi::xml_element(register_elem.append_child("StartAction")), START_EXPORT_OPTION_MAP.left.at(m_start_export_action));

        if (!m_ui_item_small.empty())
        {
            auto ui_small_node = register_elem.append_child("UISmall");
            xpugi::setText(xpugi::xml_element(ui_small_node.append_child("ItemName")), m_ui_item_small);
        }

        if (!m_ui_item_full.empty())
        {
            auto ui_full_node = register_elem.append_child("UIFull");
            xpugi::setText(xpugi::xml_element(ui_full_node.append_child("ItemName")), m_ui_item_full);
        }

        return xpugi::toXML(doc);
    }



    StartExport::StartExport()
        : m_transaction_id()
        , m_properties()
    {

    }

    bool StartExport::parse(const std::string_view& xml_string)
    {
        m_transaction_id = 0;
        m_properties = {};

        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{

                auto export_properties_node = doc.document_element().child("ExportProperties");
                if (!m_properties.parse(export_properties_node))
                {
                    return false;
                }

                auto transaction_id_node = doc.document_element().child("TransactionId");
                if (transaction_id_node)
                {
                    m_transaction_id = odk::from_string<std::uint64_t>(transaction_id_node.attribute("transaction_id").value());
                }
                else
                {
                    return false;
                }

            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string StartExport::generate() const
    {
        pugi::xml_document doc;
        auto start_export_node = doc.append_child("StartExport");

        auto transaction_id_node = start_export_node.append_child("TransactionId");
        transaction_id_node.append_attribute("transaction_id").set_value(m_transaction_id);

        std::string properties = m_properties.generate();
        pugi::xml_document props_doc;
#if ODK_CPLUSPLUS >= 201703L
        props_doc.load_buffer_inplace(properties.data(), properties.size(), pugi::parse_default, pugi::encoding_utf8);
#else
        props_doc.load_buffer(properties.data(), properties.size(), pugi::parse_default, pugi::encoding_utf8);
#endif
        start_export_node.append_copy(props_doc.document_element());

        return xpugi::toXML(doc);
    }

    ExportProperties::ExportProperties()
        : m_channels()
          , m_export_intervals()
          , m_format_id()
          , m_custom_properties()
    {
    }

    bool ExportProperties::parse(const std::string_view& xml_string)
    {
        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            return parse(doc.document_element());
        }
        else
        {
            return false;
        }
    }

    bool ExportProperties::parse(pugi::xml_node export_properties_node)
    {
        m_channels.clear();
        m_export_intervals.clear();
        m_custom_properties.clear();
        m_filename.clear();
        m_format_id.clear();

        if (auto main_channel_node = export_properties_node.child("Channels"))
        {
            auto channel_nodes = main_channel_node.children("Channel");
            for (auto channel_node : channel_nodes)
            {
                auto a_ch_id = channel_node.attribute("channel_id").as_ullong();
                m_channels.push_back(a_ch_id);
            }
        }

        if (auto main_interval_node = export_properties_node.child("Intervals"))
        {
            auto window_nodes = main_interval_node.children("Interval");
            for (auto window_node : window_nodes)
            {
                auto a_begin = window_node.attribute("begin").as_double();
                auto a_end = window_node.attribute("end").as_double();
                m_export_intervals.emplace_back(a_begin, a_end);
            }
        }

        if (auto special_main_node = export_properties_node.child("CustomProperties"))
        {
            for (const auto& prop : special_main_node.children())
            {
                odk::Property p;
                if (p.readFrom(prop, {1, 0}))
                {
                    m_custom_properties.setProperty(p);
                }
            }
        }

        if (auto format_id_node = export_properties_node.child("FormatId"))
        {
            m_format_id = std::string(format_id_node.attribute("format").value());
        }

        if (auto filename_node = export_properties_node.child("Filename"))
        {
            m_filename = std::string(filename_node.attribute("name").value());
        }

        return true;
    }

    std::string ExportProperties::generate() const
    {
        pugi::xml_document doc;
        auto export_props_node = doc.append_child("ExportProperties");

        auto channels_node = export_props_node.append_child("Channels");
        for (const auto& channel : m_channels)
        {
            auto channel_node = channels_node.append_child("Channel");
            channel_node.append_attribute("channel_id").set_value(channel);
        }

        auto intervals_node = export_props_node.append_child("Intervals");
        for (const auto& interval : m_export_intervals)
        {
            auto interval_node = intervals_node.append_child("Interval");
            interval_node.append_attribute("begin").set_value(interval.m_begin);
            interval_node.append_attribute("end").set_value(interval.m_end);
        }

        auto custom_settings_node = export_props_node.append_child("CustomProperties");
        for (const auto& setting : m_custom_properties)
        {
            setting.appendTo(custom_settings_node);
        }

        auto format_id_node = export_props_node.append_child("FormatId");
        format_id_node.append_attribute("format").set_value(m_format_id.c_str());

        auto filename_node = export_props_node.append_child("Filename");
        filename_node.append_attribute("name").set_value(m_filename.c_str());

        return xpugi::toXML(doc);
    }

    std::string ExportProperties::generateNodeXML() const
    {
        std::string serialized = generate();
        auto end_pi_tag = serialized.find("?>");
        if (end_pi_tag != std::string::npos)
        {
            return serialized.substr(end_pi_tag + 2);
        }
        return serialized;
    }

    ValidateExport::ValidateExport()
        : m_properties()
    {
    }

    bool ValidateExport::parse(const std::string_view& xml_string)
    {
        m_properties = {};

        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{
                auto export_properties_node = doc.document_element().child("ExportProperties");
                if (!m_properties.parse(export_properties_node))
                {
                    return false;
                }
            }
            catch (...)
            {
                return false;
            }
        }
        return true;
    }

    std::string ValidateExport::generate() const
    {
        pugi::xml_document doc;
        auto start_export_node = doc.append_child("ValidateExportSettings");

        std::string properties = m_properties.generate();
        pugi::xml_document props_doc;
#if ODK_CPLUSPLUS >= 201703L
        props_doc.load_buffer_inplace(properties.data(), properties.size(), pugi::parse_default, pugi::encoding_utf8);
#else
        props_doc.load_buffer(properties.data(), properties.size(), pugi::parse_default, pugi::encoding_utf8);
#endif
        start_export_node.append_copy(props_doc.document_element());

        return xpugi::toXML(doc);
    }

    ChannelError::ChannelError(std::uint64_t channel_id, std::uint64_t error_code)
        : channel_id(channel_id)
        , error_code(error_code)
        , error_message()
    {}

    ChannelError::ChannelError(std::uint64_t channel_id, std::uint64_t error_code, const std::string& error_message)
        : channel_id(channel_id)
        , error_code(error_code)
        , error_message(error_message)
    {}


    std::string ValidationMessage::toString(Severity s)
    {
        return SEVERITY_MAP.left.at(s);
    }


    ValidateExportResponse::ValidateExportResponse()
        : m_success(false)
    {
    }

    bool ValidateExportResponse::parse(const std::string_view& xml_string)
    {
        m_success = false;
        m_channel_errors.clear();

        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{
                if (auto success_node = doc.child("ValidationSuccess"))
                {
                    m_success = true;
                    for (auto ch : success_node.select_nodes("Channels/Channel"))
                    {
                        auto channel_id = xpugi::getText(ch.node().child("Id"));
                        auto channel_code = xpugi::getText(ch.node().child("ErrorCode"));
                        auto channel_msg = xpugi::getText(ch.node().child("ErrorMessage"));

                        try
                        {
                            auto channel_id_num = odk::from_string<std::uint64_t>(channel_id);
                            auto channel_code_num = odk::from_string<std::uint64_t>(channel_code);
                            m_channel_warnings.emplace_back(channel_id_num, channel_code_num, channel_msg);
                        }
                        catch (const std::logic_error&)
                        {
                            return false;
                        }
                    }

                    m_messages = parseMessagesFrom(success_node);
                    return true;
                }
                else if (auto failure_node = doc.child("ValidationFailed"))
                {
                    m_success = false;
                    for (auto ch : failure_node.select_nodes("Channels/Channel"))
                    {
                        auto channel_id = xpugi::getText(ch.node().child("Id"));
                        auto channel_code = xpugi::getText(ch.node().child("ErrorCode"));
                        auto channel_msg = xpugi::getText(ch.node().child("ErrorMessage"));

                        try
                        {
                            auto channel_id_num = odk::from_string<std::uint64_t>(channel_id);
                            auto channel_code_num = odk::from_string<std::uint64_t>(channel_code);
                            m_channel_errors.emplace_back(channel_id_num, channel_code_num, channel_msg);
                        }
                        catch(const std::logic_error&)
                        {
                            return false;
                        }
                    }

                    m_messages = parseMessagesFrom(failure_node);
                    return true;
                }
                return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return false;
    }

    std::string ValidateExportResponse::generate() const
    {
        pugi::xml_document doc;
        if (m_success)
        {
            auto root = doc.append_child("ValidationSuccess");
            if (!m_channel_warnings.empty())
            {
                auto ch_warnings = root.append_child("Channels");
                for (const auto& channel : m_channel_warnings)
                {
                    auto ch_warning = ch_warnings.append_child("Channel");
                    auto xpath = std::string("//ExportProperties/Channels/Channel[Id=") + odk::to_string(channel.channel_id) + "]";
                    ch_warning.append_attribute("path").set_value(xpath.c_str());
                    xpugi::setText(ch_warning.append_child("Id"), odk::to_string(channel.channel_id));
                    xpugi::setText(ch_warning.append_child("ErrorCode"), odk::to_string(channel.error_code));
                    xpugi::setText(ch_warning.append_child("ErrorMessage"), odk::error_codes::toString(channel.error_code));
                }
            }

            appendMessagesTo(root, m_messages);
        }
        else
        {
            auto root = doc.append_child("ValidationFailed");
            if (!m_channel_errors.empty())
            {
                auto ch_errors = root.append_child("Channels");
                for (const auto& channel : m_channel_errors)
                {
                    auto ch_error = ch_errors.append_child("Channel");
                    auto xpath = std::string("//ExportProperties/Channels/Channel[Id=") + odk::to_string(channel.channel_id) + "]";
                    ch_error.append_attribute("path").set_value(xpath.c_str());
                    xpugi::setText(ch_error.append_child("Id"), odk::to_string(channel.channel_id));
                    xpugi::setText(ch_error.append_child("ErrorCode"), odk::to_string(channel.error_code));
                    xpugi::setText(ch_error.append_child("ErrorMessage"), channel.error_message);
                }
            }

            appendMessagesTo(root, m_messages);
        }
        return xpugi::toXML(doc);
    }

}
