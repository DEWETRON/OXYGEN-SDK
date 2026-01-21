// Copyright DEWETRON GmbH 2026

#include "odkfw_properties.h"
#include "odkfw_software_channel_plugin.h"
#include "odkapi_utils.h"
#include "odkapi_video.h"

#include <cstddef>
#include <vector>
#include <cstring>

static const char* PLUGIN_MANIFEST =
R"XML(<?xml version="1.0"?>
<OxygenPlugin name="ODK_VIDEO_SOURCE" version="1.0" uuid="DB88DCCD-F05E-4FF3-BF5B-B084F5721866">
  <Info name="Example Plugin: Video Source">
    <Vendor name="DEWETRON GmbH"/>
    <Description>SDK Example plugin that provides a video stream</Description>
    <ReleaseNotes>
      <i>1.0</i> Initial demo with RGB spinner and GRAY spiral image generation
    </ReleaseNotes>
  </Info>
  <Host minimum_version="8.0"/>
</OxygenPlugin>
)XML";

static const char* TRANSLATION_EN =
R"XML(<?xml version="1.0"?>
<TS version="2.1" language="en" sourcelanguage="en">
    <context><name>ConfigKeys</name>
        <message><source>ODK_VIDEO_SOURCE/FrameRate</source><translation>Frames per second</translation></message>
        <message><source>ODK_VIDEO_SOURCE/Width</source><translation>Width</translation></message>
        <message><source>ODK_VIDEO_SOURCE/Height</source><translation>Height</translation></message>
        <message><source>ODK_VIDEO_SOURCE/Color</source><translation>Use colors</translation></message>
    </context>
</TS>
)XML";

// forward declarations to rendering methods
void render_logo_rgb(void* frame, uint32_t width, uint32_t height, float angle);
void render_spiral_y800(void* frame, uint32_t width, uint32_t height, float angle);

/**
 * Define the names for user facing config keys
 */
static const char* KEY_FRAMERATE = "ODK_VIDEO_SOURCE/FrameRate";
static const char* KEY_WIDTH = "ODK_VIDEO_SOURCE/Width";
static const char* KEY_HEIGHT = "ODK_VIDEO_SOURCE/Height";
static const char* KEY_USE_COLORS = "ODK_VIDEO_SOURCE/Color";

/**
 * Define the names for storage related config keys
 */
static const char* KEY_FRAME_HEIGHT = "FrameHeight";
static const char* KEY_FRAME_WIDTH = "FrameWidth";
static const char* KEY_FRAME_FORMAT = "FrameFormat";
static const char* KEY_SAMPLE_RATE = "SampleRate";
static const char* KEY_VIDEO_COMPRESSION = "Neon/VideoCompression";

static double TIMEBASE_FREQUENCY = 1000000;

static const char* FOURCC_Y800 = "Y800";
static const char* FOURCC_RGB = "RGB\0";

namespace // utility functions/structs
{
    struct Color { float r, g, b; };

    uint32_t make_multiple_of_8(uint32_t value)
    {
        return (value + 7) & ~7;
    }
}

class VideoSourceChannelInstance : public odk::framework::SoftwareChannelInstance
{
public:
    VideoSourceChannelInstance()
        : m_framerate(std::make_shared<odk::framework::EditableScalarProperty>(30, "Hz"))
        , m_width(std::make_shared<odk::framework::EditableUnsignedProperty>(320))
        , m_height(std::make_shared<odk::framework::EditableUnsignedProperty>(240))
        , m_use_color(std::make_shared<odk::framework::EditableBooleanProperty>(true))
        , m_frame_width(std::make_shared<odk::framework::EditableScalarProperty>(320, "pixel"))
        , m_frame_height(std::make_shared<odk::framework::EditableScalarProperty>(240, "pixel"))
        , m_frame_format(std::make_shared<odk::framework::StringProperty>(FOURCC_Y800))
        , m_sample_rate(std::make_shared<odk::framework::EditableScalarProperty>(0, "Hz"))
        , m_compression(std::make_shared<odk::framework::StringProperty>("video/x-h264"))
        , m_current_frame(0)
    {
        m_framerate->setMinMaxConstraint(1, 120);
        m_width->setMinMaxConstraint(8, 1920);
        m_height->setMinMaxConstraint(8, 1080);
    }

    /**
     * Return the information that is used to display the software channel in the calculation list in the GUI
     */
    static odk::RegisterSoftwareChannel getSoftwareChannelInfo()
    {
        odk::RegisterSoftwareChannel telegram;
        telegram.m_display_name = "Example Plugin: Video Source";
        telegram.m_service_name = "VideoSource";
        telegram.m_display_group = "Data Input";
        telegram.m_description = "Provides an image stream of generated images";
        return telegram;
    }

    InitResult init(const InitParams&) final
    {
        return { true };
    }

    void initTimebases(odk::IfHost*) final
    {
        // Use a fixed 1us timebase
        m_output_channels[0]->setSimpleTimebase(TIMEBASE_FREQUENCY);
    }

