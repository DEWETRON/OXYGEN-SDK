// Copyright DEWETRON GmbH 2019

#include "odkfw_software_channel_instance.h"

#define ODK_EXTENSION_FUNCTIONS

#include "odkapi_block_descriptor_xml.h"
#include "odkapi_data_set_xml.h"
#include "odkapi_error_codes.h"
#include "odkapi_utils.h"
#include "odkbase_message_return_value_holder.h"
#include "odkfw_channels.h"
#include "odkfw_exceptions.h"
#include "odkfw_properties.h"
#include "odkfw_property_list_utils.h"
#include "odkfw_stream_reader.h"
#include "odkuni_logger.h"
#include "odkuni_assert.h"

namespace odk
{
namespace framework
{
    static const char* INSTANCE_CHANNEL_KEY = "SoftwareChannelInstanceKey";
    static const char* INSTANCE_ROOT_ID_KEY = "SoftwareChannelInstanceRootID";

    namespace
    {

        void addDefaultProperties(PluginChannelPtr& channel, const std::string& instance_channel_key)
        {
            auto key_property = std::make_shared<EditableStringProperty>(instance_channel_key);
            key_property->setVisiblity("HIDDEN");
            channel->addProperty(INSTANCE_CHANNEL_KEY, key_property);
            channel->addProperty("Used", std::make_shared<BooleanProperty>(true));
        }

    }

    std::map<uint64_t, odk::framework::StreamIterator> SoftwareChannelInstance::createChannelIterators(
        const std::vector<odk::StreamDescriptor>& stream_descriptor,
        const odk::IfDataBlockList* block_list)
    {
        odk::DataRegions data_regions;

        for (const auto& sd : stream_descriptor)
        {
            for (auto& channel : sd.m_channel_descriptors)
            {
                data_regions.m_data_regions.push_back(DataRegion(channel.m_channel_id, odk::Interval<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max())));
            }
        }
        return createChannelIterators(stream_descriptor,
                                      block_list,
                                      odk::Interval<double>(0, std::numeric_limits<double>::max()),
                                      data_regions);
    }

    std::map<uint64_t, odk::framework::StreamIterator> SoftwareChannelInstance::createChannelIterators(
        const std::vector<odk::StreamDescriptor>& stream_descriptor,
        const odk::IfDataBlockList* block_list,
        const odk::Interval<double>& interval,
        const odk::DataRegions& data_regions)
    {

        std::map<uint64_t, odk::framework::StreamIterator> iterators;

        auto block_list_descriptor_xml = odk::ptr(block_list->getBlockListDescription());
        BlockListDescriptor list_descriptor;
        list_descriptor.parse(block_list_descriptor_xml->getValue());


        odk::framework::StreamReader stream_reader;

        const auto block_count = block_list->getBlockCount();
        for (int i = 0; i < block_count; ++i)
        {
            auto block = odk::ptr(block_list->getBlock(i));
            auto block_descriptor_xml = odk::ptr(block->getBlockDescription());

            BlockDescriptor block_descriptor;
            block_descriptor.parse(block_descriptor_xml->getValue());

            if (!block_descriptor.m_block_channels.empty())
            {
                stream_reader.addDataBlock(std::move(block_descriptor), block->data());
            }
        }

        for(const auto& data_region : data_regions.m_data_regions)
        {
            stream_reader.addDataRegion(data_region);
        }

        // for every stream, create a reader on the data blocks and read the data
        for (const auto& sd : stream_descriptor)
        {
            stream_reader.setStreamDescriptor(sd);

            for (auto& channel : sd.m_channel_descriptors)
            {
                odk::Interval<std::uint64_t> channel_interval(0, std::numeric_limits<std::uint64_t>::max());
                if(interval.m_begin >= 0.0 && interval.m_end < std::numeric_limits<double>::max())
                {
                    channel_interval.m_begin = convertTimeToTickAtOrAfter(interval.m_begin, getInputChannelProxy(channel.m_channel_id)->getTimeBase().m_frequency);
                    channel_interval.m_end = convertTimeToTickAtOrAfter(interval.m_end, getInputChannelProxy(channel.m_channel_id)->getTimeBase().m_frequency);
                }
                iterators[channel.m_channel_id] = stream_reader.createChannelIterator(channel.m_channel_id, channel_interval);
            }
        }

        return iterators;
    }

