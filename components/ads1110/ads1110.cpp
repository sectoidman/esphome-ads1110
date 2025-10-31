#include "ads1110.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <cstring>
#include <cstdint>

namespace esphome {
namespace ads1110 {

static const char *const TAG = "ads1110";

void ADS1110Component::setup()
{
    ESP_LOGCONFIG(TAG, "Setting up ADS1110...");

    // the ads1110 doesn't let you address individual
    // registers for reading, you can read either
    // 2 (just output) or 3 bytes (output + config)
    uint8_t value[3];
    if (!this->read(value, 3))
    {
        this->mark_failed();
        return;
    }

    this->semaphore_ = xSemaphoreCreateBinary();
    if (!this->semaphore_) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        this->mark_failed();
        return;
    }
    xSemaphoreGive(this->semaphore_);

    ESP_LOGCONFIG(TAG, "Configuring ADS1110...");
}


float ADS1110Component::request_measurement(ADS1110Gain gain, ADS1110Rate rate)
{
  xSemaphoreTake(this->semaphore_, portMAX_DELAY);
  float res = _request_measurement(gain, rate);
  xSemaphoreGive(this->semaphore_);
  return res;
}


void ADS1110Component::dump_config()
{
  ESP_LOGCONFIG(TAG, "ADS1110:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed())
  {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
}


float ADS1110Component::_request_measurement(ADS1110Gain gain,
                                             ADS1110Rate rate)
{
    uint8_t config = this->prev_config_;
    // Gain
    //        0bxxxxxxBB
    config &= 0b11111100;
    config |= (gain & 0b11);

    // Data Rate
    //        0bxxxxBBxx
    config &= 0b11110011;
    config |= (rate & 0b11) << 2;

    if (!this->continuous_mode_)
    {
        // Start conversion (set DRDY)
        config |= 1 << 7;
    }
    else
    {
        // set continuous mode (SC) bit
        config |= 1 << 4;
    }

    uint8_t raw_buffer[3] = {0, 0, 0};
    uint32_t start = millis();

    if (!this->continuous_mode_ || this->prev_config_ != config)
    {
        if (!this->write(&config, 1))
        {
            this->status_set_warning();
            return NAN;
        }
        this->prev_config_ = config;

        // keep reading until we get a fresh sample (DRDY = 0)
        while (this->read(raw_buffer, 3) && (raw_buffer[2] & 0x80) != 0)
        {
            if (millis() - start > 100)
            {
                ESP_LOGW(TAG, "Reading ADS1110 timed out");
                this->status_set_warning();
                return NAN;
            }
        }
    }

    int16_t output_signed;
    memcpy(&output_signed, raw_buffer, sizeof(output_signed));

    // Output code = -1 * Min Code * PGA * ((Vin+ - Vin-) / 2.048V)
    float volts;
    int16_t min_code;
    float gain_factor;

    switch (rate)
    {
        case ADS1110_15SPS_16B:
            min_code = INT16_MIN;
            break;
        case ADS1110_30SPS_15B:
            min_code = -16384;
            break;
        case ADS1110_60SPS_14B:
            min_code = -8192;
            break;
        case ADS1110_240SPS_12B:
            min_code = -2048;
            break;
        default:
            this->status_set_warning();
            return NAN;

    }

    switch (gain)
    {
        case ADS1110_GAIN_1:
            gain_factor = 1.0f;
            break;
        case ADS1110_GAIN_2:
            gain_factor = 2.0f;
            break;
        case ADS1110_GAIN_4:
            gain_factor = 4.0f;
            break;
        case ADS1110_GAIN_8:
            gain_factor = 8.0f;
            break;
        default:
            this->status_set_warning();
            return NAN;
    }

    volts = ((-2.048f * output_signed) / (min_code * gain_factor));

    uint32_t t = millis();
    ESP_LOGI(TAG, "%p: sample time %lu, %lu, %g, %d", this, t, t - start, volts, output_signed);

    this->status_clear_warning();
    return volts;
}

} // ads1110
} // esphome
