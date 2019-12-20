// Copyright (c) DEWETRON GmbH 2017
#ifndef __ODKBASE_BASIC_VALUES__
#define __ODKBASE_BASIC_VALUES__

#include "odkbase_if_value.h"
#include <cstdint>
#include <string>

/**
 * This header contains interfaces for the value objects used in the plugin API.
 * All of these objects are reference counted.
 * Functions returning such objects already increment the reference counter before returning the object.
 * Functions accepting values increment the reference count if the instance is required after returning from the call.
 * Notable exceptions, that reduced reference counting overhead for specific usage scenarios:
 *   - Methods starting with "ref" return a reference to an internal object with out incrementing the reference count.
 *     Therefore clients are not forced to release() such instances if they only need them for a very short time.
 *     Referenced values will become invalid if the owner value changes or is destroyed.
 *   - Methods called adopt (or methods called with the adopt flag set to true) transfer ownership of the instance from caller to callee.
 *     Therefore the caller does not have to release() his reference after the call.
 */

namespace odk
{
    class IfBooleanValue : public IfValue
    {
    public:
        virtual bool PLUGIN_API getValue() const = 0;
        virtual void PLUGIN_API set(bool value) = 0;

    public:
        static const Type type_index = Type::TYPE_BOOL;
    };
    
    class IfUIntValue : public IfValue
    {
    public:
        virtual std::uint64_t PLUGIN_API getValue() const = 0;
        virtual void PLUGIN_API set(std::uint64_t value) = 0;

    public:
        static const Type type_index = Type::TYPE_UINT;
    };

    class IfSIntValue : public IfValue
    {
    public:
        virtual std::int64_t PLUGIN_API getValue() const = 0;
        virtual void PLUGIN_API set(std::int64_t value) = 0;

    public:
        static const Type type_index = Type::TYPE_SINT;
    };

    class IfFloatingPointValue : public IfValue
    {
    public:
        virtual double PLUGIN_API getValue() const = 0;
        virtual void PLUGIN_API set(double value) = 0;

    public:
        static const Type type_index = Type::TYPE_FLOAT;
    };

    class IfComplexValue : public IfValue
    {
    public:
        virtual double PLUGIN_API getReal() const = 0;
        virtual double PLUGIN_API getImaginary() const = 0;
        virtual void PLUGIN_API set(double real, double imag) = 0;

    public:
        static const Type type_index = Type::TYPE_COMPLEX;
    };

    class IfPointValue : public IfValue
    {
    public:
        virtual double PLUGIN_API getX() const = 0;
        virtual double PLUGIN_API getY() const = 0;
        virtual void PLUGIN_API set(double x, double y) = 0;

    public:
        static const Type type_index = Type::TYPE_POINT;
    };


    class IfEnumValue : public IfValue
    {
    public:
        virtual const char* PLUGIN_API getEnumName() const = 0;
        virtual const char* PLUGIN_API getEnumValue() const = 0;
        virtual bool PLUGIN_API set(const char* enum_type, const char* enum_value) = 0;

    public:
        static const Type type_index = Type::TYPE_ENUM;
    };

    /**
     * String values are UTF-8 encoded and 0-terminated.
     * getLength returns the number of bytes (excluding the terminator) in the value.
     */
    class IfStringValue : public IfValue
    {
    public:
        virtual const char* PLUGIN_API getValue() const = 0;
        virtual int PLUGIN_API getLength() const = 0;
        virtual void PLUGIN_API set(const char* value) = 0;

    public:
        static const Type type_index = Type::TYPE_STRING;

#ifdef ODK_EXTENSION_FUNCTIONS
        typedef std::string converted_type;
        converted_type convert() const
        {
            return std::string(getValue(), getLength());
        }
#endif
    };

    
    /**
     * Represents a scalar value consisting of a floating point value and its unit as a string.
     * The unit is typically specified according to the SI format; unitless values should use an empty string or nullptr.
     */
    class IfScalarValue : public IfValue
    {
    public:
        virtual double PLUGIN_API getValue() const = 0;
        virtual const char* PLUGIN_API getUnit() const = 0;
        virtual void PLUGIN_API set(double value, const char* unit) = 0;

