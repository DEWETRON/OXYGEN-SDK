.. _plugin_qml_intro:

QML for Oxygen Plugins
======================

This section explains how to work with QML items in Oxygen plugins, which
is also illustrated by the *Sync File Replay* and *WAV Export* examples. 


QML and QRC Files
-----------------

First create the QML files you want to use in Oxygen.
Check out the :ref:`UI extensions <ui_extensions>` for information on what
you can do with this feature.

These files and their dependencies should be listed in a Qt Resource Collection
(.qrc) file because we will use Qt Resource packages to upload everything to Oxygen.
See https://doc.qt.io/qt-5/resources.html for more details.

.. code-block:: xml

  <!DOCTYPE RCC><RCC version="1.0">
  <qresource>
    <file>./AddChannel.qml</file>
    ...
  </qresource>
  </RCC>


Build Integration (CMake)
-------------------------

The plugin build needs to perform two extra steps:

- Combine all required resources, as listed in the QRC file, into a single
  binary package.

  In your CMakeLists.txt this can be automated using the following commands:
  ``find_package(Qt5Core REQUIRED)``
  ``qt5_add_binary_resources(${LIBNAME}_qml input.qrc DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/output.rcc")``
- Ensure the plugin has access to the data output.rcc file during initialization.
  The easiest way is to use the provided CMake scripts to link the file into the
  plugin binary:

  ``set(RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)``
  ``include_directories(${RESOURCE_DIR})``
  ``AddResourceFile(${LIBNAME} "${CMAKE_CURRENT_BINARY_DIR}/output.rcc" ${RESOURCE_DIR})``
  
  This generates C++ files containg the file data and builds them within
  the project. The binary data can then be referenced using the variables
  declared by the header, in this case *output.rcc.h*.



Plugin Manifest
---------------

The plugin manifest must contain the element ``<UsesUIExtensions/>``
(within the element <OxygenPlugin>) to support any custom ui features.


Upload to Oxygen
----------------

The plugin class should implement the ``registerResources()`` method to
upload the resource pacakge by calling ``addQtResources()``.

If you followed the steps above and linked the resource file into the plugin,
you can upload using the following commands:

``#include "output.rcc.h"``
``addQtResources(plugin_resources::OUTPUT_RCC_data, plugin_resources::OUTPUT_RCC_size);``


QML Sandbox
===========

The QML Sandbox is an Oxygen feature to facilitate authoring of QML items,
by previewing in-development content and providing easy iteration of the design.

Sandbox loads its content directly from the filesystem and cannot work with
ressource files.

It can be found in the "Developer" section in the "System Settings" tab.

To use it
- select the display context of the item you are developing,
- select the folder where your QML ressources are stored,
- and enter the name of the item that should be displayed.

If the item needs :ref:`custom requests <custom_qml_request>`, your plugin
(which will respond to these requests) must be loaded and selected in the correspondig list.

Errors encountered while creating the item are displayed in the text field on the right.

If successfully loaded the item is displayed in the "Current Item" section.
The size of this area can be changed to test dynamic layouts.

After changing a file on disk, the "Reload files" button can be used to refresh
ressources an recreate the item using the new sources.

.. warning::
  The reload feature does not work properly in Oxygen 5.3.0, because the item
  remains cached and is not parsed again!



Available Items
===============

Oxygen plugins may only use QML features and items described in this documentation,
to ensure compatibility with future Oxygen versions.

If Oxygen provides items for a specific use case the have to be used for better
consistency and interoperability, but standard QML components that are provided,
and not deprecated, in Qt 5.12.6 may be used in other cases.

For example you should use the customized Oxygen version of standard controls,
but the usual QML Layouts to arrange them.


.. _custom_qml_request:


Custom Plugin Requests
======================

Custom requests provide a direct, but asynchronous, communication channel between
an UI item and its plugin. Each request function is identifed by a plugin-unique
uint16 number, which has to be managed by the plugin developer and used consistently
im QML and C++.

The following paragraphs and the *Sync File Replay* example illustrate the necessary steps to add custom
requests to a plugin.


Plugin Preparation
------------------

The plugin class needs a handler to process incoming custom requests.

Add a member

``std::shared_ptr<odk::framework::CustomRequestHandler> m_custom_requests``

and activate it by calling

``addMessageHandler(m_custom_requests);``

in the constructor.


Implementing Request Functions
------------------------------

Implement the required methods in your plugin.

For each method you can select one of the following signatures:

``std::uint64_t myCustomRequest(const odk::PropertyList& params, odk::PropertyList& returns)``
    This method uses :ref:`property lists <property_list_type>` as input and
    output and can therefore cover almost all use cases.

``std::uint64_t myXmlRequest(const odk::PropertyList& params, odk::IfXMLValue& return_xml)``
    This method received its parameters in a :ref:`property lists <property_list_type>`
    and responds with a single xml document.

``std::uint64_t myVerySpecialRequest(odk::IfHost* host, const odk::IfValue* param, const odk::IfValue** ret)``
    This low-level interface should not be used, because it provides no advantages for
    qml items but is more difficult to use correctly.

Each method has to be registered in the plugin constructor::

    std::uint16_t my_request1_id = 123;
    m_custom_requests->registerFunction(my_request1_id, "MyRequest1", std::bind(&MyPlugin::myRequest1, this, arg::_1, arg::_2));


Performing a Custom Request
---------------------------

The following QML item is used to perform custom requests from QML:

CustomPluginRequest (Oxygen.Tools 1.0)
    property int **messageId**
    
      The id specified when registering the custom request.

    readonly property bool **requestPending**: false

      Indicates whether an request is currently active and waiting for a response from the plugin.
      Only a single request can be active at the same time.

    function **request** (parameters)

      Start the request using the provided parameters (usually a :ref:`property list <property_list_type>`).

    signal **response** (bool success, int code, var value)

      Emitted as soon as the response arrives.
      If request was successful the parameters provide the return code as well as the return value.
      The value is usually a :ref:`property list <property_list_type>` or an string containing XML.

The following example provides easy access to a request using one input and one output parameter:

.. code-block:: qml

    import Oxygen.Tools 1.0

    ...

    CustomPluginRequest {
        id: myRequest1
        messageId: 123

        function startRequest(param)
        {
            var props = plugin.createPropertyList();
            props.setString("param", param);
            request(props);
        }

        onResponse:
        {
            var result = value.getString("result");
        }
    }