    SoftwareChannelInstance::InitResult SoftwareChannelInstance::init(const InitParams& params)
    {
        try
        {
            //default implementation for source code compatibility with old plugins
            if (setup(params.m_properties))
            {
                init(params.m_input_channels);
                InitResult r{ true };
                r.m_channel_list_action = InitResult::ChannelListAction::SHOW_DETAILS_OF_FIRST_CHANNEL;
                return r;
            }
        }
        catch (const std::exception& e)
        {
            ODKLOG_ERROR("Unhandled exception during 'init': " << e.what());
        }
        catch (...)
        {
            ODKLOG_ERROR("Unhandled exception duriong 'init'");
        }
        return { false };
    }

    std::vector<std::uint32_t> SoftwareChannelInstance::getChannelsToDelete(std::vector<std::uint32_t> requested_chanels)
    {
        const auto root = getRootChannel();
        if (requested_chanels.size() == 0 || !root)
        {
            return {};
        }

        //if any selected, all are meant (original behaviour)
        std::vector < std::uint32_t> channels_to_delete;
        const auto all_channels = getOutputChannels();
        for (const auto a_channel : all_channels)
        {
            channels_to_delete.push_back(a_channel->getLocalId());
        }

        return channels_to_delete;
    }

    SoftwareChannelInstance::~SoftwareChannelInstance()
    {
        if (!m_plugin_channels)
        {
            return;
        }

        if (m_task)
        {
            m_plugin_channels->removeTask(m_task);
        }
        for(const auto& channel : m_output_channels)
        {
            m_plugin_channels->removeChannel(channel);
        }
        m_plugin_channels->synchronize();
    }

    void SoftwareChannelInstance::shutDown()
    {
        if (m_task)
        {
            m_plugin_channels->pauseTask(m_task);
        }
    }

    void SoftwareChannelInstance::initInstance(odk::IfHost* host)
    {
        try
        {
            m_host = host;

            auto channel = addOutputChannel("root");
            channel->setLocalParent(nullptr);
            channel->setDeletable(true);

            channel->setSampleFormat(
                odk::ChannelDataformat::SampleOccurrence::NEVER,
                odk::ChannelDataformat::SampleFormat::NONE,
                0);

            m_data_request_type = DataRequestType::STREAM;
            m_data_request_interval = 0.01;

            create(m_host);
        }
        catch (const std::exception& e)
        {
            ODKLOG_ERROR("Unhandled exception during 'initInstance': " << e.what());
        }
        catch (...)
        {
            ODKLOG_ERROR("Unhandled exception during 'initInstance'");
        }
    }

    void SoftwareChannelInstance::setDataRequestType(DataRequestType type)
    {
        m_data_request_type = type;
    }

    void SoftwareChannelInstance::setDataRequestInterval(double seconds)
    {
        m_data_request_interval = seconds;
    }

    void SoftwareChannelInstance::synchronize()
    {
        m_plugin_channels->synchronize();
    }

    void SoftwareChannelInstance::setPluginChannels(PluginChannelsPtr plugin_channels)
    {
        m_plugin_channels = plugin_channels;
        m_task = m_plugin_channels->addTask(this);
    }

    void SoftwareChannelInstance::onInitTimebases(odk::IfHost* host, std::uint64_t token)
    {
        ODK_UNUSED(token);
        try
        {
            for (auto& input_channel : m_input_channel_proxies)
            {
                input_channel->updateTimeBase();
            }

            initTimebases(host);
            m_plugin_channels->synchronize(false);
        }
        catch (const std::exception& e)
        {
            ODKLOG_ERROR("Unhandled exception during 'initTimebases': " << e.what());
        }
        catch (...)
        {
            ODKLOG_ERROR("Unhandled exception during 'initTimebases'");
        }
    }

