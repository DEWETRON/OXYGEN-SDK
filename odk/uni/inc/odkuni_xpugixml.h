// Copyright (c) DEWETRON GmbH 2015
#pragma once

#include "odkuni_xpugixml_fwd.h"
#include "odkuni_defines.h"
#include <pugixml.hpp>
#include <boost/function.hpp>
#include <string>

/**
 * Dewetron extensions for pugixml.
 */

namespace xpugi
{

    /**
     * Allocate an empty pugi document
     * and return a shared ptr
     */
    pugi::xml_document_ptr createDocument();

    /**
     * Create a copy of an existing pugi::xml_document
     * and return a shared ptr the newly allocated one.
     */
    pugi::xml_document_ptr createDocument(const pugi::xml_document& src_doc);

    /**
     * Create a copy of an existing pugi::xml_document
     * and return a shared ptr the newly allocated one.
     */
    pugi::xml_document_ptr createDocument(pugi::xml_document_ptr src_doc);

    /**
     * Create a pugi::xml_document and init it from an xml_string,
     * return a shared ptr the newly allocated one and note init
     * success through the success argument.
     */
    pugi::xml_document_ptr createDocument(const std::string& src, bool& success);

    /**
     * Save a XML file from a DOM tree.
     * @param node is a XML node
     * @param xml_file is the name of the xml_file
     * @param pretty control pretty (or not) serialization of XML
     * @return true if xml file was saved successfully.
     */
    bool save(pugi::xml_node node,
              const std::string& xml_file,
              bool pretty = false);

    /**
     * Save a XML file from a DOM tree.
     * @param node is a XML document
     * @param xml_file is the name of the xml_file
     * @param pretty control pretty (or not) serialization of XML
     * @return true if xml file was saved successfully.
     */
    bool saveDoc(const pugi::xml_document& doc,
                 const std::string& xml_file,
                 bool pretty = false);

    /**
     * DOM3 getText
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string does not contain the white spaces in element content.
     * @param node is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getText(pugi::xml_node node);
    std::string getText(pugi::xml_attribute attr);

    /**
     * DOM3 getText
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string does not contain the white spaces in element content.
     * @param xnode is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getText(pugi::xpath_node xnode);

    /**
     * getRawText
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param node is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getRawText(pugi::xml_node node);
    std::string getRawText(pugi::xml_attribute attr);

    /**
     * getRawText
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param xnode is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getRawText(pugi::xpath_node xnode);

    /**
     * DOM3 getTextFromNode
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string does not contain the white spaces in element content.
     * @param node is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getTextFromNode(pugi::xml_node node);


    /**
     * DOM3 getTextFromNode
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string does not contain the white spaces in element content.
     * @param attr is a reference to a attribute
     * @return attribute value
     */
    std::string getTextFromNode(pugi::xml_attribute attr);

    /**
     * DOM3 getTextFromNode for xpath result node
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string does not contain the white spaces in element content.
     * @param xnode is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getTextFromNode(pugi::xpath_node xnode);

    /**
     * getRawTextFromNode
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param node is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getRawTextFromNode(pugi::xml_node node);


    /**
     * getRawTextFromNode
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param attr is a reference to a attribute
     * @return attribute value
     */
    std::string getRawTextFromNode(pugi::xml_attribute attr);

    /**
     * getRawTextFromNode for xpath result node
     * On getting, no serialization is performed, the returned string does not contain any markup.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param xnode is a reference to a XML Node
     * @return all CDATA sections concatenated
     */
    std::string getRawTextFromNode(pugi::xpath_node xnode);

    /**
     * On setting, any possible children this node may have are removed and, if it the new
     * string is not empty or null, replaced by a single Text node containing the string this
     * attribute is set to.
     * No parsing is performed, the input string is taken as pure textual content.
     * @param node is a reference to a XML Node
     * @param text is a reference to the string text to build the new child node
     */
    void setText(pugi::xml_node node, const std::string& text);
    void setText(pugi::xml_node node, const char* text);

    /**
     * On setting, any possible children this node may have are removed and, if it the new
     * string is not empty or null, replaced by a single Text node containing the string this
     * attribute is set to.
     * No parsing is performed, the input string is taken as pure textual content.
     * @param node is a reference to a XML Node
     * @param text is a reference to the string text to build the new child node
     */
    void setText(pugi::xml_attribute attr, const std::string& text);
    void setText(pugi::xml_attribute attr, const char* text);

    /**
     * getInnerXML
     * Returns an XML fragment containing the text and markup of all children of node.
     * No whitespace normalization is performed and the returned string is not whitespace trimmed.
     * @param node is a reference to a XML Node
     * @return all children concatenated
     */
    std::string getInnerXML(pugi::xml_node node);

    /**
     * Serialize a XML Node to its XML representation.
     * @param doc is a reference to a document
     * @return the xml representation as stringT
     */
    std::string toXML(pugi::xml_document_ptr doc, bool pretty = false);
    /**
     * Serialize a XML Document to its XML representation.
     * @param doc is a reference to a document
     * @return the xml representation as stringT
     */
    std::string toXML(pugi::xml_document& doc, bool pretty = false);

    /**
     * Serialize a XML Node to its XML representation.
     * @param node is a reference to a XML Node
     * @return the xml representation as stringT
     */
    std::string toXML(pugi::xml_node node, bool pretty = false);

