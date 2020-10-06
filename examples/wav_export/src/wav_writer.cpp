// Copyright DEWETRON GmbH 2020

#include "wav_writer.h"
#include "odkuni_assert.h"
#include <boost/static_assert.hpp>
#include <cstring>
#include <ios>
#include <stdexcept>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if _MSC_VER > 1920
#pragma warning(disable:4996)
#endif
#endif

namespace
{
    struct Chunk
    {
        Chunk(const char* id, std::size_t size)
            : size(static_cast<std::uint32_t>(size))
        {
            BOOST_STATIC_ASSERT(sizeof(*this) == 8);
            std::strncpy(this->id, id, sizeof(this->id));
        }
        char id[4];
        std::uint32_t size;
    };

    struct RiffWaveHeader : public Chunk
    {
        RiffWaveHeader(std::size_t size)
            : Chunk("RIFF", size + sizeof(this->wave))
        {
            BOOST_STATIC_ASSERT(sizeof(*this) == 12);
            std::strncpy(this->wave, "WAVE", sizeof(this->wave));
        }
        char wave[4];
    };

    struct FormatHeader : public Chunk
    {
        FormatHeader()
            : Chunk("fmt ", sizeof(*this) - 8)
        {
            BOOST_STATIC_ASSERT(sizeof(*this) == (16 + 8));
        }

        std::uint16_t format_tag;
        std::uint16_t channels;
        std::uint32_t sample_rate;
        std::uint32_t bytes_per_second;
        std::uint16_t block_align;
        std::uint16_t bits_per_sample;
    };

#ifdef _MSC_VER
    std::wstring utf8ToUtf32(const std::string& src)
    {
        std::wstring r;
        if (!src.empty())
        {
            int rc = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), static_cast<int>(src.length()), 0, 0);
            if (rc)
            {
                wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t) * rc);
                if (buf)
                {
                    rc = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), static_cast<int>(src.length()), buf, rc);
                    if (rc)
                    {
                        r.assign(buf, buf + rc);
                    }
                    free(buf);
                }
            }
        }
        return r;
    }
#endif

    FILE* openFileUtf8(const char* filename)
    {
#ifdef _MSC_VER
        auto w_filename = utf8ToUtf32(filename);
        return _wfopen(w_filename.c_str(), L"wb+");
#else
        return fopen(filename, "wb+");
#endif
    }
}

WavWriter::WavWriter(const char* filename)
    : m_file(NULL)
    , m_num_channels(0)
    , m_num_samples(0)
    , m_sample_size(0)
    , m_samples_written(0)
{
    m_file = openFileUtf8(filename);
}

WavWriter::WavWriter(FILE* file)
    : m_file(file)
    , m_num_channels(0)
    , m_num_samples(0)
    , m_sample_size(0)
    , m_samples_written(0)
{
}

WavWriter::~WavWriter()
{
    close();
}

void WavWriter::close()
{
    if (m_file)
    {
        fclose(m_file);
        m_file = NULL;
    }
}

void WavWriter::writeHeader(WavFormatTag format, std::size_t bits_per_sample, std::uint32_t sample_rate, std::size_t num_channels, std::size_t num_samples)
{
    if (bits_per_sample % 8 != 0)
    {
        throw std::domain_error("WavWrite supports only 8-bit quantized sample sizes");
    }

    if (NULL == m_file)
    {
        throw std::runtime_error("Unable to write to file");
    }

    if (0 != fseek(m_file, 0, SEEK_SET))
    {
        throw std::runtime_error("Unable to seek in file");
    }

    FormatHeader formatheader;
    formatheader.format_tag = static_cast<std::uint16_t>(format);
    formatheader.channels = static_cast<std::uint16_t>(num_channels);
    formatheader.sample_rate = static_cast<std::uint32_t>(sample_rate);
    formatheader.bits_per_sample = static_cast<std::uint16_t>(bits_per_sample);
    formatheader.block_align = static_cast<std::uint16_t>(num_channels * ((bits_per_sample + 7) / 8));
    formatheader.bytes_per_second = static_cast<std::uint32_t>(sample_rate * formatheader.block_align);

    Chunk dataheader("data", formatheader.block_align * num_samples);

    const std::size_t filesize = sizeof(formatheader) + sizeof(dataheader) + dataheader.size;
    RiffWaveHeader riff(filesize);
    if (1 != fwrite(&riff, sizeof(riff), 1, m_file))
    {
        throw std::ios_base::failure("Unable to write RIFF header");
    }

    if (1 != fwrite(&formatheader, sizeof(formatheader), 1, m_file))
    {
        throw std::ios_base::failure("Unable to write fmt header");
    }

    if (1 != fwrite(&dataheader, sizeof(dataheader), 1, m_file))
    {
        throw std::ios_base::failure("Unable to write data header");
    }

    m_num_channels = num_channels;
    m_num_samples = num_samples;
    m_sample_size = bits_per_sample / 8;
    m_samples_written = 0;
}

void WavWriter::appendSamples(const void* value, std::size_t size)
{
    ODK_ASSERT(size % m_sample_size == 0);
    ODK_ASSERT(m_samples_written + size/m_sample_size <= m_num_channels * m_num_samples);

    if (1 != fwrite(value, size, 1, m_file))
    {
        throw std::ios_base::failure("Unable to write samples");
    }

    m_samples_written += size / m_sample_size;
}

std::size_t WavWriter::samplesWritten() const
{
    return m_samples_written;
}
