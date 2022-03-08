// Copyright DEWETRON GmbH 2022

#pragma once

#include "odkbase_if_host_fwd.h"
#include "odkuni_defines.h"
#include <vector>

namespace odk
{
    namespace framework
    {
        /**
         * The Resampler allows to use samples from a source with an unstable sampling frequency but precise timestamps
         * It allows to add samples to an output channel while estimating the underlying real sample rate
         * and resampling data to match the nominal sample rate
         */
        class Resampler
        {
        public:
            /**
             * Create a resampler and set the desired output rate (nominal sample rate)
             */
            Resampler(double nominal_sample_rate = 1);

            void setNominalSampleRate(double rate);
            ODK_NODISCARD double getNominalSampleRate() const { return m_nomianal_sample_rate; }

            /**
             * Return the timestamp of the last call to addSamples
             */
            ODK_NODISCARD double getLastTimestamp() const { return m_last_timestamp; }
            /**
             * Return the number of samples already sent to the output channel
             */
            ODK_NODISCARD std::size_t getSampleCount() const { return m_actual_scnt; }

            /**
             * Reset all channel related data
             */
            void reset();

            /**
             * Add samples to the output channel <local_channel_id> but resample all <data> samples to match the nominal sample rate
             * After resampling, the data is sent to the host with odk::host_msg::ADD_CONTIGUOUS_SAMPLES using a computed timestamp in the nominal rate
             * 
             * @param last_sample_timestamp exact time in seconds since acquisition start of the last sample (data[num_samples-1])
             * @param data pointer to the first sample
             * @param num_samples number of samples in data
             * @return result of host->messageSyncData
             */
            std::uint64_t addSamples(odk::IfHost* host, std::uint32_t local_channel_id, double last_sample_timestamp, const double* data, std::size_t num_samples);

        protected:
            double m_nomianal_sample_rate;
            double m_last_timestamp;
            std::size_t m_actual_scnt;
            std::vector<double> m_input_buffer;
            std::vector<double> m_output_buffer;
        };
    }
}
