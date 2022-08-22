// Copyright DEWETRON GmbH 2022

#include "odkfw_resampler.h"
#include "odkapi_error_codes.h"
#include "odkapi_message_ids.h"
#include "odkbase_if_host.h"
#include "odkuni_assert.h"

#include <cmath>
#include <limits>

using odk::framework::Resampler;

namespace
{
    /**
     * Linear Interpolation function
     */
    inline double lerp(double a, double b, double t)
    {
        return a + (b - a) * t;
    }

    class InputVectorLabels
    {
    public:
        InputVectorLabels(double start, double end, std::size_t size)
            : m_start(start)
            //, m_end(end)
            //, m_size(static_cast<double>(size))
            , m_scale(static_cast<double>(size) / (end - start)) // precompute the index scaling
        {
        }

        inline double indexOfTime(double time) const
        {
            //return m_size * (time - m_start) / (m_end - m_start);
            return m_scale * (time - m_start);
        }

    private:
        double m_start;
        //double m_end;
        //double m_size;
        double m_scale;
    };

    /**
     * computes linearly interpolated output samples and pushes them to the vector
     */
    std::size_t interp(std::vector<double>& output, std::uint64_t x_start, double x_samplerate, std::size_t num, const InputVectorLabels& fp_time, const std::vector<double>& fp)
    {
        for (std::size_t n = 0; n < num; ++n)
        {
            double val = (x_start + n) / x_samplerate;
            double pos = fp_time.indexOfTime(val);

            const int idx = static_cast<int>(std::floor(pos));
            const double t = pos - static_cast<double>(idx);

            ODK_ASSERT_GTE(idx, 0);
            if (static_cast<std::size_t>(idx) + 1 >= fp.size())
            {
                // element fp[idx + 1] is inaccessible, early return
                return n;
            }

            double val_interp = lerp(fp[idx], fp[idx + 1], t);
            output.push_back(val_interp);
        }
        return num;
    }
}

Resampler::Resampler(double nominal_rate)
    : m_nomianal_sample_rate(nominal_rate)
    , m_last_timestamp(0)
    , m_actual_scnt(0)
{
}

void Resampler::setNominalSampleRate(double rate)
{
    m_nomianal_sample_rate = rate;
}

void Resampler::reset()
{
    m_last_timestamp = 0;
    m_actual_scnt = 0;
    m_input_buffer.clear();
    m_output_buffer.clear();
}

std::uint64_t Resampler::addSamples(odk::IfHost* host, std::uint32_t local_channel_id, double last_sample_timestamp, const double* data, std::size_t num_samples)
{
    if (num_samples == 0)
    {
        return odk::error_codes::OK;
    }

    const double last_sample = last_sample_timestamp * m_nomianal_sample_rate;
    const std::uint64_t last_sample_int = static_cast<std::uint64_t>(std::floor(last_sample));

    double first_sample_timestamp = m_last_timestamp;
    if (!m_input_buffer.empty())
    {
        double estimated_sample_duration = (last_sample_timestamp - m_last_timestamp) / num_samples;
        first_sample_timestamp -= m_input_buffer.size() * estimated_sample_duration;
        ODK_ASSERT_GTE(first_sample_timestamp, 0);
    }
    m_input_buffer.insert(m_input_buffer.end(), data, data + num_samples);

    InputVectorLabels input_desc(first_sample_timestamp, last_sample_timestamp, m_input_buffer.size());

    std::uint64_t result = odk::error_codes::OK;
    if (last_sample_int > m_actual_scnt)
    {
        const std::size_t num = static_cast<std::size_t>(last_sample_int) - m_actual_scnt; // the maximum possible number of sample to compute

        // Prepare the output buffer which has a uint64 timestamp followed by up to <num> double values
        m_output_buffer.resize(1);

        // Store the timestamp of the first output sample
        static_assert(sizeof(std::uint64_t) == sizeof(double), "The following code only works when double and uint64 have the same size");
        *reinterpret_cast<std::uint64_t*>(m_output_buffer.data()) = m_actual_scnt;

        // write up to <num> interpolated samples to m_output_buffer
        std::size_t num_written = interp(m_output_buffer,
            m_actual_scnt, m_nomianal_sample_rate, num, // this iterates over real output timestamps in ticks
            input_desc, // Timestamps of input samples
            m_input_buffer); // actual input samples

        result = host->messageSyncData(odk::host_msg::ADD_CONTIGUOUS_SAMPLES, local_channel_id, m_output_buffer.data(), m_output_buffer.size() * sizeof(double), nullptr);
        m_actual_scnt += num_written;
    }

    // Remember old samples
    m_last_timestamp = last_sample_timestamp;
    m_input_buffer.assign(data, data + num_samples);

    return result;
}
