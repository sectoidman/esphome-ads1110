import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    CONF_RATE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_ID,
)
from .. import ads1110_ns, ADS1110Component, CONF_ADS1110_ID

AUTO_LOAD = ["voltage_sampler"]
DEPENDENCIES = ["ads1110"]


ADS1110Gain = ads1110_ns.enum("ADS1110Gain")
GAIN = {
    "1": ADS1110Gain.ADS1110_GAIN_1,
    "2": ADS1110Gain.ADS1110_GAIN_2,
    "4": ADS1110Gain.ADS1110_GAIN_4,
    "8": ADS1110Gain.ADS1110_GAIN_8,
}

ADS1110Rate = ads1110_ns.enum("ADS1110Rate")
RATE = {
    "15_SPS_16_BITS": ADS1110Rate.ADS1110_15SPS_16B,
    "30_SPS_15_BITS": ADS1110Rate.ADS1110_30SPS_15B,
    "60_SPS_14_BITS": ADS1110Rate.ADS1110_60SPS_14B,
    "240_SPS_12_BITS": ADS1110Rate.ADS1110_240SPS_12B,
}


ADS1110Sensor = ads1110_ns.class_(
    "ADS1110Sensor", sensor.Sensor, cg.PollingComponent, voltage_sampler.VoltageSampler
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        ADS1110Sensor,
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(CONF_ADS1110_ID): cv.use_id(ADS1110Component),
            cv.Optional(CONF_GAIN, default="1"): cv.enum(GAIN, string=True),
            cv.Optional(CONF_RATE, default="15_SPS_16_BITS"): cv.enum(
                RATE, upper=True, space="_"
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS1110_ID])

    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_rate(config[CONF_RATE]))