    void SoftwareChannelInstance::onStartProcessing(odk::IfHost* host, std::uint64_t token)
    {
        ODK_UNUSED(token);

        try
        {
            if (!m_input_channel_proxies.empty())
            {
                setupDataRequest(host);

                if (m_dataset_descriptor && m_data_request_type == DataRequestType::STREAM)
                {
                    PluginDataStartRequest req;
                    req.m_id = m_dataset_descriptor->m_id;
                    req.m_stream_type = StreamType::PULL;
                    req.m_ignore_regions = true;
                    req.m_block_duration = m_data_request_interval;

                    auto msg = host->createValue<odk::IfXMLValue>();
                    msg->set(req.generate().c_str());
                    host->messageAsync(odk::host_msg::DATA_START_REQUEST, 0, msg.get());
                }
            }

            prepareProcessing(host);
            return;
        }
        catch (const std::exception& e)
        {
            ODKLOG_ERROR("Unhandled exception during 'prepareProcessing': " << e.what());
        }
        catch (...)
        {
            ODKLOG_ERROR("Unhandled exception during 'prepareProcessing'");
        }
        throw odk::framework::CallbackError(odk::error_codes::UNHANDLED_EXCEPTION);
    }

    void SoftwareChannelInstance::onStopProcessing(odk::IfHost *host, std::uint64_t token)
    {
        ODK_UNUSED(token);

        try
        {
            if (m_dataset_descriptor)
            {
                if (m_data_request_type == DataRequestType::STREAM)
                {
                    PluginDataStopRequest req;
                    req.m_id = m_dataset_descriptor->m_id;

                    auto msg = host->createValue<odk::IfXMLValue>();
                    msg->set(req.generate().c_str());
                    host->messageAsync(odk::host_msg::DATA_STOP_REQUEST, 0, msg.get());
                }

                auto msg = m_host->createValue<odk::IfUIntValue>();
                msg->set(m_dataset_descriptor->m_id);
                m_host->messageSync(odk::host_msg::DATA_GROUP_REMOVE, 0, msg.get(), nullptr);

                m_dataset_descriptor = boost::none;
            }

            stopProcessing(host);
            return;
        }
        catch (const std::exception& e)
        {
            ODKLOG_ERROR("Unhandled exception during 'stopProcessing': " << e.what());
        }
        catch (...)
        {
            ODKLOG_ERROR("Unhandled exception during 'stopProcessing'");
        }
        throw odk::framework::CallbackError(odk::error_codes::UNHANDLED_EXCEPTION);
    }

    std::map<uint64_t, odk::framework::StreamIterator> SoftwareChannelInstance::getSamplesAt(double time)
    {
        if(m_dataset_descriptor)
        {
            auto xml_msg = m_host->createValue<odk::IfXMLValue>();
            if (xml_msg)
            {
                PluginDataRequest req(m_dataset_descriptor->m_id, PluginDataRequest::SingleValue(time));
                xml_msg->set(req.generate().c_str());

                const odk::IfValue* response = nullptr;
                if (0 != m_host->messageSync(odk::host_msg::DATA_READ, 0, xml_msg.get(), &response))
                {
                    return {};
                }

                if (auto block_list = odk::value_cast<odk::IfDataBlockList>(response))
                {
                    m_block_lists.push_back(block_list);
                    return createChannelIterators(m_dataset_descriptor->m_stream_descriptors, block_list);
                }
            }
        }
        return {};
    }

