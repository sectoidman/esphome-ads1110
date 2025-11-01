This is an external package for esphome to add support for the ads1110 ADC chip (as used in e.g. the m5stack ADC I2C Unit v1.1).

Developed using as references the ADS1110 driver from nadavmatalon (https://github.com/nadavmatalon/ADS1110), the ADS1115 driver from realthunder (https://github.com/realthunder/esphome-ads1115) and the TI datasheet (https://www.ti.com/lit/gpn/ads1110).

Sample configuration (using an m5stack ADC I2C Unit v1.1)
```
i2c:
  - id: bus_porta
    scl: GPIO1
    sda: GPIO2

ads1110:
  address: 0x48
  i2c_id: bus_porta

sensor:
  - platform: ads1110
    name: ADC Volts
    update_interval: 1s
    gain: "1"
    rate: "15_SPS_16_BITS"
    filters:
      - offset: -0.000750 # ground the input and set appropriately (using the raw voltage) to get zero
      - multiply: 6.05144 # compensates for voltage divider on m5stack board; you may need to adjust this to account for variation in the resistor values

external_components:
  - source:
      type: git
      url: https://github.com/sectoidman/esphome-ads1110
      ref: master
    components: [ ads1110 ]
```

Verbose logging will output sensor raw counts and raw voltage for debugging and calibration:
```
logger:
  level: VERBOSE

```
