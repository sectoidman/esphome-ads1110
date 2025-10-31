#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"

#include "../ads1110.h"

namespace esphome {
namespace ads1110 {

    /// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
    class ADS1110Sensor : public sensor::Sensor,
    public PollingComponent,
    public voltage_sampler::VoltageSampler,
    public Parented<ADS1110Component> {
        public:
            void update() override;
            void set_gain(ADS1110Gain gain) { this->gain_ = gain; }
            void set_rate(ADS1110Rate rate) { this->rate_ = rate; }
            float sample() override;

            void dump_config() override;

        protected:
            ADS1110Gain gain_;
            ADS1110Rate rate_;
    };

}  // namespace ads1110
}  // namespace esphome
