
------------------------
Export Plugins
------------------------

ExportPlugin is a framework support class including several helpers used to simplify export of channel data from Oxygen to custom data formats.

```````````````
Export transaction
```````````````

An export transaction consists of 3 calls:

validate
    Exporter has to validate itself with the given context and return the result and possible warning/error messages to further describe the result.
    Checks of support for the given channels, formats, custom settings etc. provided by the context object should be made here.
    
    This could be called multiple times without an actual export happening to inform the user about the state.

exportData
    One call for the actual export of data. All needed information about metadata, channels, settings etc. is provided via the context object. 
    Channel data gets automatically fetched from oxygen and is accessible via iterators.
    
cancel
    Called if export transaction gets cancelled by the user. All necessary cleanup tasks should be handled here (file deletion, etc.)


```````````````
Custom Settings GUI
```````````````

If an export plugin needs some custom configuration from the user, a custom QML GUI element can be provided via export registration telegram.
Separate elements can be provided for the small and fullscreen export tab in oxygen if needed.

For creation and registration of custom UI elements see QML UI elements.

Custom configuration properties can be specified in that element and are automatically provided to the plugin in the context object of the validate and exportData calls.
The enable that, the following QML properties, functions and signal handlers need to be implemented by the root QML element:

property var customProperties
    Object which holds the custom properties, see QPropertyList
    
readonly property bool settingsValid
    Indicator if the currently selected GUI settings are valid
    
function applyDefaults()
    All custom properties with their default values have to be set in here

onCustomPropertiesChanged
    Called if the custom properties changed and used to update the custom GUI accordingly


