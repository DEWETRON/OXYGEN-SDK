// Copyright DEWETRON GmbH 2023

#include "odkapi_xml_builder.h"

using odk::xml_builder::Attribute;
using odk::xml_builder::Document;
using odk::xml_builder::Element;
using odk::xml_builder::Node;

Element Node::append_child(const std::string_view& name)
{
    m_has_children = true;
    return Element(m_document, name);
}

Element::Element(Document& doc, const std::string_view& name)
    : Node(doc)
    , m_name(name)
{
    m_document.writeList('<', name, '>');
}

Element::~Element()
{
    if (m_has_children)
    {
        m_document.writeList("</", m_name, '>');
    }
    else
    {
        m_document.unwrite(1); // overwrite last > sign
        m_document.write("/>");
    }
}

void Element::append_text(const std::string_view& text)
{
    m_has_children = true;
    m_document.write_escaped(text);
}

Document::Document(std::ostream& out)
    : Node(*this)
    , m_out(out)
    , m_buffer_pos(m_buffer)
{
    static_assert(BUFFER_SIZE >= 22);
    write_unchecked("<?xml version=\"1.0\" ?>");
}

Document::~Document()
{
    flush();
    m_out.flush();
}

void Document::write_escaped(const std::string_view& text)
{
    std::string_view::const_pointer p_start = text.data();
    std::string_view::const_pointer p_cur = text.data();
    std::string_view::const_pointer p_end = text.data() + text.size();
    auto flush = [&p_start, &p_cur, this]()
        {
            write(std::string_view(p_start, p_cur - p_start));
            p_start = p_cur + 1;
        };
    for (; p_cur != p_end; ++p_cur)
    {
        switch (*p_cur)
        {
        case '\"':
            flush();
            write("&quot;");
            break;
        case '\'':
            flush();
            write("&apos;");
            break;
        case '<':
            flush();
            write("&lt;");
            break;
        case '>':
            flush();
            write("&gt;");
            break;
        case '&':
            flush();
            write("&amp;");
            break;
        default:
            break;
        }
    }
    flush();
}

void Document::unwrite(std::size_t num)
{
    if (m_buffer_pos >= m_buffer + num)
    {
        m_buffer_pos -= num;
        return;
    }
    flush();
    m_out.seekp(-static_cast<std::streamoff>(num), std::ios_base::cur);
}

void Document::flush()
{
    if (m_buffer == m_buffer_pos)
    {
        return;
    }
    m_out.write(m_buffer, m_buffer_pos - m_buffer);
    m_buffer_pos = m_buffer;
}

void Document::write(const std::string_view& sv)
{
    if (ensure_buffer(sv.size()))
    {
        write_unchecked(sv);
    }
    else
    {
        m_out.write(sv.data(), sv.size());
    }
}
