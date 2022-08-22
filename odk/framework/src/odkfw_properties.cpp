// Copyright DEWETRON GmbH 2019
#include "odkfw_properties.h"

#include "odkbase_basic_values.h"

#include "odkuni_assert.h"
#include "odkuni_string_util.h"

#include <cmath>
#include <regex>


///TODO: notifyChanged when a constraint changes?

namespace odk
{
namespace framework
{

    RawPropertyHolder::RawPropertyHolder()
        : m_value{}
    {
    }

    RawPropertyHolder::RawPropertyHolder(const odk::Property& value)
        : m_value{value}
    {
    }

    bool odk::framework::RawPropertyHolder::update(const odk::Property& value)
    {
        m_value = value;
        return true;
    }

    void RawPropertyHolder::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        auto prop = m_value;
        prop.setName(property_name);
        telegram.addProperty(prop);
    }

    odk::Property RawPropertyHolder::getProperty() const
    {
        return m_value;
    }

    EditableUnsignedProperty::EditableUnsignedProperty(const RawPropertyHolder& value)
        : m_value{}
        , m_min(1)
        , m_max(0)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    EditableUnsignedProperty::EditableUnsignedProperty(unsigned int val, unsigned int mi, unsigned int ma)
        : m_value(val)
        , m_min(mi)
        , m_max(ma)
    {
    }
    unsigned int EditableUnsignedProperty::getValue() const
    {
        return m_value;
    }

    void EditableUnsignedProperty::setValue(unsigned int v)
    {
        if (v != m_value)
        {
            m_value = v;
            notifyChanged();
        }
    }
    void EditableUnsignedProperty::setMinMaxConstraint(unsigned int min, unsigned int max)
    {
        if (m_min != min || m_max != max)
        {
            m_min = min;
            m_max = max;
            notifyChanged();
        }
    }
    bool EditableUnsignedProperty::hasValidRange() const
    {
        return m_min <= m_max;
    }

    void EditableUnsignedProperty::clearOptions()
    {
        if (!m_options.empty())
        {
            m_options.clear();
            notifyChanged();
        }
    }

    void EditableUnsignedProperty::addOption(unsigned int val)
    {
        m_options.push_back(val);
        notifyChanged();
    }

    bool EditableUnsignedProperty::update(const odk::Property& value)
    {
        //TBD: is it necessary to accept/convert compatible property types?
        unsigned int v = 0;
        switch (value.getType())
        {
        case odk::Property::UNSIGNED_INTEGER:
            v = value.getUnsignedIntValue();
            break;
        case odk::Property::FLOATING_POINT_NUMBER:
        {
            double pv = value.getDoubleValue();
            if (pv >= 0 && pv <= std::numeric_limits<unsigned int>::max())
            {
                v = static_cast<unsigned int>(round(pv));
            }
            else
            {
                return false;
            }
        } break;
        case odk::Property::STRING:
        {
            try
            {
                v = odk::from_string<unsigned int>(value.getStringValue());
            }
            catch (const std::logic_error&)
            {
                return false;
            }
            break;
        }
        default:
            return false;
        }
        if (hasValidRange())
        {
            if (v < m_min || v > m_max)
            {
                return false;
            }
        }
        m_value = v;
        return true;
    }
    void EditableUnsignedProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        telegram.addProperty<std::uint32_t>(property_name, m_value);
        if (hasValidRange())
        {
            telegram.addConstraint(property_name, odk::makeRangeConstraint(m_min, m_max));
        }

