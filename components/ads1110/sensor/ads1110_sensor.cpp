#include "ads1110_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ads1110 {

static const char *const TAG = "ads1110.sensor";

static const char* const GAIN_CFG_STR[] = {"1x", "2x", "4x", "8x"};
static const char* const RATE_CFG_STR[] = {"240Hz", "60Hz", "30Hz", "15Hz"};

float ADS1110Sensor::sample() {
  return this->parent_->request_measurement(this->gain_, this->rate_);
}

void ADS1110Sensor::update() {
  float v = this->sample();
  if (!std::isnan(v)) {
    ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    this->publish_state(v);
  }
}

void ADS1110Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS1110 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %s", GAIN_CFG_STR[this->gain_]);
  ESP_LOGCONFIG(TAG, "    Rate: %s", RATE_CFG_STR[this->rate_]);
}

}  // namespace ads1110
}  // namespace esphome
