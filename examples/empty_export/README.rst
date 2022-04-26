
Template: Empty Export Plugin
-----------------------------

This is an empty plugin-project to be used as a template for
writing your own plugins.

This project does not compile, nor does it provide any funtionality.

Please copy the whole folder, rename as appropriate, and fill
in all elements, that are marked with $ODK_REPLACE_ME$.

Those marked elements are also marked with an identifier
($[IDENTIFIER]$, eg $GUID$) as an easy reference in the descriptive legend.

This replacement is necessary in both - the cmake file, and in the \*.cpp file.



Elements in the \*.cpp File
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Plugin_Manifest
~~~~~~~~~~~~~~~

**$UNIQUE_PLUGIN_NAME$**: 
  a (short)name of the plugin. This is only used internally,
  and will not be dipslayed to the user.
  
  It *shall* be used to prefix custom configuration-keys
  (that is for configuration items declared and managed by the plugin)
  
  It *shall* follow this pattern, to ensure sufficient
  uniqueness: *COMPANYID_PLUGIN_NAME*
  
  e.g.: "MYCOMPANY_AN_EXAMPLE_PLUGIN"

  *Allowed charachters*:
    * Upper case alphabetic charachters
    * underscores

  To avoid conflicts with Oxygen-internal data or Dewetron-supplied plugins following COMPANYID prefixes are reserved:
    * OXY
    * OXYGEN
    * NEON
    * DEWETRON
    * DEWE
    * DW

**$PLUGIN_GUID$**:
  a GUID, used to identify the plugin (eg. in stored setup-files, etc)

  e.g.: "5D75B044-B52E-4721-B594-666E3B413FE3"

  The GUID generator in Visual Studio (Tools -> Create GUID) can be used to generate this value.
  Use the 'Registry Format' and replace the curly braces with double quotes.

**$PLUGIN_NAME$**:
  The human readable name of the plugin. This name will be shown in the
  plugin-overview ("System Settings"/"Extensions and Plugins"/"Overview")

  e.g.: "Example Plugin"

**$VENDOR_NAME$**:
  This information will be shown in the plugin-overview as "Vendor".

  e.g.: "My Company"

**$PLUGIN_DESCRIPTION$**:
  This information will be shown in the plugin-overview. as a more
  elaborate description, than the name.

  e.g.: "This plugin is an empty example, and does nothing at all."

**$MIN_HOST_VERSION$**:
  The minimum Oxygen version required to run this plugin

  e.g.: "5.0.2"


getSoftwareChannelInfo()
~~~~~~~~~~~~~~~~~~~~~~~~

**$REGISTER_EXPORT_FORMAT_NAME$**:
  This name will be displayed in the export format list

**$REGISTER_EXPORT_FORMAT_ID$**:
  Plugin unique id of the format

**$REGISTER_EXPORT_FILE_EXTENSION$**:
  Extension of the resulting exported file

**$REGISTER_EXPORT_UI_ITEM_SMALL$**:
  UI item shown in the export sidebar tab (used for additional configuration)

**$REGISTER_EXPORT_UI_ITEM_LARGE$**:
  UI item shown in the fullscreen export tab (used for additional configuration)



Elements in the CMakeLists.txt File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*$LIBNAME$*
  Name of the project and the resulting .plugin

*$VS_PROJECT_GUID$*:
  A GUID, necessary for Visual Studio to keep track of files in
  multi-project-workspaces.

  e.g.: "5D75B044-B52E-4721-B594-666E3B413FE3"

*$PLUGIN_SOURCE_FILES$*:
  Add here all necessary source-files of the plugin.

