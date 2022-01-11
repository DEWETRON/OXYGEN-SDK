// Copyright DEWETRON GmbH 2019

// This file is included by other headers

BEGIN_CATEGORIES()
    // plugin management stuff, init, deinit, ...
    BASE_ID(PLUGIN_SUPPORT,                0x0000000001000000);
    // oxygen support functionality, translation, conversion, paths, internal data?
    BASE_ID(SUPPORT_FUNCTIONS,             0x0000000002000000);
    // basic oxygen functionality
    BASE_ID(GENERAL_FUNCTIONS,             0x0000000003000000);
    // acquisition control functionality
    BASE_ID(ACQUISITION_CONTROL,           0x0000000004000000);
    BASE_ID(RECORDING_FUNCTIONS,           0x0000000004100000);
    // specified plugin stuff
    BASE_ID(REMOTE_CONTROL,                0x0000000005000000);
    BASE_ID(DATA_EXPORT,                   0x0000000006000000);
    // oxynet stuff
    BASE_ID(OXYNET_FUNCTIONS,              0x0000000007000000);
    BASE_ID(DATA_TRANSFER,                 0x0000000008000000);
    BASE_ID(SOFTWARE_CHANNEL,              0x0000000009000000);
    // data access
    BASE_ID(META_DATA_ACCESS,              0x0000000050000000);
    BASE_ID(CHANNEL_DATA_ACCESS,           0x0000000051000000);
END_CATEGORIES()

BEGIN_GROUP(plugin_msg)

    // PLUGIN SUPPORT
    MSG_ID(INIT,                       PLUGIN_SUPPORT, 0x000001, "not used (0)", "-", "optional; can provide an ErrorValue on problems",
            "OK (0), INTERNAL_ERROR, UNSUPPORTED_VERSION (plugin not compatible with this Oxygen version)");
    MSG_ID(DEINIT,                     PLUGIN_SUPPORT, 0x000002, "0", "-", "-",
            "OK (0), INTERNAL_ERROR");


    MSG_ID(ACQUISITION_TASK_PROCESS,           ACQUISITION_CONTROL, 0x00000105, "AcquisitionTaskID", " ", " ", "0 if okay");
    MSG_ID(ACQUISITION_TASK_INIT_TIMEBASES,    ACQUISITION_CONTROL, 0x00000106, "AcquisitionTaskID", " ", " ", "0 if okay");
    MSG_ID(ACQUISITION_TASK_START_PROCESSING,  ACQUISITION_CONTROL, 0x00000107, "AcquisitionTaskID", " ", " ", "0 if okay");
    MSG_ID(ACQUISITION_TASK_STOP_PROCESSING,   ACQUISITION_CONTROL, 0x00000108, "AcquisitionTaskID", " ", " ", "0 if okay");

    MSG_ID(PLUGIN_CONFIGURATION_CHANGE_REQUEST, GENERAL_FUNCTIONS, 0x000502, "always 0", "<UpdateConfig/> (UpdateConfigTelegram)", "?", "0: no error");
    MSG_ID(PLUGIN_LOAD_CONFIGURATION,           GENERAL_FUNCTIONS, 0x000503, "always 0", "<UpdateConfig/> (UpdateConfigTelegram)", "<ChannelIDMap/> (ChannelIDMapTelegram)", "0: no error");

    MSG_ID(PLUGIN_RESET,                        GENERAL_FUNCTIONS, 0x000504, "always 0", " ", " ", "0: no error");

    MSG_ID(PLUGIN_LOAD_INIT,                    GENERAL_FUNCTIONS, 0x000505, "always 0", " ", " ", "0: no error");
    MSG_ID(PLUGIN_LOAD_FINISH,                  GENERAL_FUNCTIONS, 0x000506, "always 0", " ", " ", "0: no error");

    MSG_ID(PLUGIN_CHANNEL_IDS_CHANGED,          GENERAL_FUNCTIONS, 0x000507, "always 0", "<ChannelIDMap/> (ChannelIDMapTelegram)", " ", "0: no error");
    MSG_ID(PLUGIN_CHANNEL_DATAFORMAT_CHANGED,   GENERAL_FUNCTIONS, 0x000508, "always 0", "<ChannelID/> (DataFormatTelegram?)", " ", "0: no error");

    MSG_ID(NOTIFY_CHANNEL_CONFIG_CHANGED,       GENERAL_FUNCTIONS, 0x000509, "always 0", "<ChannelConfigChanged/> (oxy_plugin_common::ChannelConfigChangedTelegram) containing the changed properties", " ", "0: no error");

    MSG_ID(PLUGIN_RESERVE_CHANNEL_IDS,          GENERAL_FUNCTIONS, 0x00050a, "always 0", "<ChannelID/> (List of channel IDs)", " ", "0: no error");

    // > Plugin
    // PLUGIN SUPPORT
    MSG_ID(NOTIFY_EVENT,               SUPPORT_FUNCTIONS, 0x000001, "Event ID", "event specific", "-",
           "0 if okay");

    MSG_ID(CUSTOM_QML_REQUEST,         SUPPORT_FUNCTIONS, 0x001000, "request id (uint range due to javascript limitations)", "request specific", "request specifc", "-1 if not implemented; request specific otherwise");

    MSG_ID(SOFTWARE_CHANNEL_CREATE,       SOFTWARE_CHANNEL, 0x000003, "SoftwareChannelId", "-", "-", " ");

    MSG_ID(SOFTWARE_CHANNEL_QUERY_ACTION, SOFTWARE_CHANNEL, 0x000004, "SoftwareChannelId", "-", "-", " ");

    MSG_ID(DELETE_PLUGIN_CHANNELS,        GENERAL_FUNCTIONS, 0x000007, "-", "<Channels/> (odk::ChannelList) containing the local ids of channels to remove", "-", " ");

    MSG_ID(EXPORT_VALIDATE_SETTINGS,      DATA_EXPORT, 0x000201, "Plugin UNIQUE",
            "<settings as EXPORT_START>", "<reason OK/NOK>",
            "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_GENERATE_NAME,          DATA_EXPORT, 0x000202, "Plugin UNIQUE", "<prefix>",
            "<generated filename>", "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_START,                  DATA_EXPORT, 0x000203, "Plugin UNIQUE",
            "<start time, channels, range, filename>", "<reason OK/NOK>",
            "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_CANCEL,                 DATA_EXPORT, 0x000204, "Plugin UNIQUE", "<reason?>", "-",
            "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_FINALIZE,               DATA_EXPORT, 0x000205, "Plugin UNIQUE", "<reason?>", "-",
            "0 if okay; 1 for internal error; 2 for compatibility error");


