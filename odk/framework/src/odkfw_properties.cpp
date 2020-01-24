// Copyright DEWETRON GmbH 2019
#include "odkfw_properties.h"

#include <cmath>

namespace odk
{
namespace framework
{
        
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
        m_value = v;
        notifyChanged();
    }
    bool EditableUnsignedProperty::hasValidRange() const
    {
        return m_min <= m_max;
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
    void EditableUnsignedProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            telegram.addProperty<std::uint32_t>(property_name, m_value);
            if (hasValidRange())
            {
                telegram.addConstraint(property_name, odk::makeRangeConstraint(m_min, m_max));
            }
            addBaseConstraints(telegram, property_name);
        }
    }

    EditableScalarProperty::EditableScalarProperty(double val, const std::string& unit, double mi, double ma)
        : m_value(val)
        , m_unit(unit)
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
        m_value = s.m_val;
        m_unit = s.m_unit;
        notifyChanged();
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
                if (scalar_value.m_unit != m_unit)
                {
                    return false;
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
    void EditableScalarProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            telegram.addProperty(property_name, odk::Scalar(m_value, m_unit));
            if (hasValidRange())
            {
                telegram.addConstraint(property_name, odk::makeRangeConstraint(m_min, m_max));
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
            addBaseConstraints(telegram, property_name);
        }
    }
    void EditableScalarProperty::addOption(double val)
    {
        m_options.push_back(val);
    }

    EditableStringProperty::EditableStringProperty(const std::string& val)
        : m_value(val)
        , m_is_a_arbitrary_string(true)
    {
    }
    std::string EditableStringProperty::getValue() const
    {
        return m_value;
    }
    void EditableStringProperty::setValue(const std::string& str)
    {
        m_value = str;
        notifyChanged();
    }
    void EditableStringProperty::setRegEx(const std::string& regex)
    {
        m_regex = regex;
        if (!regex.empty())
        {
            m_is_a_arbitrary_string = false;
        }
        notifyChanged();
    }
    bool EditableStringProperty::update(const odk::Property& value)
    {
        auto v = value.getStringValue();
        if (!m_regex.empty())
        {
            //TODO: verify regex match
        }
        m_value = v;
        return true;
    }
    void EditableStringProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            telegram.addProperty<std::string>(property_name, m_value);
            if (m_is_a_arbitrary_string)
            {
                telegram.addConstraint(property_name, odk::makeArbitraryStringConstraint());
            }

            if (! m_regex.empty())
            {
                telegram.addConstraint(property_name, odk::makeRegExConstraint(m_regex.c_str()));
            }

            for (const auto& option : m_options)
            {
                telegram.addConstraint(property_name,
                    odk::UpdateConfigTelegram::Constraint::makeOption(odk::Property("", option)));
            }

            addBaseConstraints(telegram, property_name);
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
        m_value = ch_id;
        notifyChanged();
    }

    void EditableChannelIDProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            property.setChannelIDValue(m_value);
            telegram.addProperty(property);
            telegram.addConstraint(property_name, odk::makeChannelIdsConstraint(1));
            addBaseConstraints(telegram, property_name);
        }
    }

    bool EditableChannelIDProperty::update(const odk::Property& value)
    {
        m_value = value.getChannelIDValue();
        return true;
    }

    EditableChannelIDListProperty::EditableChannelIDListProperty()
        : m_value()
    {
    }

    odk::ChannelIDList EditableChannelIDListProperty::getValue() const
    {
        return m_value;
    }

    void EditableChannelIDListProperty::setValue(odk::ChannelIDList value)
    {
        m_value = value;
        notifyChanged();
    }

    void EditableChannelIDListProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            property.setChannelIDListValue(m_value);
            telegram.addProperty(property);
            telegram.addConstraint(property_name, odk::makeChannelIdsConstraint(0));
            addBaseConstraints(telegram, property_name);
        }
    }

    bool EditableChannelIDListProperty::update(const odk::Property& value)
    {
        m_value = value.getChannelIDListValue();
        return true;
    }

    BooleanProperty::BooleanProperty(bool value)
        : m_value(value)
    {
    }

    const bool& BooleanProperty::getValue() const
    {
        return m_value;
    }

    void BooleanProperty::setValue(const bool& value)
    {
        m_value = value;
        notifyChanged();
    }

    void BooleanProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            property.setValue(m_value);
            telegram.addProperty(property);
            addBaseConstraints(telegram, property_name);
        }
    }

    bool BooleanProperty::update(const odk::Property& value)
    {
        m_value = value.getBoolValue();
        return true;
    }

    EditableBooleanProperty::EditableBooleanProperty(bool value)
        : m_value(value)
    {
    }

    const bool& EditableBooleanProperty::getValue() const
    {
        return m_value;
    }

    void EditableBooleanProperty::setValue(const bool& value)
    {
        m_value = value;
        notifyChanged();
    }

    void EditableBooleanProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            property.setValue(m_value);
            telegram.addProperty(property);

            std::vector<odk::Property> options{
                odk::Property("", false),
                odk::Property("", true) };
            telegram.addConstraint(property_name, odk::UpdateConfigTelegram::Constraint::makeOptions(options));
            addBaseConstraints(telegram, property_name);
        }
    }

    bool EditableBooleanProperty::update(const odk::Property& value)
    {
        m_value = value.getBoolValue();
        return true;
    }

    RangeProperty::RangeProperty(const odk::Range &value)
        : m_value(value)
    {
    }

    const odk::Range& RangeProperty::getValue() const
    {
        return m_value;
    }

    void RangeProperty::setValue(const odk::Range& value)
    {
        m_value = value;
        notifyChanged();
    }

    void RangeProperty::addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const
    {
        if (isLive())
        {
            odk::Property property(property_name);
            property.setValue(m_value);
            telegram.addProperty(property);
            addBaseConstraints(telegram, property_name);
        }
    }

    bool RangeProperty::update(const odk::Property& value)
    {
        m_value = value.getRangeValue();
        return true;
    }

}
}