    void SoftwareChannelInstance::onProcess(odk::IfHost *host, std::uint64_t token, const odk::IfXMLValue* param)
    {
        ODK_UNUSED(token);

        std::uint64_t ret = odk::error_codes::OK;
        odk::AcquisitionTaskProcessTelegram telegram;
        if (param)
        {
            telegram.parse(param->getValue());
        }

        ProcessingContext context;
        const auto master_timebase = getMasterTimestamp(host);
        context.m_master_timestamp = master_timebase;

        if (m_dataset_descriptor && telegram.m_start.timestampValid() && telegram.m_end.timestampValid())
        {
            odk::DataRegions data_regions;
            {
                auto xml_msg = host->createValue<odk::IfXMLValue>();
                if (xml_msg)
                {
                    double start = telegram.m_start.m_ticks / telegram.m_start.m_frequency;
                    double end = telegram.m_end.m_ticks / telegram.m_end.m_frequency;
                    PluginDataRegionsRequest req(m_dataset_descriptor->m_id);
                    req.m_data_window = PluginDataRegionsRequest::DataWindow(start, end);
                    xml_msg->set(req.generate().c_str());
                }

                const odk::IfValue* data_regions_result = nullptr;
                host->messageSync(odk::host_msg::DATA_REGIONS_READ, 0, xml_msg.get(), &data_regions_result);

                const odk::IfXMLValue* data_regions_result_xml = odk::value_cast<odk::IfXMLValue>(data_regions_result);
                if (data_regions_result)
                {
                    if (data_regions_result_xml)
                    {
                        data_regions.parse(data_regions_result_xml->getValue());
                    }
                    data_regions_result->release();
                }
            }
            auto xml_msg = host->createValue<odk::IfXMLValue>();
            if (xml_msg)
            {
                double start = telegram.m_start.m_ticks / telegram.m_start.m_frequency;
                double end = telegram.m_end.m_ticks / telegram.m_end.m_frequency;
                PluginDataRequest req(m_dataset_descriptor->m_id, PluginDataRequest::DataWindow(start, end));
                xml_msg->set(req.generate().c_str());
            }

            const odk::IfValue* response = nullptr;

            if (0 != host->messageSync(odk::host_msg::DATA_READ, 0, xml_msg.get(), &response))
            {
                return;
            }

            if (auto block_list = odk::value_cast<odk::IfDataBlockList>(response))
            {
                auto block_list_descriptor_xml = odk::ptr(block_list->getBlockListDescription());
                BlockListDescriptor list_descriptor;
                list_descriptor.parse(block_list_descriptor_xml->getValue());

                if (list_descriptor.m_windows.empty())
                {
                    response->release();
                    return;
                }

                context.m_window.first = list_descriptor.m_windows.front().m_begin;
                context.m_window.second = list_descriptor.m_windows.back().m_end;

                context.m_channel_iterators = createChannelIterators(m_dataset_descriptor->m_stream_descriptors,
                                                                     block_list,
                                                                     odk::Interval<double>(context.m_window.first, context.m_window.second),
                                                                     data_regions);

                process(context, host);
                response->release();
            }
        }
        else if (m_dataset_descriptor && m_data_request_type == DataRequestType::STREAM)
        {
            const double max_time = master_timebase.m_ticks / master_timebase.m_frequency;
            double current_time = 0.0;
            while (current_time < max_time)
            {

                auto xml_msg = host->createValue<odk::IfXMLValue>();
                if (xml_msg)
                {
                    PluginDataRequest req(m_dataset_descriptor->m_id, PluginDataRequest::DataStream());
                    xml_msg->set(req.generate().c_str());
                }

                odk::MessageReturnValueHolder<odk::IfDataBlockList> block_list;
                if (0 != host->messageSync(odk::host_msg::DATA_READ, 0, xml_msg.get(), block_list.data()))
                {
                    throw odk::framework::CallbackError(odk::error_codes::INTERNAL_ERROR);
                }

                if (block_list.valid())
                {
                    auto block_list_descriptor_xml = odk::ptr(block_list->getBlockListDescription());
                    BlockListDescriptor list_descriptor;
                    list_descriptor.parse(block_list_descriptor_xml->getValue());

                    if (list_descriptor.m_windows.empty())
                    {
                        return;
                    }

                    context.m_window.first = list_descriptor.m_windows.front().m_begin;
                    context.m_window.second = list_descriptor.m_windows.back().m_end;

                    context.m_channel_iterators = createChannelIterators(m_dataset_descriptor->m_stream_descriptors, block_list.ref());

                    try
                    {
                        process(context, host);
                    }
                    catch (const std::exception& e)
                    {
                        ODKLOG_ERROR("Unhandled exception during 'process': " << e.what());
                        ret = odk::error_codes::UNHANDLED_EXCEPTION;
                    }
                    catch (...)
                    {
                        ODKLOG_ERROR("Unhandled exception during 'process'");
                        ret = odk::error_codes::UNHANDLED_EXCEPTION;
                    }

                    current_time = context.m_window.second;
                }
            }
        }
        else
        {
            try
            {
                process(context, host);
            }
            catch (const std::exception& e)
            {
                ODKLOG_ERROR("Unhandled exception during 'process': " << e.what());
                ret = odk::error_codes::UNHANDLED_EXCEPTION;
            }
            catch (...)
            {
                ODKLOG_ERROR("Unhandled exception during 'process'");
                ret = odk::error_codes::UNHANDLED_EXCEPTION;
            }
        }

        // free manually retrieved data block lists
        for(auto& block_list : m_block_lists)
        {
            block_list->release();
        }
        m_block_lists.clear();

        if (ret != odk::error_codes::OK)
        {
            throw odk::framework::CallbackError(ret);
        }
    }

