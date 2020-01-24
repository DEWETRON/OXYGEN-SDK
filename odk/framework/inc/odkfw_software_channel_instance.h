// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkapi_data_set_descriptor.h"
#include "odkapi_timestamp_xml.h"
#include "odkapi_update_channels_xml.h"
#include "odkbase_basic_values.h"
#include "odkfw_input_channel.h"
#include "odkfw_interfaces.h"
#include "odkfw_stream_iterator.h"

#include <set>

namespace odk
{
namespace framework
{

    /**
     * Framework support class including several helpers
     * used to simplify read, process and/or write channel data from/to oxygen.
     */
    class SoftwareChannelInstance : public IfTaskWorker
    {
    public:

        /**
         * ProcessingContext used in process call
         * provides data needed for calculation since last call
         */
        struct ProcessingContext
        {
            void setStreamDescriptors(std::vector<odk::StreamDescriptor> stream_descriptors);

            bool setBlockList(const odk::IfDataBlockList* block_list);

            Timestamp m_master_timestamp;
            std::vector<StreamDescriptor> m_stream_descriptors;
            std::map<uint64_t, odk::framework::StreamIterator> m_channel_iterators;
            std::pair<double, double> m_window;
        };

    public:

        /**
         * This is invoked immediately after creation of an instance
         */
        virtual void create(odk::IfHost* host) {};

        /**
         * Called when channel ids of input channels change
         * config parameters are already updated by base class,
         * but all internal references of implementation have to be updated here if there are any
         * a mapping to an invalid channel id means the channel is gone
         *
         * @param  channel_mapping Mapping from old to new channel id
         */
        virtual void updateInputChannelIDs(
            const std::map<std::uint64_t, std::uint64_t>& channel_mapping) {};

        /**
         * Called after creation of a fresh instance for initialisation based on the user selection
         * input channels can either be accepted or ignored, depending on their characteristics
         *
         * @param  input_channel_data  Meta information about all input channels selected by user
         */
        virtual void init(
            const std::vector<InputChannel::InputChannelData>& input_channel_data) {};

        /**
         * Called after creation of a loaded instance for initialisation based on the saved setup
         * state of the instance has to be restored based on the request
         * this includes:
         *      - create all channels and return their new ids in channel_id_map
         *      - restore all config properties to the saved values
         *
         * their are some helper available to simplify this process:
         * @see configureFromTelegram, createMappingByKey, createChannelsFromTelegram, updatePropertiesfromTelegram
         *
         * @param  request  complete set of all output channels and their properties
         * @param  channel_id_map  mapping of channel ids in request to the newly created ids
         */
        virtual bool configure(const odk::UpdateChannelsTelegram& request,
            std::map<uint32_t, uint32_t>& channel_id_map)
            = 0;

        /**
         * Called on every config change of input and output channels of the instance
         * state of the instance has to be updated according to the new parameters
         *
         * @return  if instance is valid in current configuration
         */
        virtual bool update() = 0;

        /**
         * Called once before acquisition start when timebases of input channels are ready
         * timebases of all output channels of the instance have to be determined and set up in that call
         */
        virtual void initTimebases(odk::IfHost* host) {}

        /**
         * Called once before acquisition start
         * used to set up everything necessary for subsequent processing
         */
        virtual void prepareProcessing(odk::IfHost* host) {}

        /**
         * Called once after acquisition stop
         * used to clean up leftovers of processing
         */
        virtual void stopProcessing(odk::IfHost* host) {}

        /**
         * Called periodically as long as acquisition is running
         * processing and adding samples to output channels is done in here
         *
         * @param context  time information and sample data of input channels of processing interval
         */
        virtual void process(ProcessingContext& context, odk::IfHost* host) = 0;

    public:

        enum DataRequestType
        {
            NONE,       // no data requested
            STREAM      // continuous stream of all samples
        };

        virtual ~SoftwareChannelInstance();

        /**
         * Set up if and what kind of sample data of input channels should be requested and provided in process call via ProcessingContext
         *
         * @param type  type of data request
         */
        void setDataRequestType(DataRequestType type);

        /**
         * Get samples of all input channels at the given time
         *
         * @param time  requested time
         */
        std::map<uint64_t, odk::framework::StreamIterator> getSamplesAt(double time);

        bool containsChannel(std::uint32_t channel_id);

        /**
         * Get the root channel of this instance
         *
         * @return instance root channel
         */
        PluginChannelPtr getRootChannel();

