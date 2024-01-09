// Copyright DEWETRON GmbH 2023
#include "odkapi_xml_builder.h"

#include "odkuni_xpugixml.h"

#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_SUITE(xml_builder_test_suite)

class TestDocument : odk::xml_builder::Document
{
public:
    TestDocument(std::ostringstream& s) : Document(s)
    {
    }
    using Document::unwrite;
    using Document::flush;
    using Document::write;
};

BOOST_AUTO_TEST_CASE(DocumentTest)
{
    std::ostringstream stream;
    {
        TestDocument test(stream);
        test.write('A');
    }
    BOOST_CHECK_EQUAL(stream.str(), "<?xml version=\"1.0\" ?>A");
    stream.str("");
    {
        TestDocument test(stream);
        test.write("A");
        test.unwrite();
    }
    BOOST_CHECK_EQUAL(stream.str(), "<?xml version=\"1.0\" ?>");
    stream.str("");
    {
        TestDocument test(stream);
        test.write("A");
        test.flush(); // we want an unwrite on an empty buffer
        test.unwrite();
        test.write("B");
    }
    BOOST_CHECK_EQUAL(stream.str(), "<?xml version=\"1.0\" ?>B");
}

BOOST_AUTO_TEST_CASE(SingleElement)
{
    std::ostringstream stream;

    {
        odk::xml_builder::Document doc(stream);
        doc.append_child("RootWithVeryLongName0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
    }

    pugi::xml_document doc;
    BOOST_REQUIRE(doc.load_string(stream.str().c_str()).status == pugi::status_ok);
    BOOST_CHECK_EQUAL(doc.document_element().name(), "RootWithVeryLongName0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
}

BOOST_AUTO_TEST_CASE(SingleElementWithAttribute)
{
    std::ostringstream stream;

    {
        using odk::xml_builder::Attribute;
        odk::xml_builder::Document doc(stream);
        doc.append_child("Root", Attribute("a", 1), Attribute("b", "hello & \"world\""), Attribute("c", true));
    }

    pugi::xml_document doc;
    BOOST_REQUIRE(doc.load_string(stream.str().c_str()).status == pugi::status_ok);
    BOOST_CHECK_EQUAL(doc.document_element().name(), "Root");
    auto a = doc.document_element().first_attribute();
    BOOST_CHECK_EQUAL(a.name(), "a");
    BOOST_CHECK_EQUAL(a.as_int(), 1);
    auto b = a.next_attribute();
    BOOST_CHECK_EQUAL(b.name(), "b");
    BOOST_CHECK_EQUAL(b.as_string(), "hello & \"world\"");
    auto c = b.next_attribute();
    BOOST_CHECK_EQUAL(c.name(), "c");
    BOOST_CHECK_EQUAL(c.as_bool(), true);
}

BOOST_AUTO_TEST_CASE(SingleElementWithDynamicAttributes)
{
    std::ostringstream stream;

    {
        using odk::xml_builder::Attribute;
        odk::xml_builder::Document doc(stream);
        auto root = doc.append_child("Root", Attribute("a", -5));
        root.append_attribute("b", "hello & \"world\"");
    }

    pugi::xml_document doc;
    BOOST_REQUIRE(doc.load_string(stream.str().c_str()).status == pugi::status_ok);
    BOOST_CHECK_EQUAL(doc.document_element().name(), "Root");
    auto a = doc.document_element().first_attribute();
    BOOST_CHECK_EQUAL(a.name(), "a");
    BOOST_CHECK_EQUAL(a.as_int(), -5);
    auto b = a.next_attribute();
    BOOST_CHECK_EQUAL(b.name(), "b");
    BOOST_CHECK_EQUAL(b.as_string(), "hello & \"world\"");
}

BOOST_AUTO_TEST_CASE(Children)
{
    std::ostringstream stream;

    {
        odk::xml_builder::Document doc(stream);
        auto root = doc.append_child("Root");
        root.append_child("A");
        using odk::xml_builder::Attribute;
        root.append_child("B", Attribute("value", 123.4));
        root.append_child("C0123456789ABCDEF0123456789ABCDEF", Attribute("v2", 123));
    }

    pugi::xml_document doc;
    BOOST_REQUIRE(doc.load_string(stream.str().c_str()).status == pugi::status_ok);
    BOOST_CHECK_EQUAL(doc.document_element().name(), "Root");
    auto a = doc.document_element().first_child();
    BOOST_CHECK_EQUAL(a.name(), "A");
    auto b = a.next_sibling();
    BOOST_CHECK_EQUAL(b.name(), "B");
    BOOST_CHECK_CLOSE(b.attribute("value").as_double(), 123.4, 1e-9);
    auto c = b.next_sibling();
    BOOST_CHECK_EQUAL(c.name(), "C0123456789ABCDEF0123456789ABCDEF");
    BOOST_CHECK_EQUAL(c.attribute("v2").as_int(), 123);
}

BOOST_AUTO_TEST_CASE(Text)
{
    std::ostringstream stream;

    {
        odk::xml_builder::Document doc(stream);
        auto root = doc.append_child("Root");
        root.append_child("A").append_text("<A>'text'");
    }

    pugi::xml_document doc;
    BOOST_REQUIRE(doc.load_string(stream.str().c_str()).status == pugi::status_ok);
    BOOST_CHECK_EQUAL(doc.document_element().name(), "Root");
    BOOST_CHECK_EQUAL(doc.document_element().first_child().name(), "A");
    BOOST_CHECK_EQUAL(doc.document_element().first_child().text().as_string(), "<A>'text'");
}

BOOST_AUTO_TEST_SUITE_END()
