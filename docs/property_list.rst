.. _property_list_type:

=================
odk::PropertyList
=================


Property list is a flexible container that can be used to exchange structured data
between Oxygen and its plugins. For example it is used in custom plugin requests
to send request parameters and return values beween a QML item and its
associated plugin.

--------------------
Usage in C++ and QML
--------------------

Property list is a sorted list of Key-Value pairs with typed values.
The methods described in this section are identical for instances in C++ and QML.

*********************
Supported Value Types
*********************

The data types described in this section are supported for values in the list and
are referenced by the placeholder XY in the following document:

Bool
    Boolean value

    Default value: false

Signed
    Signed integer value

    C++ type: std::int64_t

    QML type: Number; **cannot precisely represent the full 64bit range**

    Default value: 0

Unsigned
    Unsigned integer value

    C++ type: std::uint64_t

    QML type: Number; **cannot precisely represent the full 64bit range**

    Default value: 0

Double
    Double precision floating point value

    C++ type: double

    QML type: Number

    Default value: NaN

String
    String of Unicode characters. This can be used to store xml documents if required.
    
    C++ type: std::string in **UTF-8 encoding**

    QML type: String

    Default value: ""

Scalar
    Numeric value with an associated unit string (usually SI compatible)

    C++ type: odk::Scalar

    QML type: object with properties value (Number) and unit (String)

    Default value: value 0 with no unit

ChannelId
    Unique id of an oxygen channel.

    C++ type: std::uint64_t
    
    QML type: **string**; string representation of the id

    Default value: 0 or ""

PropertyList
    A property list value, which can be used to construct hierarchical structures.
    Values of this type are always copied, therefore it is not possible to manipulate
    a list in place.

    Default value: empty list


*************
Basic Methods
*************

empty()
    returns true if the container constains no elements

clear()
    removes all elements in the container

setXY(name, value)
    appends a value of type XY to the list using key name.
    If name is not empty any other items with same name are removed.

******************
Index-based Access
******************

If values are appended with empty keys, the values are accessible by their 0-based index.

size()
    returns the number of values in the container

getXY(index)
    returns the value at position index

****************
Key-based Access
****************

Values that are stored using a unique key can be retrieved using key-based methods.

containsPropery(name)
    returns true if key name is associated with a value

getXY(name)
    returns the value stored under this name, if the types matches.


------------------------
QML Specific Information
------------------------

An object of type PropertyList can be created in JavaScript using

``var pl = plugin.createPropertyList();``


---------------------
Advanced Usage in C++
---------------------

The C++ interface provides additional methods to get direct
access to the underlying odk::Property objects, update
values in place and handle additional data types.

These features are usually not necessary to implement plugins using
the provided framework classes.
