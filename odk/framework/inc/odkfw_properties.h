// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkfw_interfaces.h"

namespace odk
{
namespace framework
{
    class PropertyBase : public IfChannelProperty
    {
    public:
        // if false, the property is hidden from oxygen (not save in setups or accessible in any way)
        void setLive(bool live)
        {
            m_live = live;
            notifyChanged();
        }

        bool isLive() const
        {
            return m_live;
        }

        // indicates if the items should be shown automatically in oxygen (channel list etc.)
        void setVisiblity(const std::string& v)
        {
            m_visibility = v;
            notifyChanged();
        }

        std::string getVisiblity() const
        {
            return m_visibility;
        }

    protected:
        PropertyBase()
            : m_live(true)
            , m_change_listener(nullptr)
        {
        }

        void setChangeListener(IfChannelPropertyChangeListener* l) override;

        void notifyChanged();

        void addBaseConstraints(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const;

    private:
        bool m_live;
        std::string m_visibility;
        IfChannelPropertyChangeListener* m_change_listener;
    };

    class EditableUnsignedProperty : public PropertyBase
    {
    public:
        explicit EditableUnsignedProperty(unsigned int val, unsigned int mi = 1, unsigned int ma = 0);

        unsigned int getValue() const;

        void setValue(unsigned int v);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

    private:
        unsigned int m_value;
        unsigned int m_min;
        unsigned int m_max;
    };

    class EditableScalarProperty : public PropertyBase
    {
    public:
        explicit EditableScalarProperty(double val, const std::string& unit, double mi = 1.0, double ma = 0.0);

        odk::Scalar getValue() const;
        void setValue(const odk::Scalar& s);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

        void addOption(double val);

    private:
        double m_value;
        std::string m_unit;
        double m_min;
        double m_max;
        std::vector<double> m_options;
    };

    class EditableStringProperty : public PropertyBase
    {
    public:
        explicit EditableStringProperty(const std::string& val);

        std::string getValue() const;
        void setValue(const std::string& str);

        void setRegEx(const std::string& regex);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        void addOption(const std::string& val);
        void setArbitraryString(bool state);
    private:
        std::string m_value;
        std::string m_regex;
        std::vector<std::string> m_options;
        bool m_is_a_arbitrary_string;
    };

    class EditableChannelIDProperty : public PropertyBase
    {
    public:
        explicit EditableChannelIDProperty(const odk::ChannelID val = -1);

        odk::ChannelID getValue() const;
        void setValue(odk::ChannelID ch_id);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        odk::ChannelID m_value;
    };

    class EditableChannelIDListProperty : public PropertyBase
    {
    public:
        explicit EditableChannelIDListProperty();

        odk::ChannelIDList getValue() const;
        void setValue(odk::ChannelIDList value);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        odk::ChannelIDList m_value;
    };

    class BooleanProperty : public PropertyBase
    {
    public:
        explicit BooleanProperty(bool value);

        const bool& getValue() const;
        void setValue(const bool& value);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        bool m_value;
    };

    class EditableBooleanProperty : public PropertyBase
    {
    public:
        explicit EditableBooleanProperty(bool value);

        const bool& getValue() const;
        void setValue(const bool& value);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        bool m_value;
    };


    class RangeProperty : public PropertyBase
    {
    public:
        explicit RangeProperty(const odk::Range& value);

        const odk::Range& getValue() const;
        void setValue(const odk::Range& value);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        odk::Range m_value;
    };
}
}
