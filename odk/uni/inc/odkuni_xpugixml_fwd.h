// Copyright (c) DEWETRON GmbH 2015
#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace pugi
{
    class xml_node;
    class xml_attribute;
    class xml_document;
    class xml_tree_walker;

    typedef boost::shared_ptr<xml_document> xml_document_ptr;
    typedef boost::weak_ptr<xml_document> xml_document_wptr;

} //pugi

namespace xpugi
{
    class xml_element;
    struct xml_element_exception;
    typedef boost::shared_ptr<pugi::xml_document> xml_document_ptr;
    typedef boost::weak_ptr<pugi::xml_document> xml_document_wptr;

} //xpugi
