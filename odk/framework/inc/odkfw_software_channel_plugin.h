// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS

#include "odkfw_channels.h"
#include "odkfw_custom_request_handler.h"
#include "odkfw_plugin_base.h"
#include "odkfw_software_channel_instance.h"

#include "odkapi_channel_list_xml.h"
#include "odkapi_event_ids.h"
#include "odkapi_software_channel_xml.h"
#include "odkapi_utils.h"

#include <type_traits>

namespace odk
{
namespace framework
{

    class SoftwareChannelPluginBase : public PluginBase
    {
    public:

        virtual void registerTranslations() {}
        virtual void registerResources() { registerTranslations(); }

        virtual bool validateInputChannels(const std::vector<InputChannel::InputChannelData>& input_channel_data,
            std::vector<std::uint64_t>& invalid_channels) {
            ODK_UNUSED(input_channel_data);
            ODK_UNUSED(invalid_channels);
            return true;
        }

    protected:
        SoftwareChannelPluginBase();

        std::uint64_t init(std::string& error_message) final;
        bool deinit() override;

        void registerSoftwareChannel();
        void updateSoftwareChannel();
        void unregisterSoftwareChannel();

        void registerCustomRequest();

        virtual odk::RegisterSoftwareChannel getSoftwareChannelInfo() = 0;

        PluginChannelsPtr getPluginChannels();
        std::shared_ptr<odk::framework::CustomRequestHandler> getCustomRequestHandler();

        bool checkOxygenCompatibility();

        /**
         * Creates a map of all channel IDs (old, existing and new) and maps them to their ID
         * Deleted channels will be mapped to the invalid ID -1
         * used as input to updateInternalInputChannelIDs and updateInputChannelIDs of instances
         * @returns true if channels have been removed, false if no channels have been removed
         */
        bool mapRemovedChannels(std::map<std::uint64_t, std::uint64_t>& mapped_channels);

        bool handleSofwareChannelQueryAction(const odk::IfValue* param, const odk::IfValue** ret);

        /**
         * method is called when odk::plugin_msg::NOTIFY_EVENT is handled
         * and handleNotifyEventEx did not already handle the event
         */
        virtual bool handleNotifyEvent(odk::EventIds event);
        /**
         * method is called when odk::plugin_msg::NOTIFY_EVENT is handled
         * default implementation calls handleNotifyEvent(event)
         */
        virtual bool handleNotifyEventEx(odk::EventIds event, const odk::IfValue* param);

    private:
        PluginChannelsPtr m_plugin_channels;
        odk::ChannelList m_all_channels;
        std::shared_ptr<odk::framework::CustomRequestHandler> m_custom_requests;
    };

    template<class SoftwareChannelInstance>
    class SoftwareChannelPlugin : public SoftwareChannelPluginBase
    {

    protected:
        using SoftwareChannelInstanceRequestFunction = std::function<std::uint64_t(SoftwareChannelInstance* instance, const odk::PropertyList& params, odk::PropertyList& returns)>;

        void registerCustomRequest(std::uint16_t id, const char* name, SoftwareChannelInstanceRequestFunction func)
        {
            namespace arg = std::placeholders;
            auto wrappedFunction = std::bind(&SoftwareChannelPlugin::forwardCustomRequest, this, func, arg::_1, arg::_2);
            getCustomRequestHandler()->registerFunction(id, name, wrappedFunction);
        }

        bool deinit() final
        {
            SoftwareChannelPluginBase::deinit();
            m_instances.clear();
            return true;
        }

    private:

        // see: https://stackoverflow.com/questions/52520276/is-decltype-of-a-non-static-member-function-ill-formed
        template<class> struct type_sink { typedef void type; };
        template<class T> using type_sink_t = typename type_sink<T>::type;
        template<class T, class = void> struct has_advanced_init : std::false_type {};
        template<class T> struct has_advanced_init<T, type_sink_t< decltype(T::getSoftwareChannelInfoEx) >> : std::true_type {};

        template<typename T>
        typename std::enable_if< has_advanced_init<T>::value, odk::RegisterSoftwareChannel >::type
        getSoftwareChannelInfoHelper()
        {
            return T::getSoftwareChannelInfoEx(getHost());
        }

        template<typename T>
        typename std::enable_if< !has_advanced_init<T>::value, odk::RegisterSoftwareChannel >::type
        getSoftwareChannelInfoHelper()
        {
            return T::getSoftwareChannelInfo();
        }

