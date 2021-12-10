// Copyright DEWETRON GmbH 2019

#ifndef __ODKAPI_EVENT_IDS__
#define __ODKAPI_EVENT_IDS__

namespace odk
{
    enum EventIds
    {
        EVENT_ID_ACQUISITION_STARTED,        //< sent when the acquisition was successfully started
        EVENT_ID_ACQUISITION_STOPPED,        //< sent when acquisition is no longer running
        EVENT_ID_ACQUISITION_CONFIGURE_DONE,
        EVENT_ID_TOPOLOGY_CHANGED,           //< sent after the system changed (new hardware found, channels added or removed)
        EVENT_ID_STARTUP_COMPLETED,          //< sent after the application is fully initialized
        EVENT_ID_SYNC_EVENT,                 //< sent when a sync event is emitted on the node
        EVENT_ID_SYNC_STATUS,                //< sent when the local sync status of a node has changed
        EVENT_ID_RECORDING_INTERRUPTED,      //< sent after recording has been stopped
        EVENT_ID_MEASUREMENT_ACTIVE,         //< sent after measurement has been started
        EVENT_ID_MEASUREMENT_STOPPED,        //< sent after measurement has been stopped
        EVENT_ID_RECORDING_ACTIVE,           //< sent after recording has been started
        EVENT_ID_RECORDING_INACTIVE,         //< sent after recording has been stopped or paused
        EVENT_ID_OUT_OF_DISK_SPACE_ACTIVE,   //< sent after a low disk space exception
        EVENT_ID_OUT_OF_DISK_SPACE_INACTIVE, //< sent after a low disk space exception
        EVENT_ID_LICENSE_CHANGED,            //< sent after a the license was changed
    };
}

#endif
