// Copyright DEWETRON GmbH 2020

#pragma once

#include <cstdio>
#include <cstdint>

enum class WavFormatTag
{
    WAV_FORMAT_PCM = 1,
    WAV_FORMAT_FLOAT = 3,
};

/**
 * The WavWriter writes RIFF WAV files from a header and samples
 * The resulting file can be read using any audio processing software given that the sample format is supported
 */
class WavWriter
{
public:
    /**
     * Create a new wav file
     */
    WavWriter(const char* filename);
    /**
     * Adopts a newly created file handle
     */
    WavWriter(FILE*);

    ~WavWriter();

    /**
     * Write/updates the RIFF header
     * @param num_samples number of samples, a stereo sample still counts as one
     */
    void writeHeader(WavFormatTag format, std::size_t bits_per_sample, std::uint32_t sample_rate, std::size_t num_channels, std::size_t num_samples);
    /**
     * Appends a samples to the file, channels must be written interleaved
     * @param value pointer to sample data
     * @param size in bytes, must be a multiple of bits_per_sample/8
     */
    void appendSamples(const void* value, std::size_t size);

    /**
     * Returns the number of samples currently added to the file (counts each sample for every channel)
     **/
    std::size_t samplesWritten() const;

    /**
     * Close the file handle
     */
    void close();

private:
    WavWriter(const WavWriter&);

    FILE* m_file;
    std::size_t m_num_channels;
    std::size_t m_num_samples;
    std::size_t m_sample_size;
    std::size_t m_samples_written;
};
