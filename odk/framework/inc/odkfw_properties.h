// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkfw_interfaces.h"

#include <regex>

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

    // This property is not intended to be used directly
    // The ODK-Framework will use this property on Load-Setup, das detailed information
    // about the actual type is plugin-knowledge
    class RawPropertyHolder : public PropertyBase
    {
    public:
        explicit RawPropertyHolder();
        explicit RawPropertyHolder(const odk::Property& value);

        bool update(const odk::Property& value) override;

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        odk::Property getProperty() const;

    private:
        odk::Property m_value;
    };

    class EditableUnsignedProperty : public PropertyBase
    {
    public:
        explicit EditableUnsignedProperty(const RawPropertyHolder& value);
        explicit EditableUnsignedProperty(unsigned int val, unsigned int mi = 1, unsigned int ma = 0);

        unsigned int getValue() const;

        void setValue(unsigned int v);

        void setMinMaxConstraint(unsigned int min = 1, unsigned int max = 0);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

    private:
        unsigned int m_value;
        unsigned int m_min;
        unsigned int m_max;
    };

    class EditableFloatingPointProperty : public PropertyBase
    {
    public:
        //TODO limit constraints
        explicit EditableFloatingPointProperty(const RawPropertyHolder& value);
        explicit EditableFloatingPointProperty(const double value);

        double getValue() const;

        void setValue(const double value);

        void setMinMaxConstraint(double min = 1.0, double max = 0.0);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

    private:
        double m_value;
        double m_min;
        double m_max;
    };

    class EditableScalarProperty : public PropertyBase
    {
    public:
        explicit EditableScalarProperty(const RawPropertyHolder& value);
        explicit EditableScalarProperty(double val, const std::string& unit, double mi = 1.0, double ma = 0.0);
        explicit EditableScalarProperty(odk::Scalar scalar, double mi = 1.0, double ma = 0.0);

        odk::Scalar getValue() const;
        void setValue(const odk::Scalar& s);

        void setMinMaxConstraint(double min = 1, double max = 0);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

        void addOption(double val);

        void clearOptions();

        void addStringOption(const std::string& val);

    private:
        double m_value;
        std::string m_unit;
        double m_min;
        double m_max;
        std::vector<double> m_options;
        std::vector<std::string> m_string_options;
    };

    class EditableFilePathProperty : public PropertyBase
    {
    public:
        enum class FileType : uint8_t
        {
            INPUT_FILE,
            OUTPUT_FILE,
            DIRECTORY
        };

        explicit EditableFilePathProperty(const RawPropertyHolder& value);
        EditableFilePathProperty(FileType ft, const std::string& name = "",
            const std::string& title = "", const std::string& path = "",
            const std::vector<std::string>& filter = {});

        static std::string toString(FileType ft);
        FileType getFileType() const;
        void setFiletype(const FileType ft);
        std::string getFilename() const;
        void setTitle(const std::string& fl);
        std::string getTitle() const;
        void setFilename(const std::string& fl);
        std::string getDefaultPath() const;
        void setDefaultPath(const std::string& path);
        std::vector<std::string> getNameFilters() const;
        void setNameFilters(const std::vector<std::string>& filter);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

    private:
        FileType m_file_type;
        std::string m_filename;

        std::string m_title;
        std::string m_default_path;
        std::vector<std::string> m_filters;
        bool m_multi_select;
    };

    class EditableStringProperty : public PropertyBase
    {
    public:
        explicit EditableStringProperty(const RawPropertyHolder& value);
        explicit EditableStringProperty(const std::string& val);

        std::string getValue() const;
        void setValue(const std::string& str);

        void setRegEx(const std::string& regex);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        void addOption(const std::string& val);
        void setArbitraryString(bool state);

        void clearOptions();
    private:
        std::string m_value;
        std::string m_regex_str;
        std::regex m_regex;
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
        explicit RangeProperty(const RawPropertyHolder& value);
        explicit RangeProperty(const odk::Range& value);

        const odk::Range& getValue() const;
        void setValue(const odk::Range& value);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        void addOption(const odk::Range& val);

        void clearOptions();

    private:
        odk::Range m_value;
        std::vector<odk::Range> m_options;
    };

    class SelectableProperty : public PropertyBase
    {
    public:
        explicit SelectableProperty(const RawPropertyHolder& value);
        explicit SelectableProperty(odk::Property);

        odk::Property getValue() const;
        void setValue(odk::Property);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        void addOption(odk::Property);
        void clearOptions();

        std::uint32_t count() const;
    private:
        odk::Property m_property;
        std::vector<odk::Property> m_options;
    };

/*    class SelectableProperty : public PropertyBase
    {
    public:
        explicit SelectableProperty(odk::detail::ApiObjectPtr<odk::IfValue>);

        odk::detail::ApiObjectPtr<odk::IfValue> getValue() const;
        void setValue(odk::detail::ApiObjectPtr<odk::IfValue>);

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

        bool update(const odk::Property& value) override;

        void addOption(const odk::detail::ApiObjectPtr<odk::IfValue>);
    private:
        odk::detail::ApiObjectPtr<odk::IfValue> m_value;
        std::vector< odk::detail::ApiObjectPtr<odk::IfValue>> m_options;
    }; */

}
}
