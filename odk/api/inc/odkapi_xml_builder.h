// Copyright DEWETRON GmbH 2023
#pragma once

#ifdef __GNUC__
#  if __GNUC__ >= 8
#    define HAS_TO_CHARS_INT // available on GCC >= 8.0
#  endif
#  if __GNUC__ >= 11
#    define HAS_TO_CHARS_FLOAT // available on GCC >= 11.0
#  endif
#else
#  define HAS_TO_CHARS_INT
#  define HAS_TO_CHARS_FLOAT
#endif

#include "odkuni_assert.h"
#include <algorithm>
#if defined(HAS_TO_CHARS_INT) || defined(HAS_TO_CHARS_FLOAT)
#  include <charconv>
#endif
#if !defined(HAS_TO_CHARS_INT) || !defined(HAS_TO_CHARS_FLOAT)
#  include <cstdio>
#endif
#include <ostream>
#include <string_view>
#include <type_traits>

namespace odk
{
    namespace xml_builder
    {
        class Element;
        class Document;

        template<typename T>
        class Attribute
        {
            friend class Element;
        public:
            Attribute(const std::string_view& name, const T& value) noexcept
                : m_name(name)
                , m_value(value)
            {
            }
        private:
            std::string_view m_name;
            const T& m_value;
        };

        class Node
        {
        public:
            Element append_child(const std::string_view& name);
            template<typename... Attributes>
            Element append_child(const std::string_view& name, Attributes&&... attributes);
        protected:
            Node(Document& doc) noexcept : m_document(doc) {}
            Node(const Node&) = delete;
            Document& m_document;
            bool m_has_children = false;
        };

        class Document : public Node
        {
            friend class Element;
        public:
            Document(std::ostream& out);
            ~Document();
        protected:
            enum
            {
                BUFFER_SIZE = 768 // should at least be the size required to store std::to_chars output
            };
            void unwrite(std::size_t num = 1);

            bool ensure_buffer(std::size_t num)
            {
                if (m_buffer_pos + num <= m_buffer + BUFFER_SIZE)
                {
                    return true;
                }
                flush();
                return num <= BUFFER_SIZE;
            }

            void write(char c)
            {
                ensure_buffer(1);
                write_unchecked(c);
            }
            void write(const std::string_view& v);
            void flush();

            template<typename... Parts>
            void writeList(Parts&&... parts)
            {
                const std::size_t combined_length = (len(std::forward<Parts>(parts)) + ...);
                if (ensure_buffer(combined_length))
                {
                    (write_unchecked(std::forward<Parts>(parts)), ...);
                }
                else
                {
                    (write(std::forward<Parts>(parts)), ...);
                }
            }
            void write_escaped(const std::string_view& text);
            std::ostream& m_out;
        private:
            constexpr static std::size_t len(char) { return 1; }
            constexpr static std::size_t len(const std::string_view & s) { return s.size(); }
            void write_unchecked(char c)
            {
                *m_buffer_pos++ = c;
            }
            void write_unchecked(const std::string_view& v)
            {
                m_buffer_pos = std::copy_n(v.data(), v.size(), m_buffer_pos);
            }
            char m_buffer[BUFFER_SIZE];
            char* m_buffer_pos;
        };

        class Element : public Node
        {
            friend class Node;
        public:
            ~Element();
            void append_text(const std::string_view& text);

            template<typename T>
            void append_attribute(const std::string_view& name, const T& value)
            {
                append_attribute(Attribute(name, value));
            }

            template<typename T>
            void append_attribute(const Attribute<T>& a)
            {
                ODK_ASSERT(!m_has_children);
                m_document.unwrite(1); // overwrite last > sign
                print_attribute(a);
                m_document.write('>');
            }
        protected:
            Element(Document&, const std::string_view& name);
            template<typename... Attributes>
            Element(Document& doc, const std::string_view& name, Attributes&&... attributes)
                : Node(doc)
                , m_name(name)
            {
                doc.writeList('<', name);
                (print_attribute(std::forward<Attributes>(attributes)), ...);
                doc.write('>');
            }
        private:
            template<typename T>
            void print_attribute(const Attribute<T>& a) const
            {
                ODK_ASSERT(!a.m_name.empty());
                // write ' {a.m_name}="'
                m_document.writeList(' ', a.m_name, "=\"");
                if constexpr (std::is_constructible_v<std::string_view, T>)
                {
                    m_document.write_escaped(a.m_value);
                }
                else if constexpr (std::is_same_v<bool, T>)
                {
                    m_document.write(a.m_value ? "true" : "false");
                }
                else if constexpr (std::is_integral_v<T>)
                {
#ifdef HAS_TO_CHARS_INT
                    // It is faster to convert integers to string than using iostreams
                    constexpr std::size_t max_len = 32; // maximum of a signed 64-bit integer conversion
                    static_assert(Document::BUFFER_SIZE >= max_len);
                    ODK_VERIFY(m_document.ensure_buffer(max_len));
                    auto res = std::to_chars(m_document.m_buffer_pos, m_document.m_buffer_pos + max_len, a.m_value);
                    ODK_ASSERT(res.ec == std::errc());
                    m_document.m_buffer_pos = res.ptr;
#else
                    m_document.write(std::to_string(a.m_value));
#endif
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    constexpr std::size_t max_len = 64;
                    static_assert(Document::BUFFER_SIZE >= max_len);
                    ODK_VERIFY(m_document.ensure_buffer(max_len));
#ifdef HAS_TO_CHARS_FLOAT
                    constexpr int precision = std::is_same_v<T, float> ? 9 : 17;
                    auto res = std::to_chars(m_document.m_buffer_pos, m_document.m_buffer_pos + max_len, a.m_value, std::chars_format::general, precision);
                    ODK_ASSERT(res.ec == std::errc());
                    m_document.m_buffer_pos = res.ptr;
#else
                    constexpr const char* format = std::is_same_v<T, float> ? "%.9g" : "%.17g";
                    m_document.m_buffer_pos += std::snprintf(m_document.m_buffer_pos, max_len, format, a.m_value);
#endif
                }
                else
                {
                    m_document.flush();
                    m_document.m_out << a.m_value;
                }
                m_document.write('\"');
            }

            std::string_view m_name;
        };

        template<typename... Attributes>
        Element Node::append_child(const std::string_view& name, Attributes&&... attributes)
        {
            m_has_children = true;
            return Element(m_document, name, std::forward<Attributes>(attributes)...);
        }
    }
}

#undef HAS_TO_CHARS_INT
#undef HAS_TO_CHARS_FLOAT