    void SoftwareChannelInstance::onChannelConfigChanged(odk::IfHost* host, std::uint64_t token)
    {
        ODK_UNUSED(host);
        ODK_UNUSED(token);

        fetchInputChannels();
        handleConfigChange();
        constexpr bool register_task = false; // the task needs to be registered by the caller
        m_plugin_channels->synchronize(register_task);
    }

    void SoftwareChannelInstance::setupDataRequest(odk::IfHost *host)
    {
        // move to DataRequester helper class
        PluginDataSet request;
        request.m_id = m_task->getID();
        request.m_data_set_type = DataSetType::SCALED;
        request.m_data_mode = DataSetMode::NORMAL;
        request.m_policy = StreamPolicy::EXACT;

        for(const auto& channel : m_input_channel_proxies)
        {
            request.m_channels.push_back(channel->getChannelId());
        }

        auto xml_msg = host->createValue<odk::IfXMLValue>();
        if (!xml_msg)
        {
            return;
        }

        // create XML request
        const auto xml_content = request.generate();
        xml_msg->set(xml_content.c_str());

        const odk::IfValue* group_add_result = nullptr;
        host->messageSync(odk::host_msg::DATA_GROUP_ADD, 0, xml_msg.get(), &group_add_result);

        const odk::IfXMLValue* group_add_result_xml = odk::value_cast<odk::IfXMLValue>(group_add_result);
        if (group_add_result)
        {
            if (group_add_result_xml)
            {
                m_dataset_descriptor = DataSetDescriptor();
                m_dataset_descriptor->parse(group_add_result_xml->getValue());
            }
            group_add_result->release();
        }
    }

    bool SoftwareChannelInstance::containsChannel(std::uint32_t channel_id)
    {
        return std::find_if(m_output_channels.begin(),
                            m_output_channels.end(),
                            [&channel_id](const PluginChannelPtr& channel)
                            {
                            return channel->getLocalId() == channel_id;
                            })
            != m_output_channels.end();
    }

    bool SoftwareChannelInstance::handleConfigChange()
    {
        const auto is_valid = update();
        bool output_channel_used = false;
        bool all_input_channels_usable = true;
        for (auto an_input_channel : m_input_channel_proxies)
        {
            all_input_channels_usable &= an_input_channel->isUsable();
        }
        for (auto an_output_channel : m_output_channels)
        {
            const auto& used_property = an_output_channel->getUsedProperty();
            const bool this_output_channel_used = (!used_property || used_property->getValue());
            output_channel_used |= this_output_channel_used;

            if (const auto parent = an_output_channel->getLocalParent())
            {
                if (const auto& parent_used_property = parent->getUsedProperty())
                {
                    bool val =
                        (parent_used_property->getValue() & this_output_channel_used);
                    used_property->setValue(val);
                }
            }
            an_output_channel->setValid(is_valid && all_input_channels_usable);
        }

        m_task->setValid(is_valid && output_channel_used && all_input_channels_usable);
        return is_valid;
    }

