// Copyright (c) DEWETRON GmbH 2015

#include "odkuni_xpugixml.h"

#include "odkuni_assert.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <sstream>
#include <fstream>

namespace xpugi
{
    namespace priv
    {
        void getTextImpl(pugi::xml_node node, std::string& text)
        {
            switch (node.type())
            {
            case pugi::node_element:
            {
                for (pugi::xml_node::iterator child = node.begin();
                    child != node.end(); ++child)
                {
                    getTextImpl(*child, text);
                }
            }
            break;

            //case pugi::node_pi:
            //case pugi::node_comment:
            case pugi::node_cdata:
                text += node.value();
                break;
            case pugi::node_pcdata:
                text += node.value();
                break;
            default:
                //nothing
                break;
            }
        }
    }

    pugi::xml_document_ptr createDocument()
    {
        return boost::make_shared<pugi::xml_document>();
    }

    pugi::xml_document_ptr createDocument(const pugi::xml_document& src_doc)
    {
        auto new_doc = createDocument();
        new_doc->reset(src_doc);
        return new_doc;
    }

    pugi::xml_document_ptr createDocument(pugi::xml_document_ptr src_doc)
    {
        return createDocument(*src_doc);
    }

    pugi::xml_document_ptr createDocument(const std::string& src, bool& success)
    {
        auto new_doc = createDocument();
        success = new_doc->load_string(src.c_str());
        return new_doc;
    }

    bool save(pugi::xml_node node,
              const std::string& xml_file,
              bool pretty)
    {
        std::ofstream out;
        out.open(xml_file, std::ios_base::out|std::ios_base::binary);
        if (!out.is_open())
        {
            return false;
        }
        pugi::xml_writer_stream writer(out);

        if (pretty)
        {
            node.print(writer);
        }
        else
        {
            node.print(writer, "", pugi::format_raw);
        }

        out.close();
        return true;
    }

    bool saveDoc(const pugi::xml_document& doc,
              const std::string& xml_file,
              bool pretty /*= false*/)
    {
        std::ofstream out;
        out.open(xml_file, std::ios_base::out | std::ios_base::binary);
        if (!out.is_open())
        {
            return false;
        }
        pugi::xml_writer_stream writer(out);

        if (pretty)
        {
            doc.save(writer);
        }
        else
        {
            doc.save(writer, "", pugi::format_raw);
        }

        out.close();
        return true;
    }

    std::string getText(pugi::xml_node node)
    {
        std::string text;

        priv::getTextImpl(node, text);

        // trim whitespace and return
        boost::algorithm::trim(text);
        return text;
    }

    std::string getText(pugi::xml_attribute attr)
    {
        std::string text = attr.value();
        // trim whitespace and return
        boost::algorithm::trim(text);
        return text;
    }

    std::string getText(pugi::xpath_node xnode)
    {
        pugi::xml_node node = xnode.node();
        if (node)
        {
            return getText(node);
        }
        pugi::xml_attribute attr = xnode.attribute();
        if (attr)
        {
            return getText(attr);
        }
        return "";
    }

    std::string getRawText(pugi::xml_node node)
    {
        std::string text;
        priv::getTextImpl(node, text);
        return text;
    }

    std::string getRawText(pugi::xml_attribute attr)
    {
        std::string text = attr.value();
        return text;
    }

    std::string getRawText(pugi::xpath_node xnode)
    {
        pugi::xml_node node = xnode.node();
        if (node)
        {
            return getRawText(node);
        }
        pugi::xml_attribute attr = xnode.attribute();
        if (attr)
        {
            return getRawText(attr);
        }
        return "";
    }

    std::string getTextFromNode(pugi::xml_node node)
    {
        std::string text;

        switch (node.type())
        {
            case pugi::node_element:
                {
                    for (pugi::xml_node::iterator child = node.begin();
                        child != node.end(); ++child)
                    {
                        if (child->type() == pugi::node_pcdata)
                        {
                            text += child->value();
                        }
                    }
                }
                break;

            case pugi::node_pi:
            case pugi::node_comment:
            case pugi::node_cdata:
                text = node.value();
                break;
            case pugi::node_pcdata:
                text = node.value();
                break;
            default:
                //nothing
                break;
        }

        // trim whitespace and return
        boost::algorithm::trim(text);
        return text;
    }

    std::string getTextFromNode(pugi::xml_attribute attr)
    {
        return getText(attr);
    }

    std::string getTextFromNode(pugi::xpath_node xnode)
    {
        pugi::xml_node node = xnode.node();
        if (node)
        {
            return getTextFromNode(node);
        }
        pugi::xml_attribute attr = xnode.attribute();
        if (attr)
        {
            return getText(attr);
        }
        return "";
    }

     std::string getRawTextFromNode(pugi::xml_node node)
    {
        std::string text;

        switch (node.type())
        {
            case pugi::node_element:
                {
                    for (pugi::xml_node::iterator child = node.begin();
                        child != node.end(); ++child)
                    {
                        if (child->type() == pugi::node_pcdata)
                        {
                            text += child->value();
                        }
                    }
                }
                break;

            case pugi::node_pi:
            case pugi::node_comment:
            case pugi::node_cdata:
                text = node.value();
                break;
            case pugi::node_pcdata:
                text = node.value();
                break;
            default:
                //nothing
                break;
        }
        return text;
    }

