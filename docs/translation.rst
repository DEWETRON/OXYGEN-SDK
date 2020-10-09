
===========
Translation
===========

Normal texts that are sent to Oxygen for display should be written in English.
In some cases (e.g. :ref:`config item keys <config_item_key>`) a specified,
more technical format, has to be used.

Usually, all of those are processed by our translation framework before they
shown to the user. Therefore it is recommended to upload plugin-specifc
translation files to Oxygen during plugin initialization.

---------------------------
Format of Translation Files
---------------------------

Oxygen currently accepts translations as XML files in the Qt Linguist format:
https://doc.qt.io/qt-5/linguist-ts-file-format.html

.. code-block:: xml

    <?xml version="1.0"?>
    <TS version="2.1" language="en" sourcelanguage="en">
        <context><name>ConfigKeys</name>
            <message>
                <source>PLUGIN_ID/MyConfigItem</source>
                <translation>My Configuration Item</translation>
            </message>
        </context>
    </TS>

All translated strings are located in a context that is specified in this
section. This is necessary to disambiguate between different modules in
the application.

------------------------
Registering Translations
------------------------

During initialization the plugin should upload all available translation
files to Oxygen, which in turn will select the most appropriate language
based on application settings.

The file that is uploaded first will be used if no better variant is
available.

Upload is performed using the following message:

``messageSync(ADD_TRANSLATION, 0, XMLValue, &ErrorValue);``

If using framework this is wrapped in the simple function

``addTranslation("<ts-xml>")``

Call this in your override of the ``registerResources()`` method in the
plugin class.


------------
Config Items
------------

:ref:`Config Item keys <config_item_key>` are shown by default as column
labels in the channel list and in the detail setup page of a channel.

At least an English translation file should be provided to make the
internal key strings display nicely.

Use context *ConfigKeys* and the key as source string to provide translations.


---------
QML Items
---------

All displayed texts in QML items should be explicitly marked for translation
using the qsTranslate method (https://doc.qt.io/qt-5/qml-qtqml-qt.html#qsTranslate-method):

``qsTranslate("MY_PLUGIN/AddChannel", "Settings for the new channel")``

Qt will automatically use a matching translated text if an entry is found in the
translation file, or use the provided text as the fallback.

``qsTranslate(context, sourceText)``
------------------------------------

  The context has to be in the format "<PLUGIN_ID>/<QmlItemName>" to prevent conflict
  between different plugins.

  sourceText should be the english version of the text. You can use placeholders
  if necessary to include dynamic parameters in the text as described in
  https://doc.qt.io/qt-5/qml-qtqml-string.html.

For the example above, a german translation could be provided by adding the following
code to your translation file:

.. code-block:: xml

    <context><name>MY_PLUGIN/AddChannel</name>
        <message>
            <source>Settings for the new channel</source>
            <translation>Einstellungen des neuen Kanals</translation>
        </message>
    </context>