     /**
      * Serialize a XML Node to its XML representation for xpath result node.
      * @param xnode is a reference to a xpath_node
      * @return the xml representation as stringT
      */
    std::string toXML(pugi::xpath_node xnode, bool pretty = false);

    /**
     * Parses a xml text and returns the pretty printed document.
     */
    std::string xmlPrettyPrint(const std::string& xml_txt);

    /**
     * Count the number of child nodes.
     * Note: O(n) function
     * @param node is a reference to a XML Node
     * @return the number of child nodes.
     */
    std::size_t getChildCount(pugi::xml_node node);

    /**
     * Returns the first found child with the given tag name or an
     * empty node. Only checks for ELEMENT_NODES
     * @param node is the parent element
     * @param name is the name of the child node to return
     * @return the found node or an empty node.
     */
    pugi::xml_node getChildNodeByTagName(pugi::xml_node node, const std::string& name);

    /**
     * Returns the first found child element with the given tag name or an
     * empty node. Only checks for ELEMENT_NODES
     * @param node is the parent element
     * @param name is the name of the child element to return
     * @return the found node or an empty node.
     */
    pugi::xml_node getChildElementByTagName(pugi::xml_node node, const std::string& name);

    /**
     * Wrap
     */
    pugi::xpath_node selectSingleNode(pugi::xml_node node, const std::string& xpath_expr);
    pugi::xpath_node selectSingleNode(pugi::xpath_node node, const std::string& xpath_expr);
    pugi::xpath_node selectSingleNode(pugi::xml_document_ptr node, const std::string& xpath_expr);

    /**
     * Wrap
     */
    pugi::xpath_node_set selectNodes(pugi::xml_node node, const std::string& xpath_expr);
    pugi::xpath_node_set selectNodes(pugi::xml_document_ptr node, const std::string& xpath_expr);

    /**
     * Clone and append a document element node
     */
    pugi::xml_node appendNode(pugi::xml_document& dest_doc, pugi::xml_node src_node, bool deep);

    /**
     * Clone and append a element node
     */
    pugi::xml_node appendNode(pugi::xml_node dest_node, pugi::xml_node src_node, bool deep);

    /**
     * Clone and append all children from src_node to dest_node.
     */
    void appendAllChildren(pugi::xml_node dest_node, pugi::xml_node src_node, bool deep);

    /**
     * Remove all child nodes of the given paren node.
     */
    void removeAllChildren(pugi::xml_node parent);

    /**
     * Replace a child node with a copy of the given node.
     * @param parent is the parent of the to be replaced and replaced nodes.
     * @param new_node node that replaces the given old node
     * @param old_node
     * @return the inserted new node or 0 in case of an error
     */
    pugi::xml_node replaceCopy(pugi::xml_node parent_node, pugi::xml_node new_node, pugi::xml_node old_node);


    /**
     * @brief Create a new attribute and set its value
     * @pre The node does not contain an attribute name
     */
    void setNewAttribute(pugi::xml_node node, const std::string& name, const std::string& value);

    /**
     * @brief Create an xpath expression for the node (not its parents) with all attributes
     * <A><B x="y" /></A> for B --> B[@x='y']
     */
    std::string nodeAsXPath(pugi::xml_node node);

    /**
     * @brief Create an xpath expression for the node (including its parents) with all attributes
     * @note may not work correctly with multiple nodes with the same names and a non-identical set of attributes
     * <A><B x="y" /></A> for B --> A/B[@x='y']
     */
    std::string xpathToNode(pugi::xml_node node);

    /**
     * Exception thrown if a xml_node to xml_element conversion failed.
     */
    struct xml_element_exception : public std::exception
    {
    public:
        xml_element_exception(const std::string& msg) :
            std::exception()
        {
            ODK_UNUSED(msg);
        }

        virtual ~xml_element_exception() noexcept
        {}

        virtual const char* what() const noexcept
        {
            return m_message.c_str();
        }

        std::string m_message;
    };

    /**
     * Wrapper extending pugi::xml_node.
     */
    class xml_element : public pugi::xml_node
    {
    public:
        xml_element();
        xml_element(pugi::xml_node node);

        std::string getNodeName() const;

        // DW CS
        bool hasAttributes() const;

        bool hasAttribute(const std::string& attr_name) const;
        std::string getAttribute(const std::string& attr_name) const;
        void setAttribute(const std::string& attr_name, const std::string& attr_value);

        bool hasChildNodes() const;

        // pugi cs
        bool has_attributes() const;

        bool has_attribute(const std::string& attr_name) const;
        std::string get_attribute(const std::string& attr_name) const;
        void set_attribute(const std::string& attr_name, const std::string& attr_value);

        bool has_child_nodes() const;

        xml_element replace_copy(xml_element new_elem, xml_element old_elem);

        // Add child node with specified type. Returns added node, or empty node on errors.
        pugi::xml_node append_child(pugi::xml_node_type type = pugi::node_element);
        pugi::xml_node prepend_child(pugi::xml_node_type type = pugi::node_element);

        // Add child element with specified name. Returns added node, or empty node on errors.
        xml_element append_child(const std::string& name);
        xml_element prepend_child(const std::string& name);
    };


} // xpugi
