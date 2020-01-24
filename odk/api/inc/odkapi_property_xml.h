// Copyright DEWETRON GmbH 2014

#pragma once

#include "odkapi_types.h"
#include "odkapi_version_xml.h"

#include <boost/rational.hpp>
#include <boost/shared_ptr.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace odk
{
    class PropertyList;
    struct Scalar
    {
        Scalar();
        Scalar(const double& val, const std::string& unit);

        bool operator==(Scalar const& other) const;

        double m_val;
        std::string m_unit;
    };

    struct Rational
    {
        using value_type = boost::rational<int64_t>;
        Rational();

        Rational(const value_type& val, const std::string& unit);

        bool operator==(Rational const& other) const;

        value_type m_val;
        std::string m_unit;
    };

    struct DecoratedNumber
    {
        DecoratedNumber();

        DecoratedNumber(const double& val, const std::string& prefix, const std::string& suffix);

        bool operator==(DecoratedNumber const& other) const;

        double m_val;
        std::string m_prefix;
        std::string m_suffix;
    };

    struct Range
    {
        Range();
        Range(const double& min, const double& max, const std::string& min_unit, const std::string& max_unit);

        bool operator==(Range const& other) const;

        double m_min;
        double m_max;
        std::string m_min_unit;
        std::string m_max_unit;
    };

    template<class VT>
    struct ValueList
    {
        typedef VT ValueType;
        typedef std::vector<ValueType> ListType;

        ValueList()
            : m_values()
        {}

        explicit ValueList(const ListType& values)
            : m_values(values)
        {
        }

        explicit ValueList(ListType&& values)
            : m_values(std::move(values))
        {
        }

        explicit ValueList(std::initializer_list<ValueType> list)
            : m_values(list)
        {
        }

        template<class Iter>
        explicit ValueList(Iter begin, Iter end)
            : m_values(begin, end)
        {
        }

        bool operator==(ValueList<ValueType> const& rhs) const
        {
            if (m_values.size() != rhs.m_values.size())
            {
                return false;
            }

            auto it = m_values.begin();
            auto rhs_it = rhs.m_values.begin();
            for (; it != m_values.end(); ++it, ++rhs_it)
            {
                if (*it != *rhs_it)
                {
                    return false;
                }
            }
            return true;
        }

        ListType m_values;
    };
    typedef ValueList<double> DoubleList;
    typedef ValueList<std::string> StringList;
    typedef std::pair<double, double> Point;
    typedef ValueList<Point> PointList;
    typedef ValueList<ChannelID> ChannelIDList;

    /**
     * @brief The Property class holds a generic name/value pair that can be
     * converted from/to XML
     *
     * In addition the type of the value is stored as well
     */
    class Property
    {
    public:
        enum Type {
            UNKNOWN = 0,
            STRING = 1,
            INTEGER = 2,
            UNSIGNED_INTEGER = 3,
            UNSIGNED_INTEGER64 = 4,
            FLOATING_POINT_NUMBER = 5,
            BOOLEAN = 6,
            COLOR = 7,
            DATE = 8,
            DATETIME = 9,
            RANGE = 10,
            ENUM = 11,
            PROPERTY_LIST = 12,
            SCALAR = 13,
            STRING_LIST = 14,
            POINT_LIST = 15,
            DECORATED_NUMBER = 16,
            CHANNEL_ID = 17,
            FLOATING_POINT_NUMBER_LIST = 18,
            RATIONAL = 19,
            GEO_COORDINATE = 20,
            POINT = 21,
            CHANNEL_ID_LIST = 22,
        };

        static Type getPropertyTypeFromValue(const Scalar& v) { return SCALAR; }
        static Type getPropertyTypeFromValue(const DecoratedNumber& v) { return DECORATED_NUMBER; }
        static Type getPropertyTypeFromValue(const Range& v) { return RANGE; }
        static Type getPropertyTypeFromValue(const DoubleList& v) { return FLOATING_POINT_NUMBER_LIST; }
        static Type getPropertyTypeFromValue(const StringList& v) { return STRING_LIST; }
        static Type getPropertyTypeFromValue(const Point& v) { return POINT; }
        static Type getPropertyTypeFromValue(const PointList& v) { return POINT_LIST; }
        static Type getPropertyTypeFromValue(const Rational& v) { return RATIONAL; }
        static Type getPropertyTypeFromValue(const ChannelIDList& v) { return CHANNEL_ID_LIST; }
        static Type getPropertyTypeFromValue(const PropertyList& v) { return PROPERTY_LIST; }

        Property();
        Property(const Property& other);
        explicit Property(const std::string& name);

        Property(const std::string& name, const std::string& string_value);
        Property(const std::string& name, const char* const string_value);
        Property(const std::string& name, bool value);
        Property(const std::string& name, const std::string& value, const std::string& enum_type);
        Property(const std::string& name, Type type, const std::string& value);


        template <class T>
        Property(const std::string& name, T value)
            : m_name(name)
            , m_type(getPropertyTypeFromValue(value))
            , m_value(std::make_shared<T>(value))
        {
        }

        std::string getNodeName() const;

        bool operator==(Property const& other) const;

        /**
         * Returns true, if the data in the property is valid
         */
        bool isValid() const;

        /**
         * Sets the name of this property as string
         */
        void setName(const std::string& name);
        /**
         * Returns the name of this property as string
         */
        const std::string& getName() const;
        /**
         * Returns the type of this property
         */
        Type getType() const;

        /**
         * Sets the value of this property as string
         */
        void setValue(const std::string& value);
        /**
         * Returns the value of this property as string
         * This method always returns the property as string, regardless of it's type
         */
        const std::string& getStringValue() const;

        /**
         * Returns the value of this property as const char
         */
        void setValue(const char* value);

        /**
         * Sets the value of this property as int
         */
        void setValue(int value);
        /**
         * Returns the value of this property as int
         * If this property is not of type INTEGER, an exception is thrown
         */
        int getIntValue() const;

        /**
         * Sets the value of this property as unsigned int
         */
        void setValue(unsigned int value);
        /**
         * Returns the value of this property as int
         * If this property is not of type UNSIGNED_INTEGER, an exception is thrown
         */
        unsigned int getUnsignedIntValue() const;

        /**
         * Sets the value of this property as double
         */

        /**
         * Sets the value of this property as unsigned int 64 bit
         */
        void setValue(std::uint64_t value);
        /**
         * Returns the value of this property as uint64
         * If this property is not of type UNSIGNED_INTEGER64, an exception is thrown
         */
        std::uint64_t getUnsignedInt64Value() const;

        void setValue(double value);
        /**
         * Returns the value of this property as double
         * If this property is not of type FLOATING_POINT_NUMBER, an exception is thrown
         */
        double getDoubleValue() const;

        /**
         * Sets the value of this property as bool
         */
        void setValue(bool value);
        /**
         * Returns the value of this property as bool
         * If this property is not of type BOOLEAN, an exception is thrown
         */
        bool getBoolValue() const;

        /**
         * Sets the value of this property as scalar
         */
        void setValue(const Scalar& value);

        /**
         * Returns the value of this property as Scalar
         * If this property is not of type SCALAR, an exception is thrown
         */
        const Scalar& getScalarValue() const;

        void setValue(const DecoratedNumber& value);

        /**
         * Returns the value of this property as DecoratedNumber
         * If this property is not of type DECORATED_NUMBER, an exception is thrown
         */
        const DecoratedNumber& getDecoratedNumberValue() const;

        /**
         * Sets the value of this property as range
         */
        void setValue(const Range& value);

        /**
         * Returns the value of this property as Range
         * If this property is not of type RANGE, an exception is thrown
         */
        const Range& getRangeValue() const;

        /**
         * Sets the value of this property as enum
         */
        void setEnumValue(const std::string& value, const std::string& enum_type);
        /**
         * Returns the value of this property as string
         * If this property is not of type ENUM, an exception is thrown
         */
        const std::string& getEnumValue() const;
        /**
         * Returns the enum type of this property as string
         * If this property is not of type ENUM, an exception is thrown
         */
        const std::string& getEnumType()const;

        /**
         * Sets the value of this property as PropertyList
         */
        void setValue(const PropertyList& value);

        /**
        * Sets the value of this property as DoubleList
        */
        void setValue(const DoubleList& value);

        /**
        * Returns the value of this property as DoubleList
        * If this property is not of type FLOATING_POINT_NUMBER_LIST, an exception is thrown
        */
        DoubleList getDoubleListValue() const;

        /**
         * Sets the value of this property as StringList
         */
        void setValue(const StringList& value);

        /**
         * Returns the value of this property as StringList
         * If this property is not of type STRING_LIST, an exception is thrown
         */
        StringList getStringListValue() const;

        /**
         * Sets the value of this property as Point
         */
        void setValue(const Point& value);

        const Point& getPointValue() const;

        /**
         * Sets the value of this property as PointList
         */
        void setValue(const PointList& value);

        /**
         * Sets the value of this property as boost::rational
         */
        void setValue(const Rational& value);

        const Rational& getRationalValue() const;

        void setChannelIDValue(const ChannelID& value);

        ChannelID getChannelIDValue() const;

        void setChannelIDListValue(const ChannelIDList& value);

        ChannelIDList getChannelIDListValue() const;

        void setDateValue(const std::string& date);
        void setDateTimeValue(const std::string& date_time);
        void setColorValue(const std::string& color);
        void setGeoCoordinateValue(const std::string& coord);
        /**
         * Returns the value of this property as string
         * If this property is not of type POINT_LIST, an exception is thrown
         */
        PointList getPointListValue() const;

        /**
         * Returns the value of this property as PropertyList
         * If this property is not of type PROPERTY_LIST, an exception is thrown
         */
        const PropertyList& getPropertyListValue() const;

        pugi::xml_node appendTo(pugi::xml_node parent) const;
        bool readFrom(const pugi::xml_node& tree, const Version& version);

        pugi::xml_node appendValue(pugi::xml_node parent) const;
        bool readValue(pugi::xml_node type_node, const Version& version);

        /**
        * @brief returns a representable string of the value
        */
        std::string valueToString() const;

    private:
        std::string toXMLType(Type type) const;
        Type fromXMLType(const std::string& xml_type) const;

        bool parseEnum(const pugi::xml_node& type_node);

        void appendScalarNode(pugi::xml_node parent) const;
        Scalar parseScalarNode(const pugi::xml_node& type_node);

        void appendDecoratedNumberNode(pugi::xml_node parent) const;
        DecoratedNumber parseDecoratedNumberNode(const pugi::xml_node& type_node);

        void appendRangeNode(pugi::xml_node parent) const;
        Range parseRangeNode(const pugi::xml_node& type_node);

        void appendPropertyListNode(pugi::xml_node parent) const;
        PropertyList parsePropertyListNode(const pugi::xml_node& type_node, const Version& version);

        void appendDoubleListNode(pugi::xml_node parent) const;
        DoubleList parseDoubleListNode(const pugi::xml_node& type_node);

        void appendStringListNode(pugi::xml_node parent) const;
        StringList parseStringListNode(const pugi::xml_node& type_node);

        void appendPointNode(pugi::xml_node parent) const;
        Point parsePointNode(const pugi::xml_node& type_node);

        void appendPointListNode(pugi::xml_node parent) const;
        PointList parsePointListNode(const pugi::xml_node& type_node);

        void appendRationalNode(pugi::xml_node parent) const;
        Rational parseRationalNode(const pugi::xml_node& type_node);

        void appendChannelIdListNode(pugi::xml_node parent) const;
        ChannelIDList parseChannelIdListNode(const pugi::xml_node& type_node);

        std::string unitToString(const std::string& unit_string) const;

        std::string m_name;
        Type m_type;
        std::string m_enum_type;
        //only one of m_string_value or m_value may contain a value
        std::string m_string_value;
        std::shared_ptr<void> m_value;
    };
} // namespace neoncfg