    std::string getRawTextFromNode(pugi::xml_attribute attr)
    {
        return getRawText(attr);
    }

    std::string getRawTextFromNode(pugi::xpath_node xnode)
    {
        pugi::xml_node node = xnode.node();
        if (node)
        {
            return getRawTextFromNode(node);
        }
        pugi::xml_attribute attr = xnode.attribute();
        if (attr)
        {
            return getRawText(attr);
        }
        return "";
    }

    void setText(pugi::xml_node node, const std::string& text)
    {
        if (text.empty())
        {
            return;
        }
        setText(node, text.c_str());
    }

    void setText(pugi::xml_node node, const char* text)
    {
        if (!text || *text == '\0')
        {
            return;
        }

        switch (node.type())
        {
            case pugi::node_element:
                {
                    removeAllChildren(node);
                }
                node.append_child(pugi::node_pcdata).set_value(text);
                break;

            case pugi::node_pi:
            case pugi::node_comment:
            case pugi::node_cdata:
            case pugi::node_pcdata:
                node.set_value(text);
                break;
            default:
                //nothing
                break;
        }
    }

    void setText(pugi::xml_attribute attr, const std::string& text)
    {
        attr.set_value(text.c_str());
    }

    void setText(pugi::xml_attribute attr, const char* text)
    {
        attr.set_value(text);
    }

    std::string getInnerXML(pugi::xml_node node)
    {
        std::ostringstream s;
        for (auto& c : node.children())
        {
            c.print(s, "", pugi::format_raw);
        }
        return s.str();
    }

    std::string toXML(pugi::xml_document_ptr doc, bool pretty)
    {
        return toXML(*doc, pretty);
    }

    std::string toXML(pugi::xml_document& doc, bool pretty)
    {
        std::stringstream ss;
        if (pretty)
        {
            doc.save(ss);
        }
        else
        {
            doc.save(ss, "", pugi::format_raw);
        }

        return ss.str();
    }


    std::string toXML(pugi::xml_node node, bool pretty)
    {
        std::stringstream ss;
        if (pretty)
        {
            node.print(ss);
        }
        else
        {
            node.print(ss, "", pugi::format_raw);
        }

        return ss.str();
    }

    std::string toXML(pugi::xpath_node xnode, bool pretty)
    {
        pugi::xml_node node = xnode.node();
        if (node)
        {
            return toXML(node, pretty);
        }
        return "";
    }

    std::string xmlPrettyPrint(const std::string& xml_txt)
    {
        pugi::xml_document doc;
        auto parse_result = doc.load_string(xml_txt.c_str());
        if (parse_result.status == pugi::status_ok)
        {
            return toXML(doc, true);
        }

        return xml_txt;
    }

    std::size_t getChildCount(pugi::xml_node node)
    {
        return std::distance(node.children().begin(), node.children().end());
    }

    pugi::xml_node getChildNodeByTagName(pugi::xml_node node, const std::string& name)
    {
        if (node)
        {
            for (auto child_it = node.begin();
                 child_it != node.end(); ++child_it)
            {
                if (boost::equals(child_it->name(), name))
                {
                    return *child_it;
                }
            }
        }
        return pugi::xml_node();
    }

    pugi::xml_node getChildElementByTagName(pugi::xml_node node, const std::string& name)
    {
        if (node)
        {
            auto children = node.children(name.c_str());
            if (children.begin() != children.end())
            {
                auto child = *children.begin();
                if (child.type() == pugi::node_element)
                {
                    return child;
                }
            }
        }
        return pugi::xml_node();
    }

    pugi::xpath_node selectSingleNode(pugi::xml_node node, const std::string& xpath_expr)
    {
        return node.select_node(xpath_expr.c_str());
    }

    pugi::xpath_node selectSingleNode(pugi::xpath_node node, const std::string& xpath_expr)
    {
        return node.node().select_node(xpath_expr.c_str());
    }

    pugi::xpath_node selectSingleNode(pugi::xml_document_ptr node, const std::string& xpath_expr)
    {
        if (node)
        {
            return node->select_node(xpath_expr.c_str());
        }
        return pugi::xpath_node();
    }

    pugi::xpath_node_set selectNodes(pugi::xml_node node, const std::string& xpath_expr)
    {
        return node.select_nodes(xpath_expr.c_str());
    }

    pugi::xpath_node_set selectNodes(pugi::xml_document_ptr node, const std::string& xpath_expr)
    {
        if (node)
        {
            return node->select_nodes(xpath_expr.c_str());
        }
        return pugi::xpath_node_set();
    }

