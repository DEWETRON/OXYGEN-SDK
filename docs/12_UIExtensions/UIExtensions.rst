.. _ui_extensions:

UI Extensions
=============

Oxygen provides hooks allowing plugins to provide a custom user interface for configuration pages.

Custom UI items are created in the Qt QML language (https://doc.qt.io/qt-5/qmlapplications.html) and uploaded to Oxygen by the plugin.
Check out :ref:`Plugin QML documentation <plugin_qml_intro>` for more information.

The instantiation context of the custom UI defines which global objects are provided and
which properties and behaviours are expected for proper integration with the host application.
The expected QML properties, functions and signals have to be added to the root item
and implemented according to this specification.

.. note::
  Properties tagged as ``readonly`` are only read by Oxygen.
  If you cannot use a binding you are free to make them mutable in your item
  and assign new values as necessary.

The following sections provide more details.



Common Properties
-----------------

The following layout properties should be used in all QML items for Oxygen
to communicate their size preferences.

.. note::
  Depending on the context and application settings these constraints may
  or may not be respected, therefore every effort should be made to present
  an attractive layout based on the actual width and height of the item.

.. code-block:: qml

  readonly property real preferredHeight
  readonly property real minimumHeight
  readonly property real maximumHeight

  readonly property real preferredWidth
  readonly property real minimumWidth
  readonly property real maximumWidth



Export settings
---------------

The *WAV Export* example demonstrates usage of custom *Export Settings*.

Export plugins can register two QML items by settings their names in the
``odk::RegisterExport`` telegram:

  - **m_ui_item_small** is displayed in the sidebar

  - **m_ui_item_full** is used when the export settings area is displayed full screen

These items will be shown in addition to the channel list and any
common export settings provided by oxygen.

Context Objects
---------------

None

Item interface
--------------

``property var customProperties``
    This property will be initalized to a :ref:`PropertyList <property_list_type>` instance by Oxygen
    and is used to synchronize settings between the full and small UI items.
    That means the structures have to be compatible.

``onCustomPropertiesChanged``
    This signal handler is strictly speaking not part of the interface,
    but should be implemented to update the content of the item based on
    the values stored in the customProperties list.

``function applyDefaults()``
    This function has to initialize the customProperties object to sensible
    default values. It is called when opening the export settings for the
    first time or after a configuration reset.

``readonly property bool settingsValid``
    This property should be *true* iff the current configuration could
    be used to start the export.



Add channel dialog
------------------

The *Sync File Replay* example demonstrates a custom *Add Channel* interface.

Software channel plugins can display a custom item in the *Add Channel* dialog.
Its name should be sent to Oxygen in **m_ui_item_add** member of the
``odk::RegisterSoftwareChannel`` telegram.


Context Objects
---------------

None

Item interface
--------------

``readonly property bool settingsValid``
  Should be true if the current settings are okay and  *Add* Button should
  be enabled.

  .. warning::
    This flag is ignored in Oxygen 5.3.0 but will be required in the next release.

``function queryProperties()``
  Starting from Oxygen 5.4.0 this function should return a
  :ref:`PropertyList <property_list_type>` containing the settings
  specified in the UI.
  This object is forwarded to the plugin after the user presses
  the *Add* button.

  .. note::
    For backwards compatibility with Oxygen 5.3, a JavaScript dictionary can
    be returned as well. The stored JavaScript values are converted to a
    property list using heuristics which makes type safe access from C++
    more difficult.
    Using PropertyList directly is therefore recommended if version 5.3
    does not have to be supported.



