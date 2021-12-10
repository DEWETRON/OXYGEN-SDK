// Copyright DEWETRON GmbH 2019

#ifndef __ODKAPI_OXYGEN_QUERIES__
#define __ODKAPI_OXYGEN_QUERIES__

#define STATIC_CONTEXT(name, context_value, doc) const static char* name = context_value
#define READ_ONLY_PROPERTY(context_name, name, type, doc) const static char* context_name##_##name = #name
#define WRITE_ONLY_PROPERTY(context_name, name, type, doc) const static char* context_name##_##name = #name
#define READ_WRITE_PROPERTY(context_name, name, type, doc) const static char* context_name##_##name = #name

namespace odk
{
namespace queries
{
    //Current context strings use # as a separator in order to prevent conflict with config key names (ID:TRION/BoardId)
    //  and potential usage of xpath in the future

    STATIC_CONTEXT( PluginHost, "#PluginHost", "Properties and functionality of the plugin host application (not necessarily oxygen)");

    READ_ONLY_PROPERTY( PluginHost,     VendorName,         IfStringValue,      "Vendor of the host application");
    READ_ONLY_PROPERTY( PluginHost,     Name,               IfStringValue,      "Name of the host application (without version)");
    READ_ONLY_PROPERTY( PluginHost,     VersionString,      IfStringValue,      "Version of the host application as a displayable string");
    READ_ONLY_PROPERTY( PluginHost,     LogPath,            IfStringValue,      "Absolute path to the directory where log files should be stored");

    STATIC_CONTEXT( Oxygen,                 "#Oxygen",                      "References global oxygen properties");
    STATIC_CONTEXT( OxygenAcqStartTime,     "#Oxygen#AcquisitionStartTime", "References (absolute) acquisition start time information");
    STATIC_CONTEXT( OxygenMeasStartTime,    "#Oxygen#MeasurementStartTime", "References measurment start time information (relative to acquisition start)");
    STATIC_CONTEXT( OxygenMeasEndTime,      "#Oxygen#MeasurementEndTime",   "References measurment end time (current time or end of file) information (relative to acquisition start)");
    STATIC_CONTEXT( OxygenRecording,        "#Oxygen#Recording",            "References global oxygen recording properties");


    READ_ONLY_PROPERTY( Oxygen,                     AcquisitionState,      IfEnumValue,          "Current acquisition state");
    READ_ONLY_PROPERTY( Oxygen,                     MasterTimebaseValue,   IfXMLValue,           "Current master time base value");
    READ_ONLY_PROPERTY( Oxygen,                     AnalysisModeActive,    IfBooleanValue,       "Is currently in Analysis Mode currently");

    READ_ONLY_PROPERTY( OxygenAcqStartTime,         AbsISO,         IfStringValue,          "Returns the absolute time of acquisition start");
    READ_ONLY_PROPERTY( OxygenAcqStartTime,         AbsXML,         IfXMLValue,             "Returns the absolute time of acquisition start and timezone information in a verbose format");

    READ_ONLY_PROPERTY( OxygenMeasStartTime,        AbsISO,         IfStringValue,          "Returns the absolute time of measurement start (Armed, start recording)");
    READ_ONLY_PROPERTY( OxygenMeasStartTime,        AbsXML,         IfXMLValue,             "Returns the absolute time of measurement start and timezone information in a verbose format");
    READ_ONLY_PROPERTY( OxygenMeasStartTime,        RelSec,         IfFloatingPointValue,   "Returns the measurement start time in seconds relative to the acqusition start");
    READ_ONLY_PROPERTY( OxygenMeasStartTime,        RelXML,         IfXMLValue,             "Returns the measurement start time using a sample-exact description");

    READ_ONLY_PROPERTY( OxygenMeasEndTime,          AbsISO,         IfStringValue,          "Returns the absolute time of acquisition end (steadily increasing while measurement is running)");
    READ_ONLY_PROPERTY( OxygenMeasEndTime,          AbsXML,         IfXMLValue,             "Returns the absolute time of acquisition end and timezone information in a verbose format");
    READ_ONLY_PROPERTY( OxygenMeasEndTime,          RelSec,         IfFloatingPointValue,   "Returns the acquisition end time in seconds relative to the acquisition start");
    READ_ONLY_PROPERTY( OxygenMeasEndTime,          RelXML,         IfXMLValue,             "Returns the acquisition end time using a sample-exact description");

