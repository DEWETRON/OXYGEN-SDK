// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkfw_interfaces.h"
#include "odkfw_if_message_handler.h"
#include "odkfw_properties.h"

#include "odkbase_if_host.h"

#include "odkapi_acquisition_task_xml.h"
#include "odkapi_channel_config_changed_xml.h"
#include "odkapi_channel_list_xml.h"
#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_channel_mapping_xml.h"
#include "odkapi_oxygen_queries.h"
#include "odkapi_update_channels_xml.h"

#include <map>
#include <set>



namespace odk
{
namespace framework
{



    class PluginChannel : public IfChannelPropertyChangeListener
    {
        friend class PluginChannels;
    public:

        PluginChannel(std::uint32_t local_id, IfPluginChannelChangeListener* change_listener, odk::IfHost* host);

        ~PluginChannel();

        std::uint32_t getLocalId() const;

        PluginChannel& setSampleFormat(odk::ChannelDataformat::SampleOccurrence occurrence,
            odk::ChannelDataformat::SampleFormat format, std::uint32_t dimension = 1);

        PluginChannel& setSimpleTimebase(double frequency);
        PluginChannel& setTimebaseWithOffset(double frequency, double offset);

        PluginChannel& setDefaultName(std::string_view name);
        const std::string& getDefaultName() const { return m_channel_info.m_default_name; }

        PluginChannel& setDomain(std::string_view name);
        const std::string& getDomain() const { return m_channel_info.m_domain; }

        PluginChannel& setDeletable(bool deletable);
        bool isDeletable() const { return m_channel_info.m_deletable; }

        const PluginChannelPtr& getLocalParent() const { return m_local_parent; }
        PluginChannel& setLocalParent(PluginChannelPtr ch);

        PluginChannel& setValid(const bool valid);
        PluginChannel& setRange(const odk::Range& range);
        PluginChannel& setRange(double range_min, double range_max, std::string_view unit);
        PluginChannel& setSamplerate(const odk::Scalar& sample_rate);
        PluginChannel& setUnit(std::string_view unit);

        PluginChannel& addProperty(const std::string& name, ChannelPropertyPtr prop);
        PluginChannel& addProperty(const std::string& name, const odk::Property& prop);
        void removeProperty(const std::string& name);

        ChannelPropertyPtr getProperty(const std::string& name) const;
        PluginChannel& replaceProperty(const std::string& name, ChannelPropertyPtr prop);

        PluginChannel& setProperty(const std::string& name, const odk::Property& prop);

        void updatePropertyTypes();

        std::shared_ptr<BooleanProperty> getUsedProperty() const;
        /**
         * @brief Returns if the channel is currently used (turned on)
         */
        bool getUsed() const;
        /**
         * @brief turns the channel on or off
         * @param value true for on, false for off
         */
        PluginChannel& setUsed(bool value);

        std::shared_ptr<RangeProperty> getRangeProperty() const;
        std::shared_ptr<EditableScalarProperty> getSamplerateProperty() const;
        std::shared_ptr<EditableStringProperty> getUnitProperty() const;

        const std::vector<std::pair<std::string, ChannelPropertyPtr>>& getProperties() const;

        /**
         * @brief returns the current name of the channel
         */
        std::string getName();

    protected:
        void setChangeListener(IfPluginChannelChangeListener* l);

        void onChannelPropertyChanged(const IfChannelProperty* channel) override;

        template <class T>
        odk::detail::ApiObjectPtr<const T> getChannelParam(const char* const key)
        {
            std::string channel_context = odk::queries::OxygenChannels;
            channel_context += '#';
            channel_context += std::to_string(getLocalId());

            return m_host->getValue<T>(channel_context.c_str(), key);
        }

        template <class T>
        odk::detail::ApiObjectPtr<const T> getChannelConfigParam(const char* const key)
        {
            std::string channel_context = odk::queries::OxygenChannels;
            channel_context += '#';
            channel_context += std::to_string(getLocalId());
            channel_context += "#Config#";
            channel_context += key;

            return m_host->getValue<T>(channel_context.c_str(), "Value");
        }

    protected:
        IfPluginChannelChangeListener* m_change_listener;
        odk::IfHost* m_host;
        odk::UpdateChannelsTelegram::PluginChannelInfo m_channel_info;
        std::vector<std::pair<std::string, ChannelPropertyPtr>> m_properties;
        PluginChannelPtr m_local_parent;
    };

    class PluginTask
    {
        friend class PluginChannels;
    public:
        PluginTask(std::uint64_t id, IfPluginTaskChangeListener* l, std::shared_ptr<IfTaskWorker> worker, std::uint64_t token = 0);

        void clearAllInputChannels();
        void addInputChannel(std::uint64_t ch_id);
        void addOutputChannel(PluginChannelPtr ch);

        void removeOutputChannel(PluginChannelPtr ch);

        std::uint64_t getID() const;

        bool isValid() const;
        void setValid(bool valid);

        void setBlockDuration(double duration);

    protected:

