// Copyright DEWETRON GmbH 2017

#include "odkfw_data_requester.h"
#include "odkapi_oxygen_queries.h"
#include "odkapi_channel_dataformat_xml.h"

namespace odk
{
namespace framework
{
    uint64_t DataRequestIDManager::m_next_id = 0;

    DataRequester::DataRequester(IfHost *host, uint64_t channel_id, bool user_reduced)
        : m_host(host)
        , m_current_position(-1)
        , m_channel_id(channel_id)
        , m_is_single_value(false)
        , m_user_reduced(user_reduced)
    {
        setupDataRequest();
    }

    DataRequester::~DataRequester()
    {
        auto msg = m_host->createValue<odk::IfUIntValue>();
        msg->set(m_dataset_descriptor.m_id);
        m_host->messageSync(odk::host_msg::DATA_GROUP_REMOVE, 0, msg.get(), nullptr);
    }

    void DataRequester::setupDataRequest()
    {
        PluginDataSet request;
        request.m_id = DataRequestIDManager::getNextID();
        request.m_data_set_type = DataSetType::SCALED;
        request.m_data_mode = m_user_reduced ? DataSetMode::REDUCED : DataSetMode::NORMAL;
        request.m_policy = StreamPolicy::EXACT;
        request.m_channels.push_back(m_channel_id);

        auto xml_msg = m_host->createValue<odk::IfXMLValue>();
        if (!xml_msg)
        {
            return;
        }

        // create XML request
        const auto xml_content = request.generate();
        xml_msg->set(xml_content.c_str());

        odk::MessageReturnValueHolder<odk::IfXMLValue> group_add_result;
        m_host->messageSync(odk::host_msg::DATA_GROUP_ADD, 0, xml_msg.get(), group_add_result.data());

        if (group_add_result.valid())
        {
            m_dataset_descriptor = DataSetDescriptor();
            m_dataset_descriptor.parse(group_add_result->asStringView());
        }

        if (m_dataset_descriptor.m_stream_descriptors.size() == 1)
        {
            m_stream_reader.setStreamDescriptor(m_dataset_descriptor.m_stream_descriptors.at(0));
        }

        std::string channel_context = odk::queries::OxygenChannels + ("#" + std::to_string(m_channel_id));
        auto data_format_xml = m_host->getValue<IfXMLValue>(channel_context.c_str(), "DataFormat");
        odk::ChannelDataformat dataformat;
        if (dataformat.parse(data_format_xml->asStringView()))
        {
            m_is_single_value = dataformat.m_sample_occurrence == odk::ChannelDataformat::SampleOccurrence::SINGLE_VALUE;
        }
    }

    void DataRequester::fetchMoreData()
    {
        m_stream_reader.clearBlocks();
        m_data_block_list.reset();

        while(m_current_position != m_end_position
              && (!m_data_block_list || m_data_block_list->getBlockCount() == 0))
        {
            double next_position = std::min(m_current_position + BLOCK_LENGTH, m_end_position);

            if (auto xml_msg = m_host->createValue<odk::IfXMLValue>())
            {
                if (m_is_single_value)
                {
                    PluginDataRequest req(m_dataset_descriptor.m_id, odk::PluginDataRequest::SingleValue(std::numeric_limits<double>::max()));
                    xml_msg->set(req.generate().c_str());
                }
                else
                {
                    PluginDataRequest req(m_dataset_descriptor.m_id, PluginDataRequest::DataWindow(m_current_position, next_position));
                    xml_msg->set(req.generate().c_str());
                }

                const odk::IfValue* response = nullptr;
                if (0 != m_host->messageSync(odk::host_msg::DATA_READ, 0, xml_msg.get(), &response))
                {
                    return;
                }

                m_data_block_list = odk::ptr(odk::value_cast<odk::IfDataBlockList>(response));
                m_current_position = next_position;
            }

            const auto block_count = m_data_block_list->getBlockCount();

            auto block_list_descriptor_xml = odk::ptr(m_data_block_list->getBlockListDescription());
            BlockListDescriptor list_descriptor;
            list_descriptor.parse(block_list_descriptor_xml->asStringView());

            odk::framework::StreamReader stream_reader;

            for (int i = 0; i < block_count; ++i)
            {
                auto block = odk::ptr(m_data_block_list->getBlock(i));
                auto block_descriptor_xml = odk::ptr(block->getBlockDescription());

                BlockDescriptor block_descriptor;
                block_descriptor.parse(block_descriptor_xml->asStringView());

                if (!block_descriptor.m_block_channels.empty())
                {
                    m_stream_reader.addDataBlock(std::move(block_descriptor), block->data());
                }
            }

            std::shared_ptr<odk::DataRegions> data_regions;
            {
                auto xml_msg = m_host->createValue<odk::IfXMLValue>();
                if (xml_msg)
                {
                    PluginDataRegionsRequest req(m_dataset_descriptor.m_id);
                    req.m_data_window = PluginDataRegionsRequest::DataWindow(m_current_position, next_position);
                    xml_msg->set(req.generate().c_str());
                }

                const odk::IfValue* data_regions_result = nullptr;
                m_host->messageSync(odk::host_msg::DATA_REGIONS_READ, 0, xml_msg.get(), &data_regions_result);

                const odk::IfXMLValue* data_regions_result_xml = odk::value_cast<odk::IfXMLValue>(data_regions_result);
                if (data_regions_result)
                {
                    if (data_regions_result_xml)
                    {
                        data_regions = std::make_shared<DataRegions>();
                        data_regions->parse(data_regions_result_xml->asStringView());
                    }
                    data_regions_result->release();
                }
            }

            if(data_regions)
            {
                for(const auto& invalid_region : data_regions->m_data_regions)
                {
                    stream_reader.addDataRegion(invalid_region);
                }
            }
        }
    }

    void DataRequester::updateStreamIterator(StreamIterator* iterator)
    {
        iterator->clearRanges();
        if (m_current_position != m_end_position)
        {
            fetchMoreData();
            m_stream_reader.updateStreamIterator(m_channel_id, *iterator, odk::Interval<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max()));
        }
    }

    std::shared_ptr<StreamIterator> DataRequester::getIterator(double start, double end)
    {
        m_current_position = start;
        m_end_position = end;
        fetchMoreData();
        if(!m_iterator)
        {
            m_iterator = std::make_shared<StreamIterator>(m_stream_reader.createChannelIterator(m_channel_id));
            m_iterator->setDataRequester(this);
        }
        return m_iterator;
    }

}
}