    void SoftwareChannelInstance::updateInternalInputChannelIDs(const std::map<std::uint64_t, std::uint64_t>& changed_ids)
    {
        for(auto& channel : getOutputChannels())
        {
            for(auto& property : channel->getProperties())
            {
                if(auto channel_id_property = std::dynamic_pointer_cast<EditableChannelIDProperty>(property.second))
                {
                    const auto mapping_it = changed_ids.find(channel_id_property->getValue());
                    if (mapping_it != changed_ids.cend())
                    {
                        channel_id_property->setValue(mapping_it->second);
                    }
                }
                else if(auto channel_list_property = std::dynamic_pointer_cast<EditableChannelIDListProperty>(property.second))
                {
                    const auto current_channel_ids(channel_list_property->getValue().m_values);
                    std::vector<std::uint64_t> new_input_channel_ids;

                    //preserve order
                    for (const auto& a_old_channel : current_channel_ids)
                    {
                        const auto mapping_it = changed_ids.find(a_old_channel);
                        if (mapping_it != changed_ids.cend())
                        {
                            new_input_channel_ids.push_back(mapping_it->second);
                        }
                    }

                    channel_list_property->setValue(odk::ChannelIDList(new_input_channel_ids));
                }
            }
        }

        fetchInputChannels();
    }

    PluginChannelPtr SoftwareChannelInstance::getRootChannel()
    {
        if (m_output_channels.size() > 0)
        {
            return m_output_channels[0];
        }

        return {};
    }

    std::vector<PluginChannelPtr> SoftwareChannelInstance::getOutputChannels()
    {
        return m_output_channels;
    }

    std::string SoftwareChannelInstance::getKey(const PluginChannelPtr &channel) const
    {
        const auto& key_property = std::dynamic_pointer_cast<EditableStringProperty>(channel->getProperty(INSTANCE_CHANNEL_KEY));
        return key_property->getValue();
    }

    void SoftwareChannelInstance::enqueueRequest(uint16_t id, const PropertyList &params)
    {
        auto properties = params;
        properties.setChannelId(INSTANCE_ROOT_ID_KEY, getRootChannel()->getLocalId());
        auto value = odk::framework::utils::convertToXMLValue(getHost(), properties);
        getHost()->messageAsync(odk::plugin_msg::CUSTOM_QML_REQUEST, id, value);
    }

    void SoftwareChannelInstance::fetchInputChannels()
    {
        clearRequestedInputChannels();
        std::set<odk::ChannelID> all_input_channels;

        for(auto& channel : getOutputChannels())
        {
            for(auto& property : channel->getProperties())
            {
                if(auto channel_id_property = std::dynamic_pointer_cast<EditableChannelIDProperty>(property.second))
                {
                    all_input_channels.insert(channel_id_property->getValue());
                }
                else if(auto channel_list_property = std::dynamic_pointer_cast<EditableChannelIDListProperty>(property.second))
                {
                    const auto& values = channel_list_property->getValue().m_values;
                    all_input_channels.insert(values.begin(), values.end());
                }
            }
        }

        for (const auto& channel : all_input_channels)
        {
            requestInputChannel(channel);
        }
    }

    bool SoftwareChannelInstance::checkInputChannelFormats(const std::set<odk::ChannelDataformat::SampleValueType>& allowed_value_types,
                                    const std::set<odk::ChannelDataformat::SampleOccurrence>& allowed_occurence)
    {
        for (const auto& an_input_channel : getInputChannelProxies())
        {
            if (an_input_channel->updateDataFormat())
            {
                const auto dataformat = an_input_channel->getDataFormat();
                if(allowed_value_types.find(dataformat.m_sample_value_type) == allowed_value_types.end())
                {
                    return false;
                }
                if(allowed_occurence.find(dataformat.m_sample_occurrence) == allowed_occurence.end())
                {
                    return false;
                }
            }
        }
        return true;
    }

    void SoftwareChannelInstance::clearRequestedInputChannels()
    {
        m_input_channel_proxies.clear();
        m_task->clearAllInputChannels();
    }