        for (std::uint32_t option : m_options)
        {
            telegram.addConstraint(property_name,
                odk::UpdateConfigTelegram::Constraint::makeOption(odk::Property("", option)));
        }
    }

    odk::framework::EditableFloatingPointProperty::EditableFloatingPointProperty(const RawPropertyHolder& value)
        : m_value{}
        , m_min(1.0)
        , m_max(0.0)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }
    EditableFloatingPointProperty::EditableFloatingPointProperty(const double value)
        : m_value(value)
        , m_min(1.0)
        , m_max(0.0)
    {
    }
    double EditableFloatingPointProperty::getValue() const
    {
        return m_value;
    }
    void EditableFloatingPointProperty::setValue(const double value)
    {
        if (value != m_value)
        {
            m_value = value;
            notifyChanged();
        }
    }
    void EditableFloatingPointProperty::setMinMaxConstraint(double min, double max)
    {
        m_min = min;
        m_max = max;
    }
    void EditableFloatingPointProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property(property_name);
        property.setValue(m_value);
        telegram.addProperty(property);
        if (hasValidRange())
        {
            telegram.addConstraint(property_name, odk::makeRangeConstraint(m_min, m_max));
        }
    }
    bool EditableFloatingPointProperty::update(const odk::Property& value)
    {
        double v = std::numeric_limits<double>::quiet_NaN();
        switch (value.getType())
        {
            case odk::Property::FLOATING_POINT_NUMBER:
            {
                double pv = value.getDoubleValue();
                v = pv;
            }
        }

        if (hasValidRange())
        {
            if (v < m_min || v > m_max)
            {
                return false;
            }
        }
        m_value = v;

        return true;
    }
    bool EditableFloatingPointProperty::hasValidRange() const
    {
        return m_min <= m_max;
    }

    EditableScalarProperty::EditableScalarProperty(const RawPropertyHolder& value)
        : m_value{}
        , m_unit{}
        , m_min(1)
        , m_max(0)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    EditableScalarProperty::EditableScalarProperty(double val, const std::string& unit, double mi, double ma)
        : m_value(val)
        , m_unit(unit)
        , m_min(mi)
        , m_max(ma)
    {
    }

    EditableScalarProperty::EditableScalarProperty(odk::Scalar scalar, double mi, double ma)
        : m_value(scalar.m_val)
        , m_unit(scalar.m_unit)
        , m_min(mi)
        , m_max(ma)
    {
    }

    odk::Scalar EditableScalarProperty::getValue() const
    {
        return { m_value, m_unit };
    }
    void EditableScalarProperty::setValue(const odk::Scalar& s)
    {
        if (!(s == odk::Scalar(m_value, m_unit)))
        {
            m_value = s.m_val;
            m_unit = s.m_unit;
            notifyChanged();
        }
    }
    void EditableScalarProperty::setMinMaxConstraint(double min, double max)
    {
        if (m_min != min || m_max != max)
        {
            m_min = min;
            m_max = max;
            notifyChanged();
        }
    }
    bool EditableScalarProperty::hasValidRange() const
    {
        return m_min <= m_max;
    }
    bool EditableScalarProperty::update(const odk::Property& value)
    {
        double v = 0;
        switch (value.getType())
        {
            case odk::Property::SCALAR:
            {
                auto scalar_value = value.getScalarValue();
                if (m_unit.empty())
                {
                    m_unit = scalar_value.m_unit;
                }
                else
                {
                    if (scalar_value.m_unit != m_unit)
                    {
                        return false;
                    }
                }
                v = scalar_value.m_val;
            } break;
            case odk::Property::FLOATING_POINT_NUMBER:
            {
                v = value.getDoubleValue();
            } break;
            default:
                return false;
        }
        if (hasValidRange())
        {
            if (v < m_min || v > m_max)
            {
                return false;
            }
        }
        m_value = v;
        return true;
    }
    void EditableScalarProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        telegram.addProperty(property_name, odk::Scalar(m_value, m_unit));
        if (hasValidRange())
        {
            telegram.addConstraint(property_name, odk::makeRangeConstraint(m_min, m_max));
        }

        for (const auto& str_option : m_string_options)
        {
            telegram.addConstraint(property_name,
                odk::UpdateConfigTelegram::Constraint::makeOption(odk::Property("", str_option)));
        }

        std::vector<odk::Property> options;
        for (const auto& option : m_options)
        {
             options.emplace_back("", odk::Scalar(option, m_unit));
        }
        if (!options.empty())
        {
            telegram.addConstraint(property_name, odk::UpdateConfigTelegram::Constraint::makeOptions(options));
        }
    }
    void EditableScalarProperty::addOption(double val)
    {
        m_options.push_back(val);
    }

    void EditableScalarProperty::clearOptions()
    {
        m_options.clear();
    }

    void EditableScalarProperty::addStringOption(const std::string& val)
    {
        m_string_options.push_back(val);
    }

    EditableFilePathProperty::EditableFilePathProperty(const RawPropertyHolder& value)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    EditableFilePathProperty::EditableFilePathProperty(EditableFilePathProperty::FileType ft,
        const std::string& name, const std::string& title, const std::string& path,
        const std::vector<std::string>& filters)
        : m_file_type(ft)
        , m_filename(name)
        , m_title(title)
        , m_default_path(path)
        , m_filters(filters)
        , m_multi_select(false)
    {
    }

    std::string EditableFilePathProperty::getFilename() const
    {
        return m_filename;
    }

    void EditableFilePathProperty::setFilename(const std::string& name)
    {
        if (m_filename != name)
        {
            m_filename = name;
            notifyChanged();
        }
    }

    std::string EditableFilePathProperty::getTitle() const
    {
        return m_title;
    }

    void EditableFilePathProperty::setTitle(const std::string& title)
    {
        if (m_title != title)
        {
            m_title = title;
            notifyChanged();
        }
    }

    std::string EditableFilePathProperty::getDefaultPath() const
    {
        return m_default_path;
    }

    void EditableFilePathProperty::setDefaultPath(const std::string& path)
    {
        if (m_default_path != path)
        {
            m_default_path = path;
            notifyChanged();
        }
    }

    std::vector<std::string> EditableFilePathProperty::getNameFilters() const
    {
        return m_filters;
    }

    void EditableFilePathProperty::setNameFilters(const std::vector<std::string>& filters)
    {
        if (m_filters != filters)
        {
            m_filters = filters;
            notifyChanged();
        }
    }

    EditableFilePathProperty::FileType EditableFilePathProperty::getFileType() const
    {
        return m_file_type;
    }

    std::string EditableFilePathProperty::toString(FileType ft)
    {
        switch (ft)
        {
            case FileType::INPUT_FILE:
                    return "Input";
            case FileType::OUTPUT_FILE:
                    return "Output";
            default:;
        }
        return "Directory";
    }

    void EditableFilePathProperty::setFiletype(const EditableFilePathProperty::FileType ft)
    {
        m_file_type = ft;
        notifyChanged();
    }

    void EditableFilePathProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram,
        const std::string& property_name) const
    {
        odk::Property property(property_name, m_filename);
        telegram.addProperty(property);
        auto ft = toString(m_file_type);
        telegram.addConstraint(property_name,
            odk::makeFilePathConstraint(ft, m_title, m_default_path, m_filters, m_multi_select));
        telegram.addConstraint(property_name, odk::makeArbitraryStringConstraint());
    }

    bool EditableFilePathProperty::update(const odk::Property& value)
    {
        m_filename = value.getStringValue();
        return true;
    }

    StringProperty::StringProperty(const RawPropertyHolder& value)
        : m_value{}
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    StringProperty::StringProperty(const std::string& val)
        : m_value(val)
    {
    }

    std::string StringProperty::getValue() const
    {
        return m_value;
    }

    void StringProperty::setValue(const std::string& str)
    {
        if (str != m_value)
        {
            m_value = str;
            notifyChanged();
        }
    }

    void StringProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        telegram.addProperty<std::string>(property_name, m_value);
    }

    bool StringProperty::update(const odk::Property& value)
    {
        m_value = value.getStringValue();
        return true;
    }

    EditableStringProperty::EditableStringProperty(const RawPropertyHolder& value)
        : StringProperty()
        , m_is_a_arbitrary_string(true)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    EditableStringProperty::EditableStringProperty(const std::string& val)
        : StringProperty(val)
        , m_is_a_arbitrary_string(true)
    {
    }

    void EditableStringProperty::setRegEx(const std::string& regex)
    {
        if (!regex.empty())
        {
            try
            {
                m_regex = std::regex(regex);
            }
            catch (std::regex_error& /*e*/)
            {
                ODK_ASSERT_FAIL("Invalid Regex");
                return;
            }
            m_regex_str = regex;
            m_is_a_arbitrary_string = false;
        }
        notifyChanged();
    }
    bool EditableStringProperty::update(const odk::Property& value)
    {
        if (!m_regex_str.empty())
        {
            try
            {
                const auto& v = value.getStringValue();
                const auto valid = std::regex_match(v, m_regex);
                if (!valid)
                {
                    return false;
                }
            }
            catch (std::regex_error& /*e*/)
            {
                ODK_ASSERT_FAIL("Invalid Regex");
            }
        }
        return StringProperty::update(value);
    }
    void EditableStringProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        StringProperty::doAddToTelegram(telegram, property_name);

        if (m_is_a_arbitrary_string)
        {
            telegram.addConstraint(property_name, odk::makeArbitraryStringConstraint());
        }

        if (! m_regex_str.empty())
        {
            telegram.addConstraint(property_name, odk::makeRegExConstraint(m_regex_str.c_str()));
        }

        for (const auto& option : m_options)
        {
            telegram.addConstraint(property_name,
                odk::UpdateConfigTelegram::Constraint::makeOption(odk::Property("", option)));
        }
    }

    void EditableStringProperty::addOption(const std::string& val)
    {
        m_options.emplace_back(val);
    }

    void EditableStringProperty::setArbitraryString(bool state)
    {
        m_is_a_arbitrary_string = state;
    }

    void EditableStringProperty::clearOptions()
    {
        m_options.clear();
    }

    void PropertyBase::setChangeListener(IfChannelPropertyChangeListener* l)
    {
        m_change_listener = l;
    }
    void PropertyBase::notifyChanged()
    {
        if (m_change_listener) m_change_listener->onChannelPropertyChanged(this);
    }
    void PropertyBase::addBaseConstraints(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (!m_visibility.empty())
        {
            telegram.addConstraint(property_name, odk::makeVisiblityConstraint(m_visibility));
        }
    }

    EditableChannelIDProperty::EditableChannelIDProperty(const odk::ChannelID val)
        : m_value(val)
    {
    }

    odk::ChannelID EditableChannelIDProperty::getValue() const
    {
        return m_value;
    }

    void EditableChannelIDProperty::setValue(odk::ChannelID ch_id)
    {
        if (ch_id != m_value)
        {
            m_value = ch_id;
            notifyChanged();
        }
    }

    bool EditableChannelIDProperty::isValid() const
    {
        return m_value != std::numeric_limits<odk::ChannelID>::max();
    }

    void EditableChannelIDProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property(property_name);
        property.setChannelIDValue(m_value);
        telegram.addProperty(property);
        telegram.addConstraint(property_name, odk::makeChannelIdsConstraint(1));
    }

    bool EditableChannelIDProperty::update(const odk::Property& value)
    {
        m_value = value.getChannelIDValue();
        return true;
    }

    EditableChannelIDListProperty::EditableChannelIDListProperty()
        : m_value()
        , m_filter_type(ChannelType::ALL)
        , m_filter_dimensions(-1)
        , m_max_channels(0)
    {
    }

    odk::ChannelIDList EditableChannelIDListProperty::getValue() const
    {
        return m_value;
    }

    void EditableChannelIDListProperty::setValue(const odk::ChannelIDList& value)
    {
        if (!(value == m_value))
        {
            m_value = value;
            notifyChanged();
        }
    }

    void EditableChannelIDListProperty::setChannelType(ChannelType type)
    {
        if (type != m_filter_type)
        {
            m_filter_type = type;
            notifyChanged();
        }
    }

    void EditableChannelIDListProperty::setMaxDimension(int dims)
    {
        if (dims != m_filter_dimensions)
        {
            m_filter_dimensions = dims;
            notifyChanged();
        }
    }

    void EditableChannelIDListProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property(property_name);
        property.setChannelIDListValue(m_value);
        telegram.addProperty(property);

        std::string type;
        switch (m_filter_type)
        {
        case ChannelType::SYNC:
            type = "SYNC";
            break;
        case ChannelType::ASYNC:
            type = "ASYNC";
            break;
        default:
            type = "ALL";
            break;
        }

        telegram.addConstraint(property_name, odk::makeChannelIdsConstraint(m_max_channels, m_filter_dimensions, type));
    }

    bool EditableChannelIDListProperty::update(const odk::Property& value)
    {
        m_value = value.getChannelIDListValue();
        return true;
    }

    BooleanProperty::BooleanProperty(bool value)
        : m_value(value)
        , m_editable(false)
    {
    }

    BooleanProperty::BooleanProperty(const RawPropertyHolder& value)
        : m_value(false)
        , m_editable(false)
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    bool BooleanProperty::getValue() const
    {
        return m_value;
    }

    void BooleanProperty::setValue(bool value)
    {
        if (value != m_value)
        {
            m_value = value;
            notifyChanged();
        }
    }

    void BooleanProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property(property_name);
        property.setValue(m_value);
        telegram.addProperty(property);

        if(m_editable)
        {
            std::vector<odk::Property> options{
                odk::Property("", false),
                odk::Property("", true) };
            telegram.addConstraint(property_name, odk::UpdateConfigTelegram::Constraint::makeOptions(options));
        }
    }

    bool BooleanProperty::update(const odk::Property& value)
    {
        m_value = value.getBoolValue();
        return true;
    }

    void BooleanProperty::setEditable(bool editable)
    {
        m_editable = editable;
    }

    EditableBooleanProperty::EditableBooleanProperty(const RawPropertyHolder& value)
        : BooleanProperty(value)
    {
    }

    EditableBooleanProperty::EditableBooleanProperty(bool value)
        : BooleanProperty(value)
    {
        setEditable(true);
    }

    RangeProperty::RangeProperty(const RawPropertyHolder& value)
        : m_value{}
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    RangeProperty::RangeProperty(const odk::Range &value)
        : m_value(value)
    {
    }

    odk::Range RangeProperty::getValue() const
    {
        return m_value;
    }

    void RangeProperty::setValue(odk::Range value)
    {
        if (!(value == m_value))
        {
            m_value = value;
            notifyChanged();
        }
    }

    void RangeProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property(property_name);
        property.setValue(m_value);
        telegram.addProperty(property);

        for (const auto& option : m_options)
        {
            telegram.addConstraint(property_name,
                odk::UpdateConfigTelegram::Constraint::makeOption(odk::Property("", option)));
        }
    }

    void RangeProperty::addOption(const odk::Range& val)
    {
        m_options.emplace_back(val);
    }

    bool RangeProperty::update(const odk::Property& value)
    {
        m_value = value.getRangeValue();
        return true;
    }

    void RangeProperty::clearOptions()
    {
        m_options.clear();
    }


    SelectableProperty::SelectableProperty(const RawPropertyHolder& value)
        : SelectableProperty(value.getProperty())
    {
    }

    SelectableProperty::SelectableProperty(odk::Property prop)
        : m_property(prop)
    {
    }

    odk::Property SelectableProperty::getValue() const
    {
        return m_property;
    }

    void SelectableProperty::setValue(odk::Property prop)
    {
        if (!(prop == m_property))
        {
            m_property = prop;
            notifyChanged();
        }
    }

    void SelectableProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        odk::Property property = m_property;
        property.setName(property_name);
        telegram.addProperty(property);
        addBaseConstraints(telegram, property_name);
        for (const auto& option : m_options)
        {
            telegram.addConstraint(property_name, odk::UpdateConfigTelegram::Constraint::makeOption(option));
        }
    }

    bool SelectableProperty::update(const odk::Property& value)
    {
        for (const auto& option : m_options)
        {
            if (value.sameValue(option))
            {
                setValue(value);
                break;
            }
        }
        return true;
    }

    void SelectableProperty::addOption(odk::Property prop)
    {
        m_options.emplace_back(prop);
    }

    void SelectableProperty::clearOptions()
    {
        m_options.clear();
    }

    std::uint32_t SelectableProperty::count() const
    {
        return static_cast<std::uint32_t>(m_options.size());
    }

    StringList StringListProperty::getValue() const
    {
        return m_value;
    }

    void StringListProperty::setValue(const StringList &value)
    {
        if(m_value != value)
        {
            m_value = value;
            notifyChanged();
        }
    }

    void StringListProperty::doAddToTelegram(UpdateConfigTelegram::ChannelConfig &telegram, const std::string &property_name) const
    {
        odk::Property property(property_name);
        property.setValue(m_value);
        telegram.addProperty(property);
    }

    bool StringListProperty::update(const Property& value)
    {
        m_value = value.getStringListValue();
        return true;
    }


