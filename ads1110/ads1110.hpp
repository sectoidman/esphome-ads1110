#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

#include <vector>

namespace esphome {
namespace ads1110 {

    enum ADS1110Gain {
        ADS1110_GAIN_1 = 0b00, // default
        ADS1110_GAIN_2 = 0b01,
        ADS1110_GAIN_4 = 0b10,
        ADS1110_GAIN_8 = 0b11,
    };

    // per the datasheet, the resolution varies depending on the rate setting:
    // 15SPS  = 16-bit resolution (32,768 to -32,767)
    // 30SPS  = 15-bit resolution (16,384 to -16,383)
    // 60SPS  = 14-bit resolution (8192 to -8191)
    // 240SPS = 12-bit resolution (-2048 to 2047)
    enum ADS1110Rate {
        ADS1110_15SPS_16B  = 0b11, // default
        ADS1110_30SPS_15B  = 0b10,
        ADS1110_60SPS_14B  = 0b01,
        ADS1110_240SPS_12B = 0b00,
    };

    class ADS1110Component : public Component, public i2c::I2CDevice {
        public:
            void setup() override;
            void dump_config() override;
            /// HARDWARE_LATE setup priority
            float get_setup_priority() const override { return setup_priority::DATA; }
            void set_continuous_mode(bool continuous_mode) { continuous_mode_ = continuous_mode; }

            /// Helper method to request a measurement from a sensor.
            float request_measurement(ADS1110Gain gain, ADS1110Rate rate);

        protected:
            float _request_measurement(ADS1110Gain gain, ADS1110Rate rate);

            uint8_t prev_config_{0};
            bool continuous_mode_;
            SemaphoreHandle_t semaphore_{0};
    };

}  // namespace ads1110
}  // namespace esphome