        void setChangeListener(IfPluginTaskChangeListener* l);

    protected:
        IfPluginTaskChangeListener* m_change_listener;
        std::uint64_t m_id;
        std::vector<std::uint64_t> m_input_channels;
        std::vector<PluginChannelPtr> m_output_channels;

        std::shared_ptr<IfTaskWorker> m_worker;
        std::uint64_t m_token;
        bool m_registered;
        bool m_valid;
        double m_block_duration;
    };

    class PluginChannels : public IfMessageHandler, public IfPluginChannelChangeListener, public IfPluginTaskChangeListener
    {
    public:

        ~PluginChannels();

        PluginChannelPtr addChannel();

        void removeChannel(PluginChannelPtr ch);

        /**
         * Sets the list topology below the given root channel (e.g. groups without a backing group channel)
         *  children of the given topology node become the children of the root channel in the host's channel tree,
         *  replacing the tree derived from the channels' local parents; channels not listed there are not shown.
         *  An empty topology restores the parent-derived tree.
         *  Subtrees of different root channels (e.g. of other instances) are independent.
         *  The topology is sent to the host on the next synchronize().
         */
        void setTopology(std::uint32_t root_local_id, const odk::UpdateChannelsTelegram::ChannelGroupInfo& topology);

        std::shared_ptr<PluginTask> addTask(IfTaskWorker* worker, uint64_t token = 0);

        std::shared_ptr<PluginTask> addTask(std::shared_ptr<IfTaskWorker> worker, uint64_t token = 0);

        void removeTask(std::shared_ptr<PluginTask> task);

        void setHost(odk::IfHost* host) override;

        void synchronize(bool register_tasks = true);

        void reset();

        void pauseTasks();
        void pauseTask(PluginTaskPtr& task);
        void restartTask(PluginTaskPtr& task);
    private:

        std::set<PluginTaskPtr> getAffectedTasks(const odk::UpdateConfigTelegram& request);

        std::uint64_t processConfigUpdate(const odk::UpdateConfigTelegram& request);

        std::uint64_t processDataFormatChange(const odk::ChannelDataformatTelegram& request);
        std::uint64_t processConfigItemsChanged(const odk::ChannelConfigChangedTelegram& telegram);
        std::uint64_t processConfigItemsChanged(const IfTaskWorker::ConfigItemChanges& changes);
        std::uint64_t processConfigItemsChanged(const std::set<PluginTaskPtr>& affected_tasks);

        std::uint64_t processChannelUpdateRequested();

        std::uint64_t reserveChannelIds(const odk::ChannelList& telegram);

        std::uint64_t pluginMessage(
            odk::PluginMessageId id,
            std::uint64_t key,
            const odk::IfValue* param,
            const odk::IfValue** ret) override;

        void onChannelSetupChanged(const PluginChannel* channel) override;

        void onChannelPropertyChanged(const PluginChannel* channel, const std::string& name) override;

        bool configChangeAllowed() const override;

        void onTaskChannelAdded(PluginTask* task, const PluginChannel* channel) override;
        void onTaskChannelRemoved(PluginTask* task, const PluginChannel* channel) override;
        void onTaskInputChannelsChanged(PluginTask* task) override;

        PluginTaskPtr findTask(uint64_t id);

        std::uint32_t generateId();

        void resetUsedIds();

        void registerTask(PluginTask& task);
        void unregisterTask(PluginTask& t);

    private:
        odk::IfHost* m_host = nullptr;

        std::map<std::uint32_t, PluginChannelPtr> m_channels;
        std::set<std::uint32_t> m_ids;
        std::set<std::uint32_t> m_reseved_ids;
        std::map<std::uint32_t, odk::UpdateChannelsTelegram::ChannelGroupInfo> m_list_topology; //< per root channel local id

        std::map<uint64_t, PluginTaskPtr> m_tasks;
        uint64_t m_next_task_id = 0;
        std::map<std::uint32_t, uint64_t> m_channel_to_task;

        bool m_channels_dirty = false; //< channels added, removed or reconfigured
        std::set<const PluginChannel*> m_properties_dirty;
    };

    template<class TargetClass>
    bool replacePropertyType(PluginChannel& channel, const std::string& property_name)
    {
        const auto original_prop =
            std::dynamic_pointer_cast<RawPropertyHolder>(channel.getProperty(property_name));
        if (original_prop)
        {
            auto property_replacer = std::make_shared<TargetClass>(*original_prop);
            channel.replaceProperty(property_name, property_replacer);
            return true;
        }
        return false;
    }

    template<class TargetClass>
    bool replacePropertyType(const PluginChannelPtr& channel, const std::string& property_name)
    {
        const auto original_prop =
            std::dynamic_pointer_cast<RawPropertyHolder>(channel->getProperty(property_name));
        if (original_prop)
        {
            auto property_replacer = std::make_shared<TargetClass>(*original_prop);
            channel->replaceProperty(property_name, property_replacer);
            return true;
        }
        return false;
    }

}
}