    // add additional params (format, )
    void SoftwareChannelInstance::requestInputChannel(std::uint64_t channel_id)
    {
        auto new_input_channel = std::make_shared<InputChannel>(m_host, channel_id);
        new_input_channel->updateDataFormat();
        m_input_channel_proxies.push_back(new_input_channel);
        m_task->addInputChannel(channel_id);
    }

    void SoftwareChannelInstance::removeOutputChannel(PluginChannelPtr& channel)
    {
        m_task->removeOutputChannel(channel);
        m_plugin_channels->removeChannel(channel);
        auto pos = std::find(m_output_channels.begin(), m_output_channels.end(), channel);
        if (pos != m_output_channels.end())
        {
            m_output_channels.erase(pos);
        }
    }

    PluginChannelPtr SoftwareChannelInstance::addOutputChannel(const std::string& key, PluginChannelPtr group_channel)
    {
        if(getOutputChannelByKey(key))
        {
            //key must be unique
            return {};
        }

        auto channel = m_plugin_channels->addChannel();
        addDefaultProperties(channel, key);
        m_task->addOutputChannel(channel);
        m_output_channels.push_back(channel);

        if (group_channel)
        {
            channel->setLocalParent(group_channel);
        }
        else
        {
            channel->setLocalParent(getRootChannel());
        }

        return channel;
    }

    PluginChannelPtr SoftwareChannelInstance::addGroupChannel(const std::string& key, PluginChannelPtr group_channel)
    {
        auto channel = addOutputChannel(key, group_channel);
        channel->addProperty("Used", std::make_shared<BooleanProperty>(true));
        channel->setSampleFormat(
            odk::ChannelDataformat::SampleOccurrence::NEVER,
            odk::ChannelDataformat::SampleFormat::NONE,
            0);

        return channel;
    }

    InputChannelPtr SoftwareChannelInstance::getInputChannelProxy(std::uint64_t channel_id)
    {
        for (auto& channel : m_input_channel_proxies)
        {
            if (channel->getChannelId() == channel_id)
            {
                return channel;
            }
        }
        return {};
    }

    std::vector<InputChannelPtr> SoftwareChannelInstance::getInputChannelProxies()
    {
        return m_input_channel_proxies;
    }

    void SoftwareChannelInstance::configureFromTelegram(const UpdateChannelsTelegram& request, std::map<uint32_t, uint32_t>& channel_id_map)
    {
        createChannelsFromTelegram(request, channel_id_map);
        updatePropertiesfromTelegram(request, channel_id_map);
    }

    void SoftwareChannelInstance::createMappingByKey(const UpdateChannelsTelegram& request, std::map<uint32_t, uint32_t>& channel_id_map)
    {
        for (auto &requested_channel : request.m_channels)
        {
            if(const auto output_channel = getOutputChannelByKey(requested_channel.m_channel_config.getProperty(INSTANCE_CHANNEL_KEY)->getStringValue()))
            {
                channel_id_map[requested_channel.m_local_id] = output_channel->getLocalId();
            }
        }
    }

    void SoftwareChannelInstance::createChannelsFromTelegram(const UpdateChannelsTelegram& request, std::map<uint32_t, uint32_t>& channel_id_map)
    {
        for (auto &requested_channel : request.m_channels)
        {
            const std::string key = requested_channel.m_channel_config.getProperty(INSTANCE_CHANNEL_KEY)->getStringValue();
            const auto ch = getOutputChannelByKey(key);
            if(!ch)
            {
                const auto original_parent_id = requested_channel.m_local_parent_id;
                const auto parent_id_it = channel_id_map.find(original_parent_id);
                if (parent_id_it != channel_id_map.end())
                {
                    const auto parent_id = parent_id_it->second;

                    const auto& parent_channel = getOutputChannel(parent_id);
                    auto output_channel = addOutputChannel(key, parent_channel);
                    output_channel->setDefaultName(requested_channel.m_default_name)
                        .setSampleFormat(requested_channel.m_dataformat_info.m_sample_occurrence,
                            requested_channel.m_dataformat_info.m_sample_format,
                            requested_channel.m_dataformat_info.m_sample_dimension);

                    channel_id_map[requested_channel.m_local_id] = output_channel->getLocalId();
                }
                else
                {
                    ODK_ASSERT_FAIL("Unable to determine Parent");
                }
            }
            else
            {
                // rebuild mapping (Arnaud)
                channel_id_map[requested_channel.m_local_id] = ch->getLocalId();
            }
        }
    }

