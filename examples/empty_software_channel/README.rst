=======================================
Template: Empty Software Channel Plugin
=======================================

This is an empty plugin-project to be used as a template for
writing your own plugins.

This project does not compile, nor does it provide any funtionality.

Please copy the whole folder, rename as appropriate, and fill
in all elements, that are marked with $ODK_REPLACE_ME$.

Those marked elements are also marked with an identifier
($[IDENTIFIER]$, eg $GUID$) as an easy reference in the descriptive legend.

This replacement is necessary in both - the cmake file, and in the \*.cpp file.

-----------------------------------------
Descriptive Legend of Elements to fill in
-----------------------------------------

---------------------------
Elements in the \*.cpp file
---------------------------

Plugin_Manifest
---------------

**$UNIQUE_PLUGIN_NAME$**: 
  a (short)name of the plugin. This is only used internally,
  and will not be dipslayed to the user.
  
  It *shall* be used to prefix custom configuration-keys
  (that is for configuration items declared and managed by the plugin)
  
  It *shall* follow this pattern, to ensure sufficient
  uniqueness: *COMPANYID_PLUGIN_NAME*
  
  eg: "MYCOMPANY_AN_EXAMPLE_PLUGIN"

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
  eg: "5D75B044-B52E-4721-B594-666E3B413FE3"
**$PLUGIN_NAME$**:
  The human readable name of the plugin. This name will be shown in the
  plugin-overview ("System Settings"/"Extensions and Plugins"/"Overview")
  eg: "Example Plugin"
**$VENDOR_NAME$**:
  This information will be shown in the plugin-overview as "Vendor".
  eg: "My Company"
**$PLUGIN_DESCRIPTION$**:
  This information will be shown in the plugin-overview. as a more
  elaborate description, than the name.
  eg: "This plugin is an empty example, and does nothing at all."
**$MIN_HOST_VERSION$**:
  The minimum Oxygen version required to run this plugin
  eg: "5.0.1"

getSoftwareChannelInfo()
------------------------
**$ADD_INSTANCE_DISPLAY_NAME$**:
  This name will be displayed in the add-channel-dialog
  (The '+' sign, in the channel-setup screen)
**$ADD_INSTANCE_DISPLAY_GROUP$**:
  This determines, in which group the plugin-instance will be listed
  within the add-channel-dialog.
  It can either be an existing group (eg. "Data Sources", "Advanced Math")
  or a new, custom-declared group.
  Please keep in mind, that too many custom-declared groups might clutter
  the interface and give a bad user experience.
**$ADD_INSTANCE_DISPLAY_DESCRIPTION$**:
  This description will be shown on the righthand-side of the add-channel-dialog.
  The string does not support control-characters for carriage-return/line-feed.

-----------------------------------
Elements in the CmakeLists.txt file
-----------------------------------

*$LIBNAME$*

*$VS_PROJECT_GUID$*:
  a GUID, necessary for visual-studio to keep track of files in
  multi-project-worksapces.
  eg: "5D75B044-B52E-4721-B594-666E3B413FE3"

*$PLUGIN_SOURCE_FILES$*:
  Add here all necessary source-files of the plugin.