/*    SelectableProperty::SelectableProperty(odk::detail::ApiObjectPtr<odk::IfValue> value)
        : m_value(value)
    {
    }

    odk::detail::ApiObjectPtr<odk::IfValue> SelectableProperty::getValue() const
    {
        return m_value;
    }

    void SelectableProperty::setValue(odk::detail::ApiObjectPtr<odk::IfValue> value)
    {
        m_value = value;
        notifyChanged();
    }

    void SelectableProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            if (auto e_num = odk::value_ptr_cast<odk::IfEnumValue>(m_value))
            {
                property.setEnumValue(e_num->getEnumValue(), e_num->getEnumName());
            }
            telegram.addProperty(property);
            addBaseConstraints(telegram, property_name);
            for (const auto& option : m_options)
            {
                auto enum_option = odk::api_ptr_cast<odk::IfEnumValue>(option);
                if (enum_option)
                {
                    odk::Property prop(property_name, enum_option->getEnumValue(), enum_option->getEnumName());
                    telegram.addConstraint(property_name, odk::UpdateConfigTelegram::Constraint::makeOption(prop));
                }
            }

        }
    }

    bool SelectableProperty::update(const odk::Property& value)
    {
        if (auto val = odk::value_ptr_cast<odk::IfEnumValue>(m_value))
        {
            val->set(value.getEnumType().c_str(), value.getEnumValue().c_str());
        }
        return true;
    }

    void SelectableProperty::addOption(const odk::detail::ApiObjectPtr<odk::IfValue> option)
    {
        m_options.emplace_back(option);
    }*/


    XmlStringProperty::XmlStringProperty(const RawPropertyHolder& value)
        : StringProperty()
    {
        if (value.getProperty().isValid())
        {
            update(value.getProperty());
        }
    }

    XmlStringProperty::XmlStringProperty(const std::string& val)
        : StringProperty(val)
    {
    }

    bool XmlStringProperty::update(const odk::Property& value)
    {
        return StringProperty::update(value);
    }

    void XmlStringProperty::doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        telegram.addProperty(odk::Property(property_name, getValue(), "XML"));
    }
}
}

