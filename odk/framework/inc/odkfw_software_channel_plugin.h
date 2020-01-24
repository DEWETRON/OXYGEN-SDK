// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS

#include "odkfw_plugin_base.h"
#include "odkfw_software_channel_instance.h"

#include "odkapi_channel_list_xml.h"
#include "odkapi_event_ids.h"
#include "odkapi_software_channel_xml.h"
#include "odkapi_utils.h"

namespace odk
{
namespace framework
{

    class SoftwareChannelPluginBase : public PluginBase
    {
    public:

        virtual void registerTranslations() {};

        void addTranslation(const char* translation_xml);

    protected:

        void registerSoftwareChannel();
        void unregisterSoftwareChannel();

        virtual odk::RegisterSoftwareChannel getSoftwareChannelInfo() = 0;

        bool checkOxygenCompatibility();

    };

    template<class SoftwareChannelInstance>
    class SoftwareChannelPlugin : public SoftwareChannelPluginBase
    {

    private:
        odk::RegisterSoftwareChannel getSoftwareChannelInfo() final
        {
            return SoftwareChannelInstance::getSoftwareChannelInfo();
        }

        std::uint64_t init(std::string& error_message) final
        {
            if (!checkOxygenCompatibility())
            {
                error_message = "Current version of Oxygen is not supported.";
                return odk::error_codes::UNSUPPORTED_VERSION;
            }

            registerTranslations();
            registerSoftwareChannel();
            return odk::error_codes::OK;
        }

        bool deinit() final
        {
            getPluginChannels()->pauseTasks();
            m_instances.clear();
            unregisterSoftwareChannel();
            return true;
        }

        bool deleteChannels(std::vector<std::uint32_t> channels)
        {

            const auto containsChannel = [this,&channels](const std::shared_ptr<SoftwareChannelInstance>& inst)
            {
                for(const auto& channel : channels)
                {
                    if(inst->containsChannel(channel))
                    {
                        return true;
                    }
                }
                return false;
            };

            {
                std::vector<std::shared_ptr<SoftwareChannelInstance>> instances_to_remove;
                std::copy_if(m_instances.begin(), m_instances.end(), std::back_inserter(instances_to_remove), containsChannel);

                for (auto& instance : instances_to_remove)
                {
                    instance->shutDown();
                    m_instances.erase(std::remove(m_instances.begin(), m_instances.end(), instance), m_instances.end());
                }
            }

            getPluginChannels()->synchronize();

            return true;
        }

        bool createInstancesfromTelegram(
            const odk::UpdateChannelsTelegram& request,
            std::map<std::uint32_t, std::uint32_t>& root_channel_id_map)
        {
            auto root_channel_ids = getRootChannels(request);

            for (auto root_channel_id : root_channel_ids)
            {
                auto instance = std::make_shared<SoftwareChannelInstance>();
                instance->setPluginChannels(getPluginChannels());
                instance->initInstance(getHost());

                m_instances.push_back(instance);
                root_channel_id_map[root_channel_id] = instance->getRootChannel()->getLocalId();
            }
            return true;
        }

        bool mapRemovedChannels(std::map<std::uint64_t, std::uint64_t>& mapped_channels)
        {
            bool channel_removed = false;
            auto channel_ids_xml = getHost()->template getValue<odk::IfXMLValue>(odk::queries::OxygenChannels, odk::queries::OxygenChannels_AllIds);

            if (channel_ids_xml)
            {
                odk::ChannelList channel_list;
                channel_list.parse(channel_ids_xml->getValue());

                for (const auto& known_channel : m_all_channels.m_channels)
                {
                    if (std::find(channel_list.m_channels.begin(), channel_list.m_channels.end(), known_channel) == channel_list.m_channels.end())
                    {
                        mapped_channels[known_channel.m_channel_id] = static_cast<std::uint64_t>(-1);
                        channel_removed = true;
                    }
                    else
                    {
                        mapped_channels[known_channel.m_channel_id] = known_channel.m_channel_id;
                    }
                }
                m_all_channels = channel_list;
            }

            return channel_removed;
        }

        bool handleMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret, std::uint64_t& ret_code) override
        {
            switch (id)
            {
            case odk::plugin_msg::SOFTWARE_CHANNEL_CREATE:
            {
                odk::CreateSoftwareChannel telegram;

                if (parseXMLValue(param, telegram))
                {
                    auto instance = std::make_shared<SoftwareChannelInstance>();
                    instance->setPluginChannels(getPluginChannels());
                    instance->initInstance(getHost());

                    std::vector<InputChannel::InputChannelData> input_channel_data;
                    for (const auto a_channel : telegram.m_all_selected_channels_data)
                    {
                        input_channel_data.push_back({ a_channel.channel_id, a_channel.data_format });
                    }
                    instance->init(input_channel_data);
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
                            if (!response.m_show_channel_details)
                            {
                                response.m_show_channel_details = true;
                                response.m_detail_channel = ch->getLocalId();
                            }
                        }

                        const auto result_xml = response.generate();
                        auto result = getHost()->template createValue<odk::IfXMLValue>();
                        result->set(result_xml.c_str());
                        *ret = result.detach();
                    }
                    return true;
                }
                return false;
            }