    STATIC_CONTEXT( OxygenHeaders, "#Oxygen#Headers", "References global measurement headers");
    READ_ONLY_PROPERTY( OxygenHeaders,  List,                       IfXMLValue,            "Returns a list of headers and their display value");
    // #Oxygen#Headers#<HeaderName>|DisplayValue                    get a displayable string describing the value (IfStringValue)

    STATIC_CONTEXT( OxygenChannels, "#Oxygen#Channels", "References the channel list");
    READ_ONLY_PROPERTY( OxygenChannels,  AllIds,                    IfXMLValue,            "Returns a list of all channel ids");
    READ_ONLY_PROPERTY( OxygenChannels,  AllItemsEx,                IfXMLValue,            "Read the current value of all items as an xml document for each channel");
    READ_ONLY_PROPERTY( OxygenChannels,  AllConstraintsEx,          IfXMLValue,            "Read the constraints of all items as an xml document for each channel");
    READ_ONLY_PROPERTY( OxygenChannels,  AllTimebasesEx,            IfXMLValue,            "Read the timebase information (frequency, parents) required to convert channel ticks into seconds relative to acquisition start");
    // #Oxygen#Channels#<ChId>#Config#<ConfigItemKey>|Value         read/write the current value of this config item (type depends on config item type)
    // #Oxygen#Channels#<ChId>#Config#<ConfigItemKey>|ValueXML      read/write the current value of this config item in xml format
    // #Oxygen#Channels#<ChId>#Config#<ConfigItemKey>|ConstraintsXML read/write the current constraints of this config item in xml format
    // #Oxygen#Channels#<ChId>#Config|AllValues                     read the current value of all config items as an xml document
    // #Oxygen#Channels#<ChId>#Config|AllIdValues                   read the current value of all ID items as an xml document
    // #Oxygen#Channels#<ChId>#Config|AllItemsEx                    read the current value of all items as an xml document (unfiltered, with chain structure)
    // #Oxygen#Channels#<ChId>#Config|AllConstraintsEx              read the constraints of all items as an xml document (unfiltered, with chain structure)
    // #Oxygen#Channels#<ChId>|Name                                 get the name of the channel (IfStringValue; alias for corresponding config item)
    // #Oxygen#Channels#<ChId>|LongName                             get the full name (including domain) of the channel (IfStringValue; alias for corresponding config item)
    // #Oxygen#Channels#<ChId>|Timebase                             read the timebase information (frequency, parents) required to convert channel ticks into seconds relative to acquisition start
    // #Oxygen#Channels#<ChId>|DataFormat                           Returns the data format-information for the given channel");

    STATIC_CONTEXT(OxygenTopology, "#Oxygen#Topology", "References the system topology");
    READ_ONLY_PROPERTY(OxygenTopology, List,                       IfXMLValue,            "Returns the system topology as xml");

    STATIC_CONTEXT(OxygenPaths, "#Oxygen#Paths", "Standard paths for Oxygen files");
    READ_ONLY_PROPERTY(OxygenPaths, CommonAppDataDir,   IfStringValue, "Absolute path of the systems wide writeable application data folder");
    READ_ONLY_PROPERTY(OxygenPaths, AppDataDir,         IfStringValue, "Absolute path of the installed application data folder");
    READ_ONLY_PROPERTY(OxygenPaths, LogDir,             IfStringValue, "Absolute path of the oxygen logging folder ");
    READ_ONLY_PROPERTY(OxygenPaths, RecordingDir,       IfStringValue, "Absolute path of the configured oxygen recording folder ");
    READ_ONLY_PROPERTY(OxygenPaths, SetupDir,           IfStringValue, "Absolute path of the oxygen setup folder ");

    STATIC_CONTEXT(OxygenExport, "#Oxygen#Export", "Standard Export for Oxygen");
    READ_WRITE_PROPERTY(OxygenExport, AutoExportDir,          IfStringValue, "Absolute path of the auto export data folder");
    READ_WRITE_PROPERTY(OxygenExport, AutoExport,             IfBooleanValue, "Enable/disable auto export");
}
}

#endif