    public:
        static const Type type_index = Type::TYPE_SCALAR;
    };

    /**
     * Similar to IfScalarValue, but the numeric amount is stored in rational representation to reduce inaccuracies.
     */
    class IfRationalScalarValue : public IfValue
    {
    public:
        //TODO: add getValue() that converts rational to double?
        virtual std::int64_t PLUGIN_API getNumerator() const = 0;
        virtual std::int64_t PLUGIN_API getDenominator() const = 0;
        virtual const char* PLUGIN_API getUnit() const = 0;
        virtual void PLUGIN_API set(std::int64_t num, std::int64_t den, const char* unit) = 0;

    public:
        static const Type type_index = Type::TYPE_RATIONAL_SCALAR;
    };

    /**
     * like Hugi124Franz ?
     */
    class IfDecoratedNumberValue : public IfValue
    {
    public:
        virtual double PLUGIN_API getValue() const = 0;
        virtual const char* PLUGIN_API getPrefix() const = 0;
        virtual const char* PLUGIN_API getSuffix() const = 0;
        virtual void PLUGIN_API set(double value, const char* prefix, const char* suffix) = 0;

    public:
        static const Type type_index = Type::TYPE_DECORATED_NUMBER;
    };

    class IfErrorValue : public IfValue
    {
    public:
        virtual std::uint64_t PLUGIN_API getCode() const = 0;
        virtual const char* PLUGIN_API getDescription() const = 0;

        virtual void PLUGIN_API set(std::uint64_t code, const char* str) = 0;
    public:
        static const Type type_index = Type::TYPE_ERROR;
    };

    class IfXMLValue : public IfValue
    {
    public:
        virtual const char* PLUGIN_API getValue() const = 0;
        virtual int PLUGIN_API getLength() const = 0;
        virtual void PLUGIN_API set(const char* value) = 0;

    public:
        static const Type type_index = Type::TYPE_XML;

#ifdef ODK_EXTENSION_FUNCTIONS
        typedef std::string converted_type;
        converted_type convert() const
        {
            return std::string(getValue(), getLength());
        }
#endif
    };

    class IfArbitraryBinaryValue : public IfValue
    {
    public:
        virtual int PLUGIN_API getSize() const = 0;
        virtual const std::uint8_t* PLUGIN_API refData() const = 0;
        virtual void PLUGIN_API set(const std::uint8_t* data, int length) = 0;
    public:
        static const Type type_index = Type::TYPE_ARBITRARY_DATA;
    };

    /**
     * Containing a xml based block description and a raw pointer to the data itself.
     */
    class IfDataBlock : public IfValue
    {
    public:
        virtual IfXMLValue* PLUGIN_API getBlockDescription() const = 0;
        virtual int PLUGIN_API dataSize() const = 0;
        virtual const std::uint8_t* PLUGIN_API data() const = 0;

        virtual void PLUGIN_API set(IfXMLValue* descr, const std::uint8_t* data, std::uint32_t length) = 0;
    public:
        static const Type type_index = Type::TYPE_DATA_BLOCK;
    };

    /**
     * A list of IfDataBlock and meta data.
     */
    class IfDataBlockList : public IfValue
    {
    public:
        virtual IfXMLValue* PLUGIN_API getBlockListDescription() const = 0;
        virtual int PLUGIN_API getBlockCount() const = 0;
        virtual IfDataBlock* PLUGIN_API getBlock(int index) const = 0;

        virtual void PLUGIN_API set(IfXMLValue* descr, IfDataBlock** data, std::uint32_t length) = 0;
    public:
        static const Type type_index = Type::TYPE_DATA_BLOCK_LIST;
    };

}

#endif //__ODKBASE_BASIC_VALUES__