END_GROUP()

BEGIN_GROUP(host_msg)
    // To Host
    // > Host

    MSG_ID(LOG_MESSAGE,                 PLUGIN_SUPPORT, 0x000003,
        "loglevel (1 fatal, 2 error), if used with string value (see odkapi_logging.h); 0 reserved for use with xml value",
        "<XML> or string", "-",
        "standard error codes");

    MSG_ID(ADD_RESOURCES,               SUPPORT_FUNCTIONS, 0x000500,
        "0: rcc (Qt Resource File); other values reserved",
        "ArbitraryBinaryValue", "optional IfErrorValue in case of problems",
        "standard error codes");

    MSG_ID(ADD_TRANSLATION,             SUPPORT_FUNCTIONS, 0x000501,
        "0: ts-xml-file; other values reserved",
        "<TS>", "optional IfErrorValue in case of problems",
        "standard error codes");

    MSG_ID(SOFTWARE_CHANNEL_REGISTER,         SOFTWARE_CHANNEL, 0x000001, "0", "<XML>",
           "<xml special settings?>",
           "0 if okay; 1 for internal error; 2 for compatibility error; 3 already registered ...");
    MSG_ID(SOFTWARE_CHANNEL_UNREGISTER,       SOFTWARE_CHANNEL, 0x000002, "0", "<REASON>", "-",
           "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(SOFTWARE_CHANNEL_UPDATE,           SOFTWARE_CHANNEL, 0x000005, "0", "<XML>",
           "<xml special settings?>",
           "0 if okay; 1 for internal error; 2 for compatibility error");

    //     EXPORT_REGISTER name, format+version, file extensions,...
    //     EXPORT_UNREGISTER
    //     EXPORT_FINISHED id
    //     EXPORT_CANCELED id, status (file deleted/invalid/okay),...
    //     EXPORT_FAILED id, reason (disk full,...)
    //     EXPORT_PROGRESS id, %, estimate?
    MSG_ID(EXPORT_REGISTER,            DATA_EXPORT, 0x000001, "0", "<XML>",
            "<xml special settings?>",
            "0 if okay; 1 for internal error; 2 for compatibility error; 3 already registered ...");
    MSG_ID(EXPORT_UNREGISTER,          DATA_EXPORT, 0x000002, "0", "<REASON>", "-",
            "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_FINISHED,            DATA_EXPORT, 0x000101, "TransactionKey",
            "-", "-", "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_CANCELED,            DATA_EXPORT, 0x000102, "TransactionKey", "<filename>", "-",
            "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_FAILED,              DATA_EXPORT, 0x000103, "TransactionKey", "<filename, reason>",
            "-", "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(EXPORT_PROGRESS,            DATA_EXPORT, 0x000104, "TransactionKey", "<filename, progress>",
            "-", "0 if okay; 1 for internal error; 2 for compatibility error");

    MSG_ID(SET_AUTO_EXPORT,            DATA_EXPORT, 0x000105, "0", "<reason>",
            "-", "0 if okay; 1 for internal error; 2 for compatibility error");
    MSG_ID(AUTO_EXPORT_FOLDER,         DATA_EXPORT, 0x000106, "0", "<reason?>",
            "-", "0 if okay; 1 for internal error; 2 for compatibility error");


    // DATA_REQUEST key, settings - blocking
    // DATA_STOP_REQUEST key, settings (now; @TS) - blocking
    MSG_ID(DATA_START_REQUEST,          CHANNEL_DATA_ACCESS, 0x000010, " ", " ", " ", " ");
    MSG_ID(DATA_STOP_REQUEST,           CHANNEL_DATA_ACCESS, 0x000011, " ", " ", " ", " ");

    //DATA EXPORT
    // DATA_GROUP_ADD xml (channel, format, interval) , key  ---> define SD
    // DATA_GROUP_REMOVE key
    // DATA_READ key, settings - blocking
    // DATA_GROUP_GET_DESCRIPTOR key

    MSG_ID(DATA_GROUP_ADD,              CHANNEL_DATA_ACCESS, 0x000001, " ", " ", " ", " ");
    MSG_ID(DATA_GROUP_REMOVE,           CHANNEL_DATA_ACCESS, 0x000002, "DataSetId", "DataSetId (optional, instead of key)", " ", " ");
    MSG_ID(DATA_READ,                   CHANNEL_DATA_ACCESS, 0x000003, " ", " ", " ", " ");
    MSG_ID(DATA_REGIONS_READ,           CHANNEL_DATA_ACCESS, 0x000005, " ", " ", " ", " ");

    MSG_ID(ACQUISITION_TASK_ADD,         ACQUISITION_CONTROL, 0x000101, "always 0", "AddAcquisitionTaskTelegram", "-", "0: no error");
    MSG_ID(ACQUISITION_TASK_REMOVE ,     ACQUISITION_CONTROL, 0x000102, "AcquisitionTaskID", "-", "-", "0: no error");

    /// Specify the output channels provided by the plugin.
    /// Every call has to provide the full list of channels that are provided by the the plugin at this point.
    /// Oxygen adds/removes/updates the channels accordingly to realize the requested state.
    /// Existing channels will not be changed if any error other than INTERNAL_ERROR occurs.
    MSG_ID(SET_PLUGIN_OUTPUT_CHANNELS, GENERAL_FUNCTIONS, 0x000500, "always 0", "<UpdatePluginChannels/> (UpdateChannelsTelegram)", "?", "0: no error");

    /// Update config-items (value and constraints) provided by the plugin
    /// Every call has to provide full set of config-items that are provided by the the plugin at this point.
    /// Oxygen adds/removes/updates the config-items accordingly to realize the requested state.
    MSG_ID(SET_PLUGIN_CONFIGURATION, GENERAL_FUNCTIONS, 0x000501, "always 0", "<UpdateConfig/> (UpdateConfigTelegram)", "?", "0: no error");


    MSG_ID(ADD_SAMPLE, GENERAL_FUNCTIONS, 0x000601, "LocalChannelID", "ArbitraryBinaryValue", "?", "0: no error");
    MSG_ID(ADD_CONTIGUOUS_SAMPLES, GENERAL_FUNCTIONS, 0x000602, "LocalChannelID", "ArbitraryBinaryValue", "?", "0: no error");
    MSG_ID(UPDATE_CHANNEL_STATE, GENERAL_FUNCTIONS, 0x000603, "LocalChannelID", "IfUIntValue", "?", "0: no error");

    MSG_ID(WRITE_TO_CHANNEL, GENERAL_FUNCTIONS, 0x000701, "ChannelID", "ArbitraryBinaryValue", "?", "0: no error");
END_GROUP()

BEGIN_GROUP(host_msg_async)
    // To Host
    // > Host

    MSG_ID(LOG_MESSAGE,                 PLUGIN_SUPPORT, 0x000003,
        "loglevel, if used with string value (see odkapi_logging.h); 0 reserved for use with xml value",
        "<XML> or string", "-",
        "standard error codes");

END_GROUP()

