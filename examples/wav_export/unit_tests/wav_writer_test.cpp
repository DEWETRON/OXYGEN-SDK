// Copyright DEWETRON GmbH 2020

#include "wav_writer.h"

//#include "uni_math_constants.h"
#include <cmath>
#include <boost/test/unit_test.hpp>

#if _MSC_VER > 1920
#pragma warning(disable:4996)
#endif

BOOST_AUTO_TEST_SUITE(uni_wav_writer_test_suite)

namespace
{
    const int CHUNK_SIZE = 8;
    const int RIFFWAVE_SIZE = CHUNK_SIZE + 4;
    const int FORMAT_SIZE = CHUNK_SIZE + 16;
}

BOOST_AUTO_TEST_CASE(WriteHeaderTest)
{
    FILE* tmp = std::tmpfile();
    WavWriter writer(tmp);
    writer.writeHeader(WavFormatTag::WAV_FORMAT_PCM, 16, 44100, 2, 0);

    struct header
    {
        char riff[4];
        uint32_t length;
        char wave[4];
    };
    BOOST_CHECK_EQUAL(sizeof(header), RIFFWAVE_SIZE);

    BOOST_REQUIRE_EQUAL(0, fseek(tmp, 0, SEEK_SET));
    header buffer;
    BOOST_REQUIRE_EQUAL(1, fread(&buffer, sizeof(header), 1, tmp));
    BOOST_CHECK(!std::strncmp(buffer.riff, "RIFF", 4));
    BOOST_CHECK_EQUAL(buffer.length, 4 + FORMAT_SIZE + CHUNK_SIZE);
    BOOST_CHECK(!std::strncmp(buffer.wave, "WAVE", 4));

    struct fmtheader
    {
        char fmt[4];
        uint32_t length;
        uint16_t format_tag;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t bytes_per_second;
        uint16_t block_align;
        uint16_t bits_per_sample;
    } fmt;

    BOOST_REQUIRE_EQUAL(1, fread(&fmt, sizeof(fmt), 1, tmp));
    BOOST_CHECK(!std::strncmp(fmt.fmt, "fmt ", 4));
    BOOST_CHECK_EQUAL(fmt.length, 16);
    BOOST_CHECK_EQUAL(fmt.format_tag, 1);
    BOOST_CHECK_EQUAL(fmt.channels, 2);
    BOOST_CHECK_EQUAL(fmt.sample_rate, 44100);
    BOOST_CHECK_EQUAL(fmt.bytes_per_second, 2 * 2 * 44100);
    BOOST_CHECK_EQUAL(fmt.block_align, 2 * 2);
    BOOST_CHECK_EQUAL(fmt.bits_per_sample, 16);

    struct dataheader
    {
        char data[4];
        uint32_t length;
    } dataheader;
    BOOST_REQUIRE_EQUAL(1, fread(&dataheader, sizeof(dataheader), 1, tmp));
    BOOST_CHECK(!std::strncmp(dataheader.data, "data ", 4));
    BOOST_CHECK_EQUAL(dataheader.length, 0);
}

BOOST_AUTO_TEST_CASE(WriteSamplesTest)
{
    FILE* tmp = std::tmpfile();
    WavWriter writer(tmp);
    writer.writeHeader(WavFormatTag::WAV_FORMAT_PCM, 16, 44100, 2, 10);
    for (int16_t n = 0; n < 10; ++n)
    {
        writer.appendSamples(&n, sizeof(int16_t));
    }
    BOOST_CHECK_EQUAL(writer.samplesWritten(), 10);

    // batch add samples
    std::vector<int16_t> data(10, 0);
    writer.appendSamples(data.data(), data.size() * sizeof(int16_t));
    BOOST_CHECK_EQUAL(writer.samplesWritten(), 20);

    BOOST_REQUIRE_EQUAL(0, fseek(tmp, RIFFWAVE_SIZE + FORMAT_SIZE + CHUNK_SIZE, SEEK_SET));
    std::vector<uint16_t> samples(2 * 10);
    BOOST_REQUIRE_EQUAL(samples.size(), fread(samples.data(), sizeof(uint16_t), samples.size(), tmp));
    for (uint16_t n = 0; n < 10; ++n)
    {
        BOOST_CHECK_EQUAL(samples[n], n);
    }
}

#if 0
BOOST_AUTO_TEST_CASE(ExternalUsabiliyTest)
{
    // This test is used to generate a WAV file with various settings to test the import into a 3rd party audio program
    WavWriter writer("C:\\Temp\\export.wav");
    const uint32_t Fs = 44100;
    const double f0 = 440;
    const std::size_t num_samples = Fs * 2;
    const std::size_t bit_depth = 24;

    writer.writeHeader(WavFormatTag::WAV_FORMAT_PCM, bit_depth, Fs, 1, num_samples);
    const double w = uni::math::constants::two_pi * f0 / Fs;
    const double lambda = -2.0 / Fs;
    const double conversion_factor = (1 << (bit_depth - 1)) - 1;
    for (std::size_t n = 0; n < num_samples; ++n)
    {
        double scale = std::exp(lambda * n);
        const double a = scale * std::sin(w * n);
        sint64 sample = static_cast<sint64>(a * conversion_factor);
        writer.appendSamples(&sample, bit_depth / 8);
    }
    writer.close();
}

BOOST_AUTO_TEST_CASE(ExternalUsabiliyFloatTest)
{
    // This test is used to generate a WAV file with various settings to test the import into a 3rd party audio program
    WavWriter writer("C:\\Temp\\export_float.wav");
    const uint32_t Fs = 44100;
    const double f0 = 440;
    const std::size_t num_samples = Fs * 2;
    using sample_format = float;
    const std::size_t bit_depth = sizeof(sample_format) * 8;

    writer.writeHeader(WavFormatTag::WAV_FORMAT_FLOAT, bit_depth, Fs, 1, num_samples);
    const double w = uni::math::constants::two_pi * f0 / Fs;
    const double lambda = -2.0 / Fs;
    for (std::size_t n = 0; n < num_samples; ++n)
    {
        double scale = std::exp(lambda * n);
        const double a = scale * std::sin(w * n);
        sample_format sample = static_cast<sample_format>(a);
        writer.appendSamples(&sample, sizeof(sample));
    }
    writer.close();
}
#endif

BOOST_AUTO_TEST_SUITE_END()