    pugi::xml_node appendNode(pugi::xml_document& dest_doc, pugi::xml_node src_node, bool deep)
    {
        pugi::xml_node cloned_node;
        if (deep)
        {
            cloned_node = dest_doc.append_copy(src_node);
        }
        else
        {
            cloned_node = dest_doc.append_child(src_node.name());
            for (pugi::xml_attribute_iterator it_attr = src_node.attributes_begin();
                it_attr != src_node.attributes_end(); ++it_attr)
            {
                cloned_node.append_copy(*it_attr);
            }
        }
        return cloned_node;
    }

    pugi::xml_node appendNode(pugi::xml_node dest_node, pugi::xml_node src_node, bool deep)
    {
        pugi::xml_node cloned_node;
        if (deep)
        {
            cloned_node = dest_node.append_copy(src_node);
        }
        else
        {
            cloned_node = dest_node.append_child(src_node.name());
            for (pugi::xml_attribute_iterator it_attr = src_node.attributes_begin();
                it_attr != src_node.attributes_end(); ++it_attr)
            {
                cloned_node.append_copy(*it_attr);
            }
        }
        return cloned_node;
    }

    void appendAllChildren(pugi::xml_node dest_node, pugi::xml_node src_node, bool deep)
    {
        for (auto src_it = src_node.begin();
            src_it != src_node.end(); ++src_it)
        {
            appendNode(dest_node, *src_it, deep);
        }
    }

    void removeAllChildren(pugi::xml_node parent)
    {
        std::vector<pugi::xml_node> children_to_remove;

        // overwrite the complete content
        for (pugi::xml_node::iterator child = parent.children().begin();
            child != parent.children().end(); ++child)
        {
            children_to_remove.push_back(*child);
        }

        for (auto it = children_to_remove.begin();
            it != children_to_remove.end(); ++it)
        {
            parent.remove_child(*it);
        }
    }

    pugi::xml_node replaceCopy(pugi::xml_node parent_node, pugi::xml_node new_node, pugi::xml_node old_node)
    {
        if (parent_node)
        {
            auto inserted = parent_node.insert_copy_before(new_node, old_node);
            if (inserted)
            {
                parent_node.remove_child(old_node);
                return inserted;
            }
            return pugi::xml_node();
        }
        return pugi::xml_node();
    }

    xml_element::xml_element()
        : pugi::xml_node()
    {}

    xml_element::xml_element(pugi::xml_node node)
        : pugi::xml_node(static_cast<xml_element*>(&node)->_root)
    {
        if ((type() != pugi::node_element)
           && (type() != pugi::node_null))
        {
            throw xml_element_exception("Invalid node type");
        }
    }

    std::string xml_element::getNodeName() const
    {
        return name();
    }

    bool xml_element::hasAttributes() const
    {
        return has_attributes();
    }

    bool xml_element::hasAttribute(const std::string& attr_name) const
    {
        return has_attribute(attr_name);
    }

    std::string xml_element::getAttribute(const std::string& attr_name) const
    {
        return get_attribute(attr_name);
    }

    void xml_element::setAttribute(const std::string& attr_name, const std::string& attr_value)
    {
        set_attribute(attr_name, attr_value);
    }

    bool xml_element::hasChildNodes() const
    {
        return has_child_nodes();
    }

    bool xml_element::has_attributes() const
    {
        pugi::xml_attribute attr = first_attribute();
        return attr != 0;
    }

    bool xml_element::has_attribute(const std::string& attr_name) const
    {
        pugi::xml_attribute attr = attribute(attr_name.c_str());
        return attr != 0;
    }

    std::string xml_element::get_attribute(const std::string& attr_name) const
    {
        return attribute(attr_name.c_str()).value();
    }

    void xml_element::set_attribute(const std::string& attr_name, const std::string& attr_value)
    {
        if (has_attribute(attr_name))
        {
            pugi::xml_attribute attr = attribute(attr_name.c_str());
            attr.set_value(attr_value.c_str());
        }
        else
        {
            pugi::xml_attribute attr = append_attribute(attr_name.c_str());
            attr.set_value(attr_value.c_str());
        }
    }

    bool xml_element::has_child_nodes() const
    {
        pugi::xml_node node = first_child();
        return node != 0;
    }

    xml_element xml_element::replace_copy(xml_element new_elem, xml_element old_elem)
    {
        return replaceCopy(*this, new_elem, old_elem);
    }

    xpugi::xml_element xml_element::append_child(const std::string& name)
    {
        return xml_node::append_child(name.c_str());
    }

    pugi::xml_node xml_element::append_child(pugi::xml_node_type type)
    {
        return xml_node::append_child(type);
    }

    xpugi::xml_element xml_element::prepend_child(const std::string& name)
    {
        return xml_node::prepend_child(name.c_str());
    }

    pugi::xml_node xml_element::prepend_child(pugi::xml_node_type type)
    {
        return xml_node::prepend_child(type);
    }

    void setNewAttribute(pugi::xml_node node, const std::string& name, const std::string& value)
    {
        auto attribute = node.append_attribute(name.c_str());
        ODK_ASSERT(attribute);
        bool success = attribute.set_value(value.c_str());
        ODK_ASSERT(success);
    }

} // xpugi