            case odk::plugin_msg::DELETE_PLUGIN_CHANNELS:
            {
                odk::ChannelList channel_list;
                if (parseXMLValue(param, channel_list))
                {
                    std::vector<std::uint32_t> channels;
                    std::transform(channel_list.m_channels.begin(), channel_list.m_channels.end(), std::back_inserter(channels),
                        [](const odk::ChannelList::ChannelInfo& channel)
                        {
                            return static_cast<std::uint32_t>(channel.m_channel_id);
                        }
                    );
                    if (deleteChannels(channels))
                    {
                        return odk::error_codes::OK;
                    }
                    return odk::error_codes::INTERNAL_ERROR;
                }
                return odk::error_codes::INVALID_INPUT_PARAMETER;
            }

            case odk::plugin_msg::PLUGIN_CHANNEL_IDS_CHANGED:
            {
                odk::ChannelMappingTelegram<std::uint64_t> telegram;

                if (parseXMLValue(param, telegram))
                {
                    for (auto instance : m_instances)
                    {
                        instance->updateInternalInputChannelIDs(telegram.m_channel_id_map);
                        instance->updateInputChannelIDs(telegram.m_channel_id_map);
                        instance->handleConfigChange();
                        getPluginChannels()->synchronize();
                    }
                }
                return true;
            }

            case odk::plugin_msg::PLUGIN_RESET:
            {
                getPluginChannels()->pauseTasks();
                m_instances.clear();
                return true;
            }

            case odk::plugin_msg::PLUGIN_LOAD_CONFIGURATION:
            {
                odk::UpdateChannelsTelegram telegram;
                if (parseXMLValue(param, telegram))
                {
                    odk::ChannelMappingTelegram<std::uint32_t>::MapType id_map_root_channels;
                    if (createInstancesfromTelegram(telegram, id_map_root_channels))
                    {
                        odk::ChannelMappingTelegram<std::uint32_t> cm;
                        cm.m_channel_id_map.insert(id_map_root_channels.cbegin(), id_map_root_channels.cend());

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

                                if(const auto root_channel = telegram.getChannel(original_root_id))
                                {
                                    instance_telegram.appendChannel(*root_channel);
                                    //instance->updatePropertiesfromTelegram(instance_telegram, {{original_root_id, instance_root_id}});

                                    const auto instance_channel_ids =
                                        getChildrenOfChannel(
                                            telegram,
                                            original_root_id,
                                            false);

                                    for(const auto& instance_channel_id : instance_channel_ids)
                                    {
                                        if(const auto instance_channel = telegram.getChannel(instance_channel_id))
                                        {
                                            instance_telegram.appendChannel(*instance_channel);
                                        }
                                    }
                                }

                                odk::ChannelMappingTelegram<std::uint32_t>::MapType id_map;

                                id_map[original_root_id] = instance_root_id;

                                instance->configure(
                                    instance_telegram, id_map);

                                cm.m_channel_id_map.insert(id_map.cbegin(), id_map.cend());
                            }
                        }

                        getPluginChannels()->synchronize();

                        const auto result_xml = cm.generate();
                        auto result = getHost()->template createValue<odk::IfXMLValue>();
                        result->set(result_xml.c_str());
                        *ret = result.detach();

                    }
                }
                return true;
            }

            case odk::plugin_msg::PLUGIN_LOAD_FINISH:
            {
                for (auto instance : m_instances)
                {
                    instance->fetchInputChannels();
                    instance->handleConfigChange();
                    getPluginChannels()->synchronize();
                }
                return true;
            }

            case odk::plugin_msg::NOTIFY_EVENT:
            {
                switch (key)
                {
                case odk::EventIds::EVENT_ID_TOPOLOGY_CHANGED:
                {
                    std::map<std::uint64_t, std::uint64_t> mapped_channels;
                    if (mapRemovedChannels(mapped_channels))
                    {
                        for (auto instance : m_instances)
                        {
                            instance->updateInternalInputChannelIDs(mapped_channels);
                            instance->updateInputChannelIDs(mapped_channels);
                            instance->handleConfigChange();
                            getPluginChannels()->synchronize();
                        }
                    }
                    return true;
                }
                }
            }
            }

            return false;
        }

    private:
        std::vector<std::shared_ptr<SoftwareChannelInstance>> m_instances;
        odk::ChannelList m_all_channels;
    };

}
}