        /**
         * Get all output channels of this instance
         *
         * @return instance output channels
         */
        std::vector<PluginChannelPtr> getOutputChannels();

    protected:

        /**
         * Checks value type and occurence of input channels
         *
         * @return true if valid
         */
        bool checkInputChannelFormats(const std::set<odk::ChannelDataformat::SampleValueType>& allowed_value_types,
                                      const std::set<odk::ChannelDataformat::SampleOccurrence>& allowed_occurence);

        void clearRequestedInputChannels();

        /**
         * Manually request an input channel
         *  input channel proxies and sample data is only provided for requested channels
         *
         *  all channels stored in a ChannelID or ChannelIDList config item are automatically requested
         *  no need to request them manually
         *
         * @param channel_id  channel to request
         */
        void requestInputChannel(std::uint64_t channel_id);

        /**
         * Adds an output channel with the given key
         * key is used to identify the channel in instance scope, used e.g. on config load
         *
         * @param key  instance unique key refering to the channel
         * @return created channel
         */
        PluginChannelPtr addOutputChannel(const std::string& key, PluginChannelPtr group_channel = {});

        PluginChannelPtr addGroupChannel(const std::string& key, PluginChannelPtr group_channel);

        /**
         * Removes the given output channel
         *
         * @param channel  channel to remove
         */
        void removeOutputChannel(PluginChannelPtr& channel);

        InputChannelPtr getInputChannelProxy(std::uint64_t channel_id);

        std::vector<InputChannelPtr> getInputChannelProxies();

        /**
         * Configure instance from telegram
         *   helper function which:
         *      - creates channels
         *      - applies properties
         *      - fills channel mapping
         *
         * @see configure()
         *
         * @param request  request telegram to use
         * @param channel_id_map  to fill mapping into
         */
        void configureFromTelegram(const odk::UpdateChannelsTelegram& request,
                                   std::map<uint32_t, uint32_t>& channel_id_map);

        /**
         * Creates channel id mapping based on channel key
         *  creates mapping between request telegram and current channel ids
         *
         * @see configure()
         *
         * @param request  request telegram to use
         * @param channel_id_map  to fill mapping into
         */
        void createMappingByKey(const odk::UpdateChannelsTelegram& request,
                                std::map<uint32_t, uint32_t>& channel_id_map);

        /**
         * Creates all channels contained in request
         *  already present channels are ignored
         *
         * @see configure()
         *
         * @param request  request telegram to use
         */
        void createChannelsFromTelegram(const odk::UpdateChannelsTelegram& request);

        /**
         * Updates channel properties based on request
         *  mapping specifies onto which current channel properties get applied
         *
         * @see configure()
         *
         * @param request  request telegram to use
         * @param channel_id_map  mapping to use
         */
        void updatePropertiesfromTelegram(
            const odk::UpdateChannelsTelegram& request,
            const std::map<uint32_t, uint32_t>& channel_id_map);

        /**
         * Get output channel by local id
         * @param local_id
         */
        PluginChannelPtr getOutputChannel(uint32_t local_id) const;

        /**
         * Get output channel by key specified on AddChannel
         * @param key
         */
        PluginChannelPtr getOutputChannelByKey(const std::string& key) const;

    private:

        template<class SoftwareChannelInstance>
        friend class SoftwareChannelPlugin;

        void shutDown();

        void initInstance(odk::IfHost* host);

        void setPluginChannels(PluginChannelsPtr plugin_channels);

        void fetchInputChannels();

        void setupDataRequest(odk::IfHost* host);

        void updateInternalInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& changed_ids);

        bool handleConfigChange();

        void onInitTimebases(odk::IfHost* host, std::uint64_t token) final;

        void onStartProcessing(odk::IfHost* host, std::uint64_t token) final;

        void onStopProcessing(odk::IfHost* host, std::uint64_t token) final;

        void onProcess(odk::IfHost* host, std::uint64_t token) final;

        void onChannelConfigChanged(odk::IfHost* host, std::uint64_t token) final;

    protected:
        std::vector<PluginChannelPtr> m_output_channels;

    private:
        PluginChannelsPtr m_plugin_channels;
        PluginTaskPtr m_task;
        DataRequestType m_data_request_type;
        std::vector<InputChannelPtr> m_input_channel_proxies;
        boost::optional<DataSetDescriptor> m_dataset_descriptor;
        std::vector<const odk::IfDataBlockList*> m_block_lists;
        odk::IfHost* m_host;
    };

}
}

