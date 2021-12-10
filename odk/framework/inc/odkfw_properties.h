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

        void addToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override
        {
            if(isLive())
            {
                doAddToTelegram(telegram, property_name);
                addBaseConstraints(telegram, property_name);
            }
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

        virtual void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const = 0;

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

        odk::Property getProperty() const;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

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

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

        void addOption(unsigned int val);

        void clearOptions();

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        unsigned int m_value;
        unsigned int m_min;
        unsigned int m_max;
        std::vector<unsigned int> m_options;
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

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

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

        bool update(const odk::Property& value) override;

        bool hasValidRange() const;

        void addOption(double val);

        void clearOptions();

        void addStringOption(const std::string& val);

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

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

        bool update(const odk::Property& value) override;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        FileType m_file_type;
        std::string m_filename;

        std::string m_title;
        std::string m_default_path;
        std::vector<std::string> m_filters;
        bool m_multi_select;
    };

    class StringProperty : public PropertyBase
    {
    public:
        StringProperty() = default;
        explicit StringProperty(const RawPropertyHolder& value);
        explicit StringProperty(const std::string& val);

        std::string getValue() const;
        void setValue(const std::string& str);

        bool update(const odk::Property& value) override;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        std::string m_value;
    };

    class EditableStringProperty : public StringProperty
    {
    public:
        explicit EditableStringProperty(const RawPropertyHolder& value);
        explicit EditableStringProperty(const std::string& val);

        void setRegEx(const std::string& regex);

        bool update(const odk::Property& value) override;

        void addOption(const std::string& val);
        void setArbitraryString(bool state);

        void clearOptions();

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
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

        bool update(const odk::Property& value) override;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        odk::ChannelID m_value;
    };

    class EditableChannelIDListProperty : public PropertyBase
    {
    public:
        enum class ChannelType : uint8_t
        {
            ALL,
            SYNC,
            ASYNC,
        };

        explicit EditableChannelIDListProperty();

        odk::ChannelIDList getValue() const;
        void setValue(const odk::ChannelIDList& value);

        bool update(const odk::Property& value) override;

        void setChannelType(ChannelType type);
        void setMaxDimension(int max_dimensions);

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        odk::ChannelIDList m_value;
        ChannelType m_filter_type;
        int m_filter_dimensions;
        uint32_t m_max_channels;
    };

    class BooleanProperty : public PropertyBase
    {
    public:
        explicit BooleanProperty(bool value);
        explicit BooleanProperty(const RawPropertyHolder& value);

        bool getValue() const;
        void setValue(bool value);

        bool update(const odk::Property& value) override;

        void setEditable(bool editable);

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        bool m_value;
        bool m_editable;
    };

    class EditableBooleanProperty : public BooleanProperty
    {
    public:
        explicit EditableBooleanProperty(const RawPropertyHolder& value);
        explicit EditableBooleanProperty(bool value);
    };

    class RangeProperty : public PropertyBase
    {
    public:
        explicit RangeProperty(const RawPropertyHolder& value);
        explicit RangeProperty(const odk::Range& value);

        odk::Range getValue() const;
        void setValue(odk::Range value);

        bool update(const odk::Property& value) override;

        void addOption(const odk::Range& val);

        void clearOptions();

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

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

        bool update(const odk::Property& value) override;

        void addOption(odk::Property);
        void clearOptions();

        std::uint32_t count() const;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        odk::Property m_property;
        std::vector<odk::Property> m_options;
    };

    class StringListProperty : public PropertyBase
    {
    public:
        explicit StringListProperty() = default;

        odk::StringList getValue() const;
        void setValue(const odk::StringList& value);

        bool update(const odk::Property& value) override;

    protected:
        void doAddToTelegram(odk::UpdateConfigTelegram::ChannelConfig& telegram, const std::string& property_name) const override;

    private:
        odk::StringList m_value;
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