        odk::RegisterSoftwareChannel getSoftwareChannelInfo() final
        {
            // choose which function (getSoftwareChannelInfo or getSoftwareChannelInfoEx) to call at compile time
            return getSoftwareChannelInfoHelper<SoftwareChannelInstance>();
        }

        bool deleteChannels(const std::vector<std::uint32_t>& channels_requested)
        {
            std::map<std::shared_ptr<SoftwareChannelInstance>, std::vector<std::uint32_t>> instance_channels_to_remove;
            for (auto& instance : m_instances)
            {
                std::vector<std::uint32_t> requested_instance_channels;
                std::copy_if(
                    channels_requested.begin(), channels_requested.end(),
                    std::back_inserter(requested_instance_channels),
                    [&instance](std::uint32_t a_channel_id)
                    {
                        return instance->containsChannel(a_channel_id);
                    }
                );

                auto actual_instance_channels_to_delete =
                    instance->getChannelsToDelete(requested_instance_channels);

                instance_channels_to_remove.emplace(instance, std::move(actual_instance_channels_to_delete));
            }

            for (const auto& [an_instance, instance_channels] : instance_channels_to_remove)
            {
                for (std::uint32_t an_instance_channel : instance_channels)
                {
                    auto channel_ptr = an_instance->getOutputChannel(an_instance_channel);
                    an_instance->removeOutputChannel(channel_ptr);
                }
            }

            for (auto instance_channels_info : instance_channels_to_remove)
            {
                auto an_instance = instance_channels_info.first;
                if (an_instance->getOutputChannels().size() == 0)
                {
                    an_instance->shutDown();
                    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), an_instance), m_instances.end());
                }
            }

            instance_channels_to_remove.clear();
            getPluginChannels()->synchronize();