    /**
     * This method is called when the configuration changes. Evaluate the input channels and update the output channel and return if the configuration is valid
     */
    bool update() final
    {
        // read user settings
        const double frame_rate = m_framerate->getValue().m_val;
        const bool use_color = m_use_color->getValue();
        uint32_t width = m_width->getValue();
        uint32_t height = m_height->getValue();

        // verify settings
        if (width == 0 || height == 0)
        {
            return false;
        }

        // Compressed video must have dimensions that are divisible by 8
        width = make_multiple_of_8(width);
        height = make_multiple_of_8(height);

        m_width->setValue(width);
        m_height->setValue(height);

        // sync storage format definition config items with user facing settings
        m_frame_width->setValue(odk::Scalar(width, "pixel"));
        m_frame_height->setValue(odk::Scalar(height, "pixel"));
        m_frame_format->setValue(use_color ? FOURCC_RGB : FOURCC_Y800);
        m_sample_rate->setValue(odk::Scalar(frame_rate, "Hz"));

        return true;
    }

    /**
     * Initialize the configuration and format of this calculation
     */
    void create(odk::IfHost* host) override
    {
        ODK_UNUSED(host);

        getRootChannel()->setDefaultName("VideoChannel")
            .setSampleFormat( // Video images need to by ASYNC with a dynamic size (dimension = 0)
                odk::ChannelDataformat::SampleOccurrence::ASYNC,
                odk::ChannelDataformat::SampleFormat::VIDEO_RAW_FRAME,
                0)
            .setDeletable(true)
            .addProperty(KEY_FRAMERATE, m_framerate)
            .addProperty(KEY_WIDTH, m_width)
            .addProperty(KEY_HEIGHT, m_height)
            .addProperty(KEY_USE_COLORS, m_use_color)
            .addProperty(KEY_FRAME_WIDTH, m_frame_width)
            .addProperty(KEY_FRAME_HEIGHT, m_frame_height)
            .addProperty(KEY_FRAME_FORMAT, m_frame_format)
            .addProperty(KEY_SAMPLE_RATE, m_sample_rate)
            .addProperty(KEY_VIDEO_COMPRESSION, m_compression)
            ;
    }

    bool configure(
        const odk::UpdateChannelsTelegram& request,
        std::map<std::uint32_t, std::uint32_t>& channel_id_map) final
    {
        configureFromTelegram(request, channel_id_map);
        return true;
    }

    void prepareProcessing(odk::IfHost*) final
    {
        const uint32_t width = m_width->getValue();
        const uint32_t height = m_height->getValue();
        const bool use_color = m_use_color->getValue();

        const size_t bytes_per_pixel = use_color ? 3 : 1;
        m_frame_buffer.resize(bytes_per_pixel * width * height + sizeof(odk::VideoFrameHeader), std::byte(0));
        m_current_frame = 0;
    }

    void stopProcessing(odk::IfHost*) final
    {
        // release all processing related resources
        m_frame_buffer.clear();
        m_frame_buffer.shrink_to_fit();
    }

    void process(ProcessingContext& context, odk::IfHost* host) final
    {
        auto out_channel = getRootChannel();

        if (!out_channel->getUsedProperty()->getValue())
        {
            // Do not output samples when the channel is not used
            return;
        }

        const double frame_rate = m_framerate->getValue().m_val;
        const uint32_t width = m_width->getValue();
        const uint32_t height = m_height->getValue();
        const bool use_color = m_use_color->getValue();

        // generate frames up to the current sample time
        size_t target_frame = odk::convertTimestampToTick(context.m_master_timestamp, frame_rate);

        if (m_current_frame >= target_frame)
        {
            return;
        }

        // write header (common for all frames)
        odk::VideoFrameHeader* const header = reinterpret_cast<odk::VideoFrameHeader*>(m_frame_buffer.data());

        // defined the used color format using the FourCC code
        std::memcpy(header->fourcc, use_color ? FOURCC_RGB : FOURCC_Y800, 4);
        header->width = width;
        header->height = height;

        // generate all frames up to the target_frame
        void* const image = m_frame_buffer.data() + sizeof(odk::VideoFrameHeader);
        for (; m_current_frame < target_frame; ++m_current_frame)
        {
            float time_in_seconds = static_cast<float>(m_current_frame / frame_rate);
            if (use_color)
            {
                render_logo_rgb(image, width, height, time_in_seconds);
            }
            else
            {
                render_spiral_y800(image, width, height, time_in_seconds);
            }

            // add a single, time stamped video frame to the channel
            uint64_t timestamp = static_cast<uint64_t>(time_in_seconds * TIMEBASE_FREQUENCY);
            addSample(host, out_channel->getLocalId(), timestamp, m_frame_buffer.data(), m_frame_buffer.size());
        }
    }

private:
    std::shared_ptr<odk::framework::EditableScalarProperty> m_framerate;
    std::shared_ptr<odk::framework::EditableUnsignedProperty> m_width;
    std::shared_ptr<odk::framework::EditableUnsignedProperty> m_height;
    std::shared_ptr<odk::framework::EditableBooleanProperty> m_use_color;

    // config items for storage support
    std::shared_ptr<odk::framework::EditableScalarProperty> m_frame_width;
    std::shared_ptr<odk::framework::EditableScalarProperty> m_frame_height;
    std::shared_ptr<odk::framework::StringProperty> m_frame_format;
    std::shared_ptr<odk::framework::EditableScalarProperty> m_sample_rate;
    std::shared_ptr<odk::framework::StringProperty> m_compression;

    size_t m_current_frame;
    std::vector<std::byte> m_frame_buffer;
};

class VideoSourcePlugin : public odk::framework::SoftwareChannelPlugin<VideoSourceChannelInstance>
{
public:
    void registerTranslations() final
    {
        addTranslation(TRANSLATION_EN);
    }
};

OXY_REGISTER_PLUGIN1("ODK_VIDEO_SOURCE", PLUGIN_MANIFEST, VideoSourcePlugin);