    void SoftwareChannelInstance::updatePropertiesfromTelegram(const UpdateChannelsTelegram& request, const std::map<uint32_t, uint32_t>& channel_id_map)
    {
        for (auto &requested_channel : request.m_channels)
        {
            const auto& local_id = channel_id_map.at(requested_channel.m_local_id);

            if(const auto& output_channel = getOutputChannel(local_id))
            {
                for(const auto& property : requested_channel.m_channel_config.m_properties)
                {
                    const auto property_name(property.getName());
                    if(!output_channel->getProperty(property_name))
                    {
                        output_channel->addProperty(property_name, std::make_shared<RawPropertyHolder>(property));
                    }
                }

                output_channel->updatePropertyTypes();
                updatePropertyTypes(output_channel);
                updateStaticPropertyConstraints(output_channel);

                for (const auto& property : requested_channel.m_channel_config.m_properties)
                {
                    const auto property_name(property.getName());
                    if (auto destination_property = output_channel->getProperty(property_name))
                    {
                        ODK_VERIFY(destination_property->update(property));
                    }
                    else
                    {
                        ODK_ASSERT_FAIL("Unable to configure property %s", property_name.c_str());
                    }
                }
            }
        }
    }

    PluginChannelPtr SoftwareChannelInstance::getOutputChannel(uint32_t local_id) const
    {
        auto match = std::find_if(m_output_channels.begin(), m_output_channels.end(),
                                  [&local_id](const odk::framework::PluginChannelPtr& output_channel)
        {
            return local_id == output_channel->getLocalId();
        });
        if(match != m_output_channels.end())
        {
            return *match;
        }
        return {};
    }

    PluginChannelPtr SoftwareChannelInstance::getOutputChannelByKey(const std::string& key) const
    {
        auto match = std::find_if(m_output_channels.begin(), m_output_channels.end(),
                                  [&key](const odk::framework::PluginChannelPtr& output_channel)
        {
            const auto& key_property = std::dynamic_pointer_cast<EditableStringProperty>(output_channel->getProperty(INSTANCE_CHANNEL_KEY));
            return key == key_property->getValue();
        });
        if(match != m_output_channels.end())
        {
            return *match;
        }
        return {};
    }

    namespace
    {
        void doGetChildrenOfChannel(
            PluginChannelPtr parent_candidate,
            const std::vector<PluginChannelPtr>& all_output_channels,
            bool recursive,
            std::vector<PluginChannelPtr>& children)
        {
            for (const auto a_channel : all_output_channels)
            {
                auto output_parent = a_channel->getLocalParent();
                if (output_parent)
                {
                    if (output_parent->getLocalId() == parent_candidate->getLocalId())
                    {
                        children.push_back(a_channel);
                        if (children.size() > all_output_channels.size())
                        {
                            throw std::domain_error("Cyclic dependency between channels detected");
                        }

                        if (recursive)
                        {
                            doGetChildrenOfChannel(a_channel, all_output_channels, recursive, children);
                        }
                    }
                }
            }
        }
    }

    std::vector<PluginChannelPtr> SoftwareChannelInstance::getChildrenOfChannel(uint32_t local_id, bool recursive) const
    {
        std::vector<PluginChannelPtr> children;
        const auto parent_candidate = getOutputChannel(local_id);
        ODK_ASSERT(parent_candidate);
        if (parent_candidate)
        {
            doGetChildrenOfChannel(parent_candidate, m_output_channels, recursive, children);
        }

        return children;
    }

    odk::IfHost* SoftwareChannelInstance::getHost()
    {
        return m_host;
    }

}
}