            return true;
        }

        bool createInstancesfromTelegram(
            const odk::UpdateChannelsTelegram& request,
            std::map<std::uint32_t, std::uint32_t>& root_channel_id_map)
        {
            auto root_channel_ids = getRootChannels(request);

            for (std::uint32_t root_channel_id : root_channel_ids)
            {
                auto instance = std::make_shared<SoftwareChannelInstance>();
                instance->setPluginChannels(getPluginChannels());
                instance->initInstance(getHost());
                instance->getRootChannel()->setDefaultName(request.getChannel(root_channel_id)->m_default_name);

                m_instances.push_back(instance);
                root_channel_id_map[root_channel_id] = instance->getRootChannel()->getLocalId();
            }
            return true;
        }

        bool handleSoftwareChannelCreate(const odk::IfValue* param, const odk::IfValue** ret)
        {
            odk::CreateSoftwareChannel telegram;

            if (!parseXMLValue(param, telegram))
            {
                return false;
            }

            auto instance = std::make_shared<SoftwareChannelInstance>();
            instance->setPluginChannels(getPluginChannels());
            instance->initInstance(getHost());

            std::vector<InputChannel::InputChannelData> input_channel_data;
            input_channel_data.reserve(telegram.m_all_selected_channels_data.size());
            for (const auto& a_channel : telegram.m_all_selected_channels_data)
            {
                input_channel_data.push_back({ a_channel.channel_id, a_channel.data_format, odk::Timebase() });
            }

            odk::framework::SoftwareChannelInstance::InitParams init_params{ input_channel_data, telegram.m_properties };
            auto init_result = std::static_pointer_cast<odk::framework::SoftwareChannelInstance>(instance)->init(init_params);
            if (!init_result.m_success)
            {
                return false;
            }

            instance->fetchInputChannels();

            instance->handleConfigChange();

            getPluginChannels()->synchronize();

            m_instances.push_back(instance);
            if (ret)
            {
                odk::CreateSoftwareChannelResponse response;
                for (const auto& ch : instance->getOutputChannels())
                {
                    response.m_channels.push_back(ch->getLocalId());
                    if (init_result.m_channel_list_action ==
                        odk::framework::SoftwareChannelInstance::InitResult::ChannelListAction::SHOW_DETAILS_OF_FIRST_CHANNEL)
                    {
                        if (!response.m_show_channel_details)
                        {
                            response.m_show_channel_details = true;
                            response.m_detail_channel = ch->getLocalId();
                        }
                    }
                }

                if (init_result.m_channel_list_action ==
                    odk::framework::SoftwareChannelInstance::InitResult::ChannelListAction::SHOW_DETAILS_OF_SPECIFIED_CHANNEL)
                {
                    response.m_show_channel_details = true;
                    response.m_detail_channel = init_result.m_detail_channel;
                }

                const auto result_xml = response.generate();
                auto result = getHost()->template createValue<odk::IfXMLValue>();
                result->set(result_xml.c_str());
                *ret = result.detach();
            }
            return true;
        }

        std::uint64_t handleDeletePluginChannel(const odk::IfValue* param)
        {
            odk::ChannelList channel_list;
            if (!parseXMLValue(param, channel_list))
            {
                return odk::error_codes::INVALID_INPUT_PARAMETER;
            }

            std::vector<std::uint32_t> channels;
            channels.reserve(channel_list.m_channels.size());
            std::transform(channel_list.m_channels.begin(), channel_list.m_channels.end(), std::back_inserter(channels),
                [](const odk::ChannelList::ChannelInfo& channel)
                {
                    return static_cast<std::uint32_t>(channel.m_channel_id);
                }
            );

            if (!deleteChannels(channels))
            {
                return odk::error_codes::INTERNAL_ERROR;
            }

            return odk::error_codes::OK;
        }

        bool handlePluginChannelIdsChanged(const odk::IfValue* param)
        {
            odk::ChannelMappingTelegram<std::uint64_t> telegram;

            if (parseXMLValue(param, telegram))
            {
                for (auto& instance : m_instances)
                {
                    instance->updateInternalInputChannelIDs(telegram.m_channel_id_map);
                    instance->updateInputChannelIDs(telegram.m_channel_id_map);
                    instance->handleConfigChange();
                    getPluginChannels()->synchronize();
                }
            }
            return true;
        }

        bool handlePluginReset()
        {
            getPluginChannels()->pauseTasks();
            m_instances.clear();
            return true;
        }

        bool hasInstance(const odk::IfValue** ret)
        {
            auto result = getHost()->template createValue<odk::IfBooleanValue>();
            result->set(!m_instances.empty());
            *ret = result.detach();
            return true;
        }

        bool handlePluginLoadConfiguration(const odk::IfValue* param, const odk::IfValue** ret)
        {
            odk::UpdateChannelsTelegram telegram;
            if (!parseXMLValue(param, telegram))
            {
                return true;
            }

            odk::ChannelMappingTelegram<std::uint32_t>::MapType id_map_root_channels;
            if (createInstancesfromTelegram(telegram, id_map_root_channels))
            {
                odk::ChannelMappingTelegram<std::uint32_t> cm;
                cm.m_channel_id_map.insert(id_map_root_channels.cbegin(), id_map_root_channels.cend());

                std::set<std::shared_ptr<SoftwareChannelInstance>> instances_to_remove;

                for (auto instance : m_instances)
                {
                    const auto instance_root_id = instance->getRootChannel()->getLocalId();
                    const auto original_root_id_it =
                        std::find_if(id_map_root_channels.begin(), id_map_root_channels.end(),
                            [instance_root_id](
                                const odk::ChannelMappingTelegram<std::uint32_t>::MapType::value_type& an_id_mapping)
                            {
                                return an_id_mapping.second == instance_root_id;
                            });

                    if (original_root_id_it != id_map_root_channels.end())
                    {
                        const auto original_root_id = original_root_id_it->first;

                        odk::UpdateChannelsTelegram instance_telegram;

                        if (const auto root_channel = telegram.getChannel(original_root_id))
                        {
                            instance_telegram.appendChannel(*root_channel);
                            //instance->updatePropertiesfromTelegram(instance_telegram, {{original_root_id, instance_root_id}});

                            const auto instance_channel_ids =
                                getChildrenOfChannel(
                                    telegram,
                                    original_root_id,
                                    true);

                            for (const auto& instance_channel_id : instance_channel_ids)
                            {
                                if (const auto instance_channel = telegram.getChannel(instance_channel_id))
                                {
                                    instance_telegram.appendChannel(*instance_channel);
                                }
                            }
                        }

                        odk::ChannelMappingTelegram<std::uint32_t>::MapType id_map;

                        id_map[original_root_id] = instance_root_id;

                        bool configuration_successful = instance->configure(instance_telegram, id_map);
                        if (configuration_successful)
                        {
                            cm.m_channel_id_map.insert(id_map.cbegin(), id_map.cend());
                        }
                        else
                        {
                            cm.m_channel_id_map.erase(original_root_id);
                            instances_to_remove.insert(instance);
                        }
                    }
                }

                for (const auto& an_instance : instances_to_remove)
                {
                    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), an_instance), m_instances.end());
                }

                instances_to_remove.clear();

                getPluginChannels()->synchronize();

                const auto result_xml = cm.generate();
                auto result = getHost()->template createValue<odk::IfXMLValue>();
                result->set(result_xml.c_str());
                *ret = result.detach();
            }
            return true;
        }

        bool handlePluginLoadFinish()
        {
            for (const auto& instance : m_instances)
            {
                instance->fetchInputChannels();
                instance->handleConfigChange();
                getPluginChannels()->synchronize();
                instance->loadFinished();
            }
            return true;
        }

        bool handleNotifyEvent(odk::EventIds event) override
        {
            switch (event)
            {
            case odk::EventIds::EVENT_ID_TOPOLOGY_CHANGED:
            {
                std::map<std::uint64_t, std::uint64_t> mapped_channels;
                if (mapRemovedChannels(mapped_channels))
                {
                    getPluginChannels()->pauseTasks();
                    for (const auto& instance : m_instances)
                    {
                        instance->updateInternalInputChannelIDs(mapped_channels);
                        instance->updateInputChannelIDs(mapped_channels);
                        instance->handleConfigChange();
                    }
                    getPluginChannels()->synchronize();
                }
                return true;
            }

            default:
                return SoftwareChannelPluginBase::handleNotifyEvent(event);
            }
        }

        bool handleNotifyEventEx(odk::EventIds event, const odk::IfValue* param) override
        {
            switch (event)
            {
            case odk::EventIds::EVENT_ID_MEASUREMENT_ACTIVE:
            {
                auto xml_param = dynamic_cast<const odk::IfXMLValue*>(param);
                for (const auto& instance : m_instances)
                {
                    instance->measurementStartedEx(xml_param);
                }
                return true;
            }

            case odk::EventIds::EVENT_ID_MEASUREMENT_STOPPED:
            {
                auto xml_param = dynamic_cast<const odk::IfXMLValue*>(param);
                for (const auto& instance : m_instances)
                {
                    instance->measurementStoppedEx(xml_param);
                }
                return true;
            }

            default:
                return handleNotifyEvent(event);
            }
        }

        bool handleMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret, std::uint64_t& ret_code) override
        {
            switch (id)
            {
            case odk::plugin_msg::SOFTWARE_CHANNEL_QUERY_ACTION:
                return handleSofwareChannelQueryAction(param, ret);

            case odk::plugin_msg::SOFTWARE_CHANNEL_CREATE:
                return handleSoftwareChannelCreate(param, ret);

            case odk::plugin_msg::DELETE_PLUGIN_CHANNELS:
                ret_code = handleDeletePluginChannel(param);
                return true;

            case odk::plugin_msg::PLUGIN_CHANNEL_IDS_CHANGED:
                return handlePluginChannelIdsChanged(param);

            case odk::plugin_msg::PLUGIN_RESET:
                return handlePluginReset();

            case odk::plugin_msg::PLUGIN_LOAD_CONFIGURATION:
                return handlePluginLoadConfiguration(param, ret);

            case odk::plugin_msg::PLUGIN_LOAD_FINISH:
                return handlePluginLoadFinish();

            case odk::plugin_msg::NOTIFY_EVENT:
                return handleNotifyEventEx(static_cast<odk::EventIds>(key), param);

            case odk::plugin_msg::SOFTWARE_CHANNEL_HAS_INSTANCE:
                return hasInstance(ret);

            default:
                return false;
            }
        }

        std::uint64_t forwardCustomRequest(SoftwareChannelInstanceRequestFunction func, const odk::PropertyList& params, odk::PropertyList& return_list)
        {
            auto root_id = params.getChannelId("SoftwareChannelInstanceRootID");
            for(auto& instance : m_instances)
            {
                if(instance->getRootChannel()->getLocalId() == root_id)
                {
                    instance->shutDown();
                    func(instance.get(), params, return_list);
                    break;
                }
            }
            getPluginChannels()->synchronize();
            return odk::error_codes::OK;
        }

    private:
        std::vector<std::shared_ptr<SoftwareChannelInstance>> m_instances;
    };
}
}

