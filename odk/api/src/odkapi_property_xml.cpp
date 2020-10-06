// Copyright DEWETRON GmbH 2014

#include "odkapi_property_xml.h"

#include "odkapi_property_list_xml.h"
#include "odkuni_assert.h"
#include "odkuni_logger.h"
#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace odk
{
    Property::Property()
        : m_name("")
        , m_type(UNKNOWN)
        , m_value()
    {
    }

    Property::Property(const Property& other)
        : m_name(other.m_name)
        , m_type(other.m_type)
        , m_enum_type(other.m_enum_type)
        , m_string_value(other.m_string_value)
        , m_value(other.m_value)
    {
    }

    Property::Property(const std::string& name)
        : m_name(name)
        , m_type(UNKNOWN)
    {
    }

    Property::Property(const std::string& name, const std::string& string_value)
        : m_name(name)
        , m_type(STRING)
        , m_string_value(string_value)
    {
    }

    Property::Property(const std::string& name, const char* const string_value)
        : m_name(name)
        , m_type(STRING)
        , m_string_value(string_value)
    {
    }

    Property::Property(const std::string& name, const std::string& value, const std::string& enum_type)
        : m_name(name)
        , m_type(ENUM)
        , m_enum_type(enum_type)
        , m_string_value(value)
    {
    }

    Property::Property(const std::string& name, bool value)
        : m_name(name)
        , m_type(UNKNOWN)
    {
        setValue(value);
    }
    Property::Property(const std::string& name, Type type, const std::string& value)
        : m_name(name)
        , m_type(type)
        , m_string_value(value)
    {
    }

    std::string Property::getNodeName() const
    {
        return {"Property"};
    }

    bool Property::operator==( Property const& other ) const
    {
        return m_name == other.m_name
            && m_type == other.m_type
            && m_enum_type == other.m_enum_type
            && m_string_value == other.m_string_value
            && (
                (m_value == other.m_value)
                ||
                (m_value && other.m_value && true/* *m_value == *other.m_value*/) //FIXME
                );

    }

    bool Property::isValid() const
    {
        return m_type != UNKNOWN;
    }

    void Property::setName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& Property::getName() const
    {
        return m_name;
    }

    Property::Type Property::getType() const
    {
        return m_type;
    }

    void Property::setValue(const std::string& value)
    {
        m_type = STRING;
        m_string_value = value;
        m_value.reset();
    }

    const std::string& Property::getStringValue() const
    {
        if (m_type == RANGE)
        {
            std::string error = std::string("property ") + m_name + " is type Range";
            ODKLOG_ERROR(error);
            throw std::runtime_error(error);
        }
        if (m_type == SCALAR)
        {
            std::string error = std::string("property ") + m_name + " is type Scalar";
            ODKLOG_ERROR(error);
            throw std::runtime_error(error);
        }
        return m_string_value;
    }

    void Property::setValue(const char* value)
    {
        setValue(std::string(value));
    }

    void Property::setValue(int value)
    {
        m_type = INTEGER;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    int Property::getIntValue() const
    {
        if (m_type != INTEGER)
        {
            std::string error = std::string("property ") + m_name + " is not of type int";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<int>(m_string_value);
    }

    void Property::setValue(unsigned int value)
    {
        m_type = UNSIGNED_INTEGER;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    unsigned int Property::getUnsignedIntValue() const
    {
        if (m_type != UNSIGNED_INTEGER)
        {
            std::string error = std::string("property ") + m_name + " is not of type unsigned int";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<unsigned int>(m_string_value);
    }

    void Property::setValue(std::uint64_t value) {
        m_type = UNSIGNED_INTEGER64;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    std::uint64_t Property::getUnsignedInt64Value() const {
        if (m_type != UNSIGNED_INTEGER64 && m_type != UNSIGNED_INTEGER)
        {
            std::string error = std::string("property ") + m_name + " is not of type uint64";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<std::uint64_t>(m_string_value);
    }

    void Property::setValue(std::int64_t value) {
        m_type = INTEGER64;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    std::int64_t Property::getInt64Value() const {
        if (m_type != INTEGER64 && m_type != INTEGER)
        {
            std::string error = std::string("property ") + m_name + " is not of type sint64";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<std::int64_t>(m_string_value);
    }

    void Property::setValue(double value)
    {
        m_type = FLOATING_POINT_NUMBER;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    double Property::getDoubleValue() const
    {
        if (m_type != FLOATING_POINT_NUMBER)
        {
            std::string error = std::string("property ") + m_name + " is not of type double";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<double>(m_string_value);
    }

    bool Property::getBoolValue() const
    {
        if (m_type != BOOLEAN)
        {
            std::string error = std::string("property ") + m_name + " is not of type bool";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        if(m_string_value == "True")
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void Property::setValue(bool value)
    {
        m_type = BOOLEAN;
        m_string_value = value ? "True" : "False";
        m_value.reset();
    }

    void Property::setValue(const Scalar& value)
    {
        m_type = SCALAR;
        m_value = std::make_shared<Scalar>(value);
    }

    const Scalar& Property::getScalarValue() const
    {
        if (m_type != SCALAR)
        {
            std::string error = std::string("property ") + m_name + " is not of type scalar";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<Scalar>(m_value);
    }

    void Property::setValue(const DecoratedNumber& value)
    {
        m_type = DECORATED_NUMBER;
        m_value = std::make_shared<DecoratedNumber>(value);
    }

    const DecoratedNumber& Property::getDecoratedNumberValue() const
    {
        if (m_type != DECORATED_NUMBER)
        {
            std::string error = std::string("property ") + m_name + " is not of type decorated_number";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<DecoratedNumber>(m_value);
    }

    void Property::setValue(const Range& value)
    {
        m_type = RANGE;
        m_string_value.clear();
        m_value = std::make_shared<Range>(value);
    }

    const Range& Property::getRangeValue() const
    {
        if (m_type != RANGE)
        {
            std::string error = std::string("property ") + m_name + " is not of type range";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<Range>(m_value);
    }

    void Property::setEnumValue(const std::string& value, const std::string& enum_type)
    {
        m_type = ENUM;
        m_string_value = value;
        m_value.reset();
        m_enum_type = enum_type;
    }

    const std::string& Property::getEnumValue() const
    {
        if (m_type != ENUM)
        {
            std::string error = std::string("property ") + m_name + " is not of type enum";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return m_string_value;
    }

    const std::string& Property::getEnumType() const
    {
        if (m_type != ENUM)
        {
            std::string error = std::string("property ") + m_name + " is not of type enum";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return m_enum_type;
    }

    void Property::setValue(const PropertyList& value)
    {
        m_type = PROPERTY_LIST;
        m_value = std::make_shared<PropertyList>(value);
    }

    void Property::setValue(const DoubleList& value)
    {
        m_type = FLOATING_POINT_NUMBER_LIST;
        m_string_value.clear();
        m_value = std::make_shared<DoubleList>(value);
    }

    DoubleList Property::getDoubleListValue() const
    {
        if (m_type != FLOATING_POINT_NUMBER_LIST)
        {
            std::string error = std::string("property ") + m_name + " is not of type realnumberlist";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<DoubleList>(m_value);
    }

    void Property::setValue(const StringList& value)
    {
        m_type = STRING_LIST;
        m_string_value.clear();
        m_value = std::make_shared<StringList>(value);
    }
    StringList Property::getStringListValue() const
    {
        if (m_type != STRING_LIST)
        {
            std::string error = std::string("property ") + m_name + " is not of type stringlist";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<StringList>(m_value);
    }

    void Property::setValue(const Rational& value)
    {
        m_type = RATIONAL;
        m_string_value.clear();
        m_value = std::make_shared<Rational>(value);
    }

    const Rational& Property::getRationalValue() const
    {
        if (m_type != RATIONAL)
        {
            std::string error = std::string("property ") + m_name + " is not of type rational";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<Rational>(m_value);
    }

    void Property::setValue(const Point& value)
    {
        m_type = POINT;
        m_string_value.clear();
        m_value = std::make_shared<Point>(value);
    }

    const Point& Property::getPointValue() const
    {
        if (m_type != POINT)
        {
            std::string error = std::string("property ") + m_name + " is not of type point";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<Point>(m_value);
    }

    void Property::setValue(const PointList& value)
    {
        m_type = POINT_LIST;
        m_string_value.clear();
        m_value = std::make_shared<PointList>(value);
    }

    void Property::setChannelIDValue(const ChannelID& value)
    {
        m_type = CHANNEL_ID;
        m_string_value = boost::lexical_cast<std::string>(value);
        m_value.reset();
    }

    ChannelID Property::getChannelIDValue() const
    {
        if (m_type != CHANNEL_ID)
        {
            std::string error = std::string("property ") + m_name + " is not of type channel id";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return boost::lexical_cast<ChannelID>(m_string_value);
    }

    void Property::setChannelIDListValue(const ChannelIDList& value)
    {
        m_type = CHANNEL_ID_LIST;
        m_string_value.clear();
        m_value = std::make_shared<ChannelIDList>(value);
    }

    ChannelIDList Property::getChannelIDListValue() const
    {
        if (m_type != CHANNEL_ID_LIST)
        {
            std::string error = std::string("property ") + m_name + " is not of type channel id list";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<ChannelIDList>(m_value);
    }

    void Property::setDateValue(const std::string& date)
    {
        m_type = DATE;
        m_string_value = date;
        m_value.reset();
    }

    void Property::setDateTimeValue(const std::string& date_time)
    {
        m_type = DATETIME;
        m_string_value = date_time;
        m_value.reset();
    }

    void Property::setColorValue(const std::string& color)
    {
        m_type = COLOR;
        m_string_value = color;
        m_value.reset();
    }

    void Property::setGeoCoordinateValue(const std::string& coord)
    {
        m_type = GEO_COORDINATE;
        m_string_value = coord;
        m_value.reset();
    }

    PointList Property::getPointListValue() const
    {
        if (m_type != POINT_LIST)
        {
            std::string error = std::string("property ") + m_name + " is not of type pointlist";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<PointList>(m_value);
    }

    const PropertyList& Property::getPropertyListValue() const
    {
        if (m_type != PROPERTY_LIST)
        {
            std::string error = std::string("property ") + m_name + " is not of type property list";
            ODKLOG_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        return *std::static_pointer_cast<PropertyList>(m_value);
    }

    pugi::xml_node Property::appendTo(pugi::xml_node parent) const
    {
        auto element = parent.append_child(getNodeName().c_str());
        ODK_ASSERT(element);
        xpugi::setNewAttribute(element, "name", m_name);
        appendValue(element);
        return element;
    }

    bool Property::readFrom(const pugi::xml_node& tree, const Version& version)
    {
        if (!odk::strequal(tree.name(), getNodeName().c_str()) || tree.type() != pugi::node_element)
        {
            ODKLOG_ERROR("Error parsing Property");
            return false;
        }
        pugi::xml_attribute attr = tree.attribute("name");
        if (!attr)
        {
            ODKLOG_ERROR("Error parsing name attribute from Property");
            return false;
        }
        m_name = attr.as_string();

        bool ret = false;
        for (pugi::xml_node type_node : tree.children())
        {
            if (type_node.type() == pugi::node_element)
            {
                ret = readValue(type_node, version);
            }
        }

        if (!ret)
        {
            ODKLOG_ERROR("Error: Property " << m_name << " has no supported type defining node");
        }
        return ret;
    }

    pugi::xml_node  Property::appendValue(pugi::xml_node element) const
    {
        std::string typeText = toXMLType(m_type);
        if (typeText.empty())
        {
            return {};
        }

        auto type = element.append_child(typeText.c_str());
        ODK_ASSERT(type);

        switch (m_type)
        {
            case STRING:
            {
                // string property uses quotes to preserve whitespaces
                const std::string& single_value = "\"" + m_string_value + "\"";
                xpugi::setText(type, single_value);
                break;
            }
            case INTEGER:
            case UNSIGNED_INTEGER:
            case UNSIGNED_INTEGER64:
            case FLOATING_POINT_NUMBER:
            case BOOLEAN:
            case COLOR:
            case DATE:
            case DATETIME:
            case CHANNEL_ID:
            case GEO_COORDINATE:
            {
                const std::string& single_value = m_string_value;
                xpugi::setText(type, single_value);
                break;
            }
            case Property::RANGE:
            {
                this->appendRangeNode(type);
                break;
            }
            case Property::SCALAR:
            {
                this->appendScalarNode(type);
                break;
            }
            case Property::RATIONAL:
            {
                this->appendRationalNode(type);
                break;
            }
            case Property::DECORATED_NUMBER:
            {
                this->appendDecoratedNumberNode(type);
                break;
            }
            case Property::ENUM:
            {
                const std::string& single_value = m_string_value;
                xpugi::setText(type, single_value);
                xpugi::setNewAttribute(type, "enum", m_enum_type);
                break;
            }
            case FLOATING_POINT_NUMBER_LIST:
            {
                this->appendDoubleListNode(type);
                break;
            }
            case STRING_LIST:
            {
                this->appendStringListNode(type);
                break;
            }
            case POINT:
            {
                this->appendPointNode(type);
                break;
            }
            case POINT_LIST:
            {
                this->appendPointListNode(type);
                break;
            }
            case PROPERTY_LIST:
            {
                this->appendPropertyListNode(type);
                break;
            }
            case CHANNEL_ID_LIST:
            {
                this->appendChannelIdListNode(type);
                break;
            }
            default:
                return {};
        }
        return type;
    }

    bool Property::readValue(pugi::xml_node type_node, const Version& version)
    {
        bool ret = false;
        std::string type_name = type_node.name();
        auto type = fromXMLType(type_name);
        m_type = UNKNOWN;
        m_string_value.clear();
        m_value.reset();
        switch (type)
        {
            case UNKNOWN:
            {
                ODKLOG_DEBUG("Warning: Property has unknown type " << type_name);
                break;
            }
            case STRING:
            {
                pugi::xml_node value = type_node.first_child();
                if (value)
                {
                    // string property uses quotes to preserve whitespace
                    std::string text = xpugi::getText(value);
                    if (*(text.begin()) == '"')
                        if (*(text.rbegin()) == '"')
                            text = text.substr(1, text.length() - 2);
                        else
                            text = text.substr(1, text.length() - 1);
                    else if (*(text.rbegin()) == '"')
                        text = text.substr(0, text.length() - 1);
                    setValue(text);
                    ret = true;
                }
                break;
            }
            case INTEGER:
            case UNSIGNED_INTEGER:
            case UNSIGNED_INTEGER64:
            case FLOATING_POINT_NUMBER:
            case BOOLEAN:
            case COLOR:
            case DATE:
            case DATETIME:
            case CHANNEL_ID:
            case GEO_COORDINATE:
            {
                pugi::xml_node value = type_node.first_child();
                if (value)
                {
                    m_type = type;
                    m_string_value = xpugi::getText(value);
                    ret = true;
                }
                break;
            }
            case RANGE:
            {
                setValue(parseRangeNode(type_node));
                ret = true;
                break;
            }
            case SCALAR:
            {
                setValue(parseScalarNode(type_node));
                ret = true;
                break;
            }
            case RATIONAL:
            {
                setValue(parseRationalNode(type_node));
                ret = true;
                break;
            }
            case DECORATED_NUMBER:
            {
                setValue(parseDecoratedNumberNode(type_node));
                ret = true;
                break;
            }
            case ENUM:
            {
                ret = parseEnum(type_node);
                break;
            }
            case FLOATING_POINT_NUMBER_LIST:
            {
                setValue(parseDoubleListNode(type_node));
                ret = true;
                break;
            }
            case STRING_LIST:
            {
                setValue(parseStringListNode(type_node));
                ret = true;
                break;
            }
            case POINT:
            {
                setValue(parsePointNode(type_node));
                ret = true;
                break;
            }
            case POINT_LIST:
            {
                setValue(parsePointListNode(type_node));
                ret = true;
                break;
            }
            case PROPERTY_LIST:
            {
                setValue(parsePropertyListNode(type_node, version));
                ret = true;
                break;
            }
            case CHANNEL_ID_LIST:
            {
                setChannelIDListValue(parseChannelIdListNode(type_node));
                ret = true;
                break;
            }
        }
        return ret;
    }

    std::string Property::toXMLType(Type type) const
    {
        switch (m_type)
        {
        case STRING:
            return "StringValue";

        case DATE:
            return "DateValue";

        case DATETIME:
            return "DateTimeValue";

        case COLOR:
            return "ColorValue";

        case INTEGER:
            return "SignedValue";

        case UNSIGNED_INTEGER:
            return "UnsignedValue";

        case UNSIGNED_INTEGER64:
            return "UnsignedInt64";

        case FLOATING_POINT_NUMBER:
            return "DoubleValue";

        case BOOLEAN:
            return "BooleanValue";

        case SCALAR:
            return "ScalarValue";

        case RATIONAL:
            return "RationalValue";

        case DECORATED_NUMBER:
            return "DecoratedNumber";

        case RANGE:
            return "RangeValue";

        case ENUM:
            return "EnumValue";

        case FLOATING_POINT_NUMBER_LIST:
            return "DoubleListValue";

        case STRING_LIST:
            return "StringListValue";

        case POINT_LIST:
            return "PointListValue";

        case PROPERTY_LIST:
            return "PropertyListValue";

        case CHANNEL_ID:
            return "ChannelID";

        case CHANNEL_ID_LIST:
            return "ChannelIDList";

        case GEO_COORDINATE:
            return "GeoCoordinateValue";

        case POINT:
            return "Point";

        default:
            ODKLOG_WARN("Unknown property data type " << m_type);
            return {};
        }
    }

    Property::Type Property::fromXMLType(const std::string& xml_type) const
    {
        if (xml_type == "StringValue")
        {
            return STRING;
        }
        if (xml_type == "DateValue")
        {
            return DATE;
        }
        if (xml_type == "DateTimeValue")
        {
            return DATETIME;
        }
        if (xml_type == "ColorValue")
        {
            return COLOR;
        }
        if (xml_type == "SignedValue")
        {
            return INTEGER;
        }
        if (xml_type == "UnsignedValue")
        {
            return UNSIGNED_INTEGER;
        }
        if (xml_type == "UnsignedInt64")
        {
            return UNSIGNED_INTEGER64;
        }
        if (xml_type == "DoubleValue")
        {
            return FLOATING_POINT_NUMBER;
        }
        if (xml_type == "BooleanValue")
        {
            return BOOLEAN;
        }
        if (xml_type == "RangeValue")
        {
            return RANGE;
        }
        if (xml_type == "ScalarValue")
        {
            return SCALAR;
        }
        if (xml_type == "RationalValue")
        {
            return RATIONAL;
        }
        if (xml_type == "DecoratedNumber")
        {
            return DECORATED_NUMBER;
        }
        if (xml_type == "EnumValue")
        {
            return ENUM;
        }
        if (xml_type == "DoubleListValue")
        {
            return FLOATING_POINT_NUMBER_LIST;
        }
        if (xml_type == "StringListValue")
        {
            return STRING_LIST;
        }
        if (xml_type == "PointListValue")
        {
            return POINT_LIST;
        }
        if (xml_type == "PropertyListValue")
        {
            return PROPERTY_LIST;
        }
        if (xml_type == "ChannelID")
        {
            return CHANNEL_ID;
        }
        if (xml_type == "ChannelIDList")
        {
            return CHANNEL_ID_LIST;
        }
        if (xml_type == "GeoCoordinateValue")
        {
            return GEO_COORDINATE;
        }
        if (xml_type == "Point")
        {
            return POINT;
        }

        ODKLOG_WARN("Unknown property data type " << xml_type);

        return UNKNOWN;
    }

    bool Property::parseEnum(const pugi::xml_node& type_node)
    {
        auto value = type_node.first_child();
        if (!value)
        {
            return false;
        }

        auto attr = type_node.attribute("enum");
        if (!attr)
        {
            ODKLOG_ERROR("Error parsing enum attribute from Property")
                return false;
        }
        setEnumValue(xpugi::getText(value), xpugi::getText(attr));

        return true;
    }

    void Property::appendScalarNode(pugi::xml_node parent) const
    {
        const Scalar& scalar = getScalarValue();

        auto value_node = parent.append_child("Value");
        ODK_ASSERT(value_node);
        //ADD_DUMMY_DATA(value_node);

        const auto value_text = boost::lexical_cast<std::string>(scalar.m_val);
        xpugi::setText(value_node, value_text);

        auto unit_node = parent.append_child("Unit");
        ODK_ASSERT(unit_node);
        //ADD_DUMMY_DATA(unit_node);

        xpugi::setText(unit_node, scalar.m_unit);
    }

    Scalar Property::parseScalarNode(const pugi::xml_node& type_node)
    {
        Scalar scalar;
        for (pugi::xml_node n : type_node.children())
        {
            if (n.type() == pugi::node_element)
            {
                std::string sub_name = n.name();
                if (sub_name == "Value")
                {
                    scalar.m_val = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else if (sub_name == "Unit")
                {
                    scalar.m_unit = xpugi::getText(n);
                }
                else
                {
                    std::string error = std::string("Warning: ") + type_node.name() +
                        " has unknown node " + sub_name;
                    ODKLOG_DEBUG(error.c_str())
                }
            }
        }
        return scalar;
    }

    void Property::appendRationalNode(pugi::xml_node parent) const
    {
        const auto& rational = getRationalValue();

        auto num_node = parent.append_child("Numerator");
        ODK_ASSERT(num_node);
        xpugi::setText(num_node, boost::lexical_cast<std::string>(rational.m_val.numerator()));

        auto den_node = parent.append_child("Denominator");
        ODK_ASSERT(den_node);
        xpugi::setText(den_node, boost::lexical_cast<std::string>(rational.m_val.denominator()));

        auto unit_node = parent.append_child("Unit");
        ODK_ASSERT(unit_node);
        xpugi::setText(unit_node, rational.m_unit);
    }

    Rational Property::parseRationalNode(const pugi::xml_node& type_node)
    {
        Rational rational;
        int64_t den = 0, num = 0;
        for (pugi::xml_node n : type_node.children())
        {
            if (n.type() == pugi::node_element)
            {
                std::string sub_name = n.name();
                if (sub_name == "Numerator")
                {
                    num = boost::lexical_cast<int64_t>(xpugi::getText(n));
                }
                else if (sub_name == "Denominator")
                {
                    den = boost::lexical_cast<int64_t>(xpugi::getText(n));
                }
                else if (sub_name == "Unit")
                {
                    rational.m_unit = xpugi::getText(n);
                }
                else
                {
                    std::string error = std::string("Warning: ") + type_node.name() +
                        " has unknown node " + sub_name;
                    ODKLOG_DEBUG(error.c_str())
                }
            }
        }
        rational.m_val.assign(num, den);
        return rational;
    }

    void Property::appendDecoratedNumberNode(pugi::xml_node parent) const
    {
        const DecoratedNumber& decorated_num = getDecoratedNumberValue();
        if (!decorated_num.m_prefix.empty())
        {
            auto prefix_element = parent.append_child("Prefix");
            ODK_ASSERT(prefix_element);
            xpugi::setText(prefix_element, decorated_num.m_prefix);
        }

        auto value_element = parent.append_child("Value");
        ODK_ASSERT(value_element);
        xpugi::setText(value_element, boost::lexical_cast<std::string>(decorated_num.m_val));

        if (!decorated_num.m_suffix.empty())
        {
            auto suffix_element = parent.append_child("Suffix");
            ODK_ASSERT(suffix_element);
            xpugi::setText(suffix_element, decorated_num.m_suffix);
        }
    }

    DecoratedNumber Property::parseDecoratedNumberNode(const pugi::xml_node& type_node)
    {
        DecoratedNumber deco_num;
        for (auto n : type_node.children())
        {
            if (n.type() == pugi::node_element)
            {
                std::string sub_name = n.name();
                if (sub_name == "Value")
                {
                    deco_num.m_val = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else if (sub_name == "Prefix")
                {
                    deco_num.m_prefix = xpugi::getText(n);
                }
                else if (sub_name == "Suffix")
                {
                    deco_num.m_suffix = xpugi::getText(n);
                }
                else
                {
                    std::string error = std::string("Warning: ") + type_node.name() +
                        " has unknown node " + sub_name;
                    ODKLOG_DEBUG(error.c_str())
                }
            }
        }
        return deco_num;
    }

    void Property::appendRangeNode(pugi::xml_node parent) const
    {
        const Range& range = getRangeValue();
        auto range_element = parent.append_child("RangeMin");
        ODK_ASSERT(range_element);
        //ADD_DUMMY_DATA(range_element);
        xpugi::setText(range_element, boost::lexical_cast<std::string>(range.m_min));

        range_element = parent.append_child("RangeMinUnit");
        ODK_ASSERT(range_element);
        //ADD_DUMMY_DATA(range_element);
        xpugi::setText(range_element, range.m_min_unit);

        range_element = parent.append_child("RangeMax");
        ODK_ASSERT(range_element);
        //ADD_DUMMY_DATA(range_element);
        xpugi::setText(range_element, boost::lexical_cast<std::string>(range.m_max));

        range_element = parent.append_child("RangeMaxUnit");
        ODK_ASSERT(range_element);
        //ADD_DUMMY_DATA(range_element);
        xpugi::setText(range_element, range.m_max_unit);
    }

    Range Property::parseRangeNode(const pugi::xml_node& type_node)
    {
        Range range;
        for (pugi::xml_node n : type_node.children())
        {
            if (n.type() == pugi::node_element)
            {
                std::string sub_name = n.name();
                if (sub_name == "RangeMin")
                {
                    range.m_min = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else if (sub_name == "RangeMax")
                {
                    range.m_max = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else if (sub_name == "RangeMinUnit")
                {
                    range.m_min_unit = xpugi::getText(n);
                }
                else if (sub_name == "RangeMaxUnit")
                {
                    range.m_max_unit = xpugi::getText(n);
                }
                else
                {
                    std::string error = std::string("Warning: ") + type_node.name() +
                        " has unknown node " + sub_name;
                    ODKLOG_DEBUG(error.c_str())
                }
            }
        }

        return range;
    }

    void Property::appendPropertyListNode(pugi::xml_node parent) const
    {
        const auto plist = std::static_pointer_cast<PropertyList>(m_value);
        plist->appendTo(parent);
    }

    PropertyList Property::parsePropertyListNode(
                const pugi::xml_node& type_node,
                const Version& version)
    {
        PropertyList plist;
        plist.readFrom(type_node, version);
        return plist;
    }

    void Property::appendDoubleListNode(pugi::xml_node parent) const
    {
        const DoubleList& list = getDoubleListValue();

        auto list_element = parent.append_child("DoubleList");
        ODK_ASSERT(list_element);
        for (double value : list.m_values)
        {
            auto item_element = list_element.append_child("Item");
            ODK_ASSERT(item_element);
            xpugi::setText(item_element, boost::lexical_cast<std::string>(value));
        }
    }

    DoubleList Property::parseDoubleListNode(const pugi::xml_node& type_node)
    {
        DoubleList list;
        auto list_nodes = type_node.children("DoubleList");
        if (list_nodes.begin() != list_nodes.end())
        {
            auto list_node = *list_nodes.begin();
            const auto& children = list_node.children("Item");
            list.m_values.reserve(std::distance(children.begin(), children.end()));
            for (auto node : children)
            {
                if (node.type() == pugi::node_element)
                {
                    std::string value_string = xpugi::getText(node);
                    boost::algorithm::trim(value_string);
                    list.m_values.push_back(boost::lexical_cast<double>(value_string));
                }
            }
        }
        else
        {
            std::string error = std::string("Warning: ") + type_node.name() +
                " has no StringList node";
            ODKLOG_DEBUG(error.c_str())
        }
        return list;
    }

    void Property::appendStringListNode(pugi::xml_node parent) const
    {
        const StringList& list = getStringListValue();

        auto list_element = parent.append_child("StringList");
        ODK_ASSERT(list_element);
        //ADD_DUMMY_DATA(list_element);
        for (const auto& text : list.m_values)
        {
            auto item_element = list_element.append_child("Item");
            ODK_ASSERT(item_element);
            xpugi::setText(item_element, text);
        }
    }

    StringList Property::parseStringListNode(const pugi::xml_node& type_node)
    {
        StringList string_list;
        auto list_nodes = type_node.children("StringList");
        if (list_nodes.begin() != list_nodes.end())
        {
            auto list_node = *list_nodes.begin();
            const auto& children = list_node.children("Item");
            string_list.m_values.reserve(std::distance(children.begin(), children.end()));
            for (auto node : children)
            {
                if (node.type() == pugi::node_element)
                {
                    string_list.m_values.push_back(xpugi::getText(node));
                }
            }
        }
        else
        {
            std::string error = std::string("Warning: ") + type_node.name() +
                " has no StringList node";
            ODKLOG_DEBUG(error.c_str())
        }
        return string_list;
    }

    void Property::appendPointNode(pugi::xml_node parent) const
    {
        const Point& point = getPointValue();

        auto x_element = parent.append_child("x");
        ODK_ASSERT(x_element);
        xpugi::setText(x_element, boost::lexical_cast<std::string>(point.first));

        auto y_element = parent.append_child("y");
        ODK_ASSERT(y_element);
        xpugi::setText(y_element, boost::lexical_cast<std::string>(point.second));
    }

    Point Property::parsePointNode(const pugi::xml_node& type_node)
    {
        Point point;

        for (auto n : type_node.children())
        {
            if (n.type() == pugi::node_element)
            {
                std::string sub_name = n.name();
                if (sub_name == "x")
                {
                    point.first = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else if (sub_name == "y")
                {
                    point.second = boost::lexical_cast<double>(xpugi::getText(n));
                }
                else
                {
                    std::string error = std::string("Warning: ") + type_node.name() +
                        " has unknown node " + sub_name;
                    ODKLOG_DEBUG(error.c_str())
                }
            }
        }

        return point;
    }

    void Property::appendPointListNode(pugi::xml_node parent) const
    {
        const PointList& list = getPointListValue();

        auto list_element = parent.append_child("PointList");
        ODK_ASSERT(list_element);
        //ADD_DUMMY_DATA(list_element);
        for (const auto& a_point : list.m_values)
        {
            auto point_element = list_element.append_child("Point");
            ODK_ASSERT(point_element);
            //ADD_DUMMY_DATA(point_element);

            auto x_element = point_element.append_child("x");
            ODK_ASSERT(x_element);
            //ADD_DUMMY_DATA(x_element);
            xpugi::setText(x_element, boost::lexical_cast<std::string>(a_point.first));

            auto y_element = point_element.append_child("y");
            ODK_ASSERT(y_element);
            //ADD_DUMMY_DATA(y_element);
            xpugi::setText(y_element, boost::lexical_cast<std::string>(a_point.second));
        }
    }

    PointList Property::parsePointListNode(const pugi::xml_node& type_node)
    {
        PointList point_list;
        auto list_nodes = type_node.children("PointList");
        if (list_nodes.begin() != list_nodes.end())
        {
            auto list_node = *list_nodes.begin();
            for (auto node : list_node.children())
            {
                if (node.type() == pugi::node_element)
                {
                    if (odk::strequal(node.name(), "Point"))
                    {
                        bool okay = false;
                        double x(std::numeric_limits<double>::quiet_NaN());
                        double y(std::numeric_limits<double>::quiet_NaN());
                        auto x_node = xpugi::getChildElementByTagName(node, "x");
                        auto y_node = xpugi::getChildElementByTagName(node, "y");
                        if (x_node && y_node)
                        {
                            try
                            {
                                std::string x_string = xpugi::getText(x_node);
                                std::string y_string = xpugi::getText(y_node);
                                boost::algorithm::trim(x_string);
                                boost::algorithm::trim(y_string);
                                x = boost::lexical_cast<double>(x_string);
                                y = boost::lexical_cast<double>(y_string);
                                okay = true;
                            }
                            catch (boost::bad_lexical_cast &)
                            {
                            }
                        }
                        ODK_ASSERT(okay);
                        if (!okay)
                        {
                            ODKLOG_WARN("PointList::Point has incompatible content")
                        }
                        point_list.m_values.push_back(std::make_pair(x, y));
                    }
                    else
                    {
                        ODKLOG_DEBUG("Warning: " << list_node.name() << " has unknown node " << node.name())
                    }
                }
            }
        }
        else
        {
            std::string error = std::string("Warning: ") + type_node.name() +
                " has no PointList node";
            ODKLOG_DEBUG(error.c_str())
        }
        return point_list;
    }

    void Property::appendChannelIdListNode(pugi::xml_node parent) const
    {
        const ChannelIDList& list = getChannelIDListValue();

        auto list_element = parent.append_child("ChannelIDList");
        ODK_ASSERT(list_element);
        for (const auto& ch_id : list.m_values)
        {
            auto id_element = list_element.append_child("ChannelID");
            ODK_ASSERT(id_element);
            xpugi::setText(id_element, boost::lexical_cast<std::string>(ch_id));
        }
    }

    ChannelIDList Property::parseChannelIdListNode(const pugi::xml_node& type_node)
    {
        ChannelIDList list;
        auto list_nodes = type_node.children("ChannelIDList");
        if (list_nodes.begin() != list_nodes.end())
        {
            auto& list_node = *list_nodes.begin();
            for (auto node : list_node.children("ChannelID"))
            {
                ChannelID id = node.text().as_ullong();
                list.m_values.push_back(id);
            }
        }
        else
        {
            ODKLOG_DEBUG("Warning: " << type_node.name() << " has no ChannelIDList node");
        }
        return list;
    }

    std::string Property::unitToString(const std::string& unit_string) const
    {
        if (unit_string.empty())
        {
            return "";
        }
        return " " + unit_string;
    }
    std::string Property::valueToString() const
    {
        switch (m_type)
        {
            case SCALAR:
            {
                odk::Scalar scalar = getScalarValue();
                return boost::lexical_cast<std::string>(scalar.m_val) + unitToString(scalar.m_unit);
            }
            case RATIONAL:
            {
                auto rational = getRationalValue();
                return boost::lexical_cast<std::string>(rational.m_val) + unitToString(rational.m_unit);
            }
            case RANGE:
            {
                odk::Range range = getRangeValue();
                return "(" + boost::lexical_cast<std::string>(range.m_min) + unitToString(range.m_min_unit)
                    + ", " + boost::lexical_cast<std::string>(range.m_max) + unitToString(range.m_max_unit)
                    + ")";
            }
            case FLOATING_POINT_NUMBER_LIST:
            {
                odk::DoubleList list = getDoubleListValue();
                std::uint32_t num_of_elems = static_cast<std::uint32_t>(list.m_values.size());
                std::uint32_t index = 0;
                std::string ret_string{};
                for (double v : list.m_values)
                {
                    ret_string.append(boost::lexical_cast<std::string>(v));
                    ++index;
                    if (index < num_of_elems)
                    {
                        ret_string.append(", ");
                    }
                }
                return ret_string;
            }
            case STRING_LIST:
            {
                odk::StringList list = getStringListValue();
                std::uint32_t num_of_elems = static_cast<std::uint32_t>(list.m_values.size());
                std::uint32_t index = 0;
                std::string ret_string{};
                for (const StringList::ValueType& str : list.m_values)
                {
                    ret_string.append(str);
                    ++index;
                    if (index < num_of_elems)
                    {
                        ret_string.append(", ");
                    }
                }
                return ret_string;
            }
            case PROPERTY_LIST:
            {
                odk::PropertyList list = getPropertyListValue();
                std::uint32_t num_of_elems = static_cast<std::uint32_t>(list.size());
                std::uint32_t index = 0;
                std::string ret_string{};
                for (Property prop : list)
                {
                    ret_string.append(prop.valueToString());
                    ++index;
                    if (index < num_of_elems)
                    {
                        ret_string.append(", ");
                    }
                }
                return ret_string;
            }
            case POINT_LIST:
            {
                odk::PointList list = getPointListValue();
                std::uint32_t num_of_elems = static_cast<std::uint32_t>(list.m_values.size());
                std::uint32_t index = 0;
                std::string ret_string{};
                ret_string.append("(");
                for (PointList::ValueType point : list.m_values)
                {
                    ret_string.append("[");
                    ret_string.append(boost::lexical_cast<std::string>(point.first));
                    ret_string.append(", ");
                    ret_string.append(boost::lexical_cast<std::string>(point.second));
                    ret_string.append("]");
                    ++index;
                    if (index < num_of_elems)
                    {
                        ret_string.append(", ");
                    }
                }
                ret_string.append(")");
                return ret_string;
            }
            case CHANNEL_ID_LIST:
            {
                auto list = getChannelIDListValue();
                std::uint32_t num_of_elems = static_cast<std::uint32_t>(list.m_values.size());
                std::uint32_t index = 0;
                std::string ret_string{};
                for (const ChannelID id : list.m_values)
                {
                    ret_string.append(boost::lexical_cast<std::string>(id));
                    ++index;
                    if (index < num_of_elems)
                    {
                        ret_string.append(", ");
                    }
                }
                return ret_string;
            }
            default:
                return getStringValue();
        }
    }

    Scalar::Scalar()
        : m_val(0.0)
        , m_unit()
    {}

    Scalar::Scalar(const double& val, const std::string & unit)
        : m_val(val)
        , m_unit(unit)
    {
    }

    bool Scalar::operator==(Scalar const& other) const
    {
        return (m_val == other.m_val)
            && (m_unit == other.m_unit);
    }

    Rational::Rational()
        : m_val()
        , m_unit()
    {}

    Rational::Rational(const value_type & val, const std::string & unit)
        : m_val(val)
        , m_unit(unit)
    {
    }

    bool Rational::operator==(Rational const& other) const
    {
        return (m_val == other.m_val)
            && (m_unit == other.m_unit);
    }

    DecoratedNumber::DecoratedNumber()
        : m_val(0.0)
        , m_prefix()
        , m_suffix()
    {
    }

    DecoratedNumber::DecoratedNumber(const double& val, const std::string& prefix, const std::string& suffix)
        : m_val(val)
        , m_prefix(prefix)
        , m_suffix(suffix)
    {
    }

    bool DecoratedNumber::operator==(DecoratedNumber const& other) const
    {
        return (m_val == other.m_val)
            && (m_prefix == other.m_prefix)
            && (m_suffix == other.m_suffix);
    }

    Range::Range()
        : m_min(0.0)
        , m_max(0.0)
        , m_min_unit("")
        , m_max_unit("")
    {}

    Range::Range(const double& min, const double& max, const std::string & min_unit, const std::string & max_unit)
        : m_min(min)
        , m_max(max)
        , m_min_unit(min_unit)
        , m_max_unit(max_unit)
    {
    }

    Range::Range(const double& min, const double& max, const std::string & unit)
        : m_min(min)
        , m_max(max)
        , m_min_unit(unit)
        , m_max_unit(unit)
    {
    }

    bool Range::operator==(Range const& other) const
    {
        return (m_min == other.m_min)
            && (m_max == other.m_max)
            && (m_min_unit == other.m_min_unit)
            && (m_max_unit == other.m_max_unit);
    }

} // namespace neoncfg
