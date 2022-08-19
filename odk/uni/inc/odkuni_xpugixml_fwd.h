// Copyright (c) DEWETRON GmbH 2015
#pragma once

#include <memory>

namespace pugi
{
    class xml_node;
    class xml_attribute;
    class xml_document;
    class xml_tree_walker;

    typedef std::shared_ptr<xml_document> xml_document_ptr;
    typedef std::weak_ptr<xml_document> xml_document_wptr;

} //pugi

namespace xpugi
{
    class xml_element;
    struct xml_element_exception;
    typedef std::shared_ptr<pugi::xml_document> xml_document_ptr;
    typedef std::weak_ptr<pugi::xml_document> xml_document_wptr;

} //xpugi
