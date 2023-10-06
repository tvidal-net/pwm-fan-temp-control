# pwm-fan-temp-control

Arduino pwm-fan based temperature control, using one-wire DS18B20 temperature sensor.

## Source Code

* [main.cpp](src/main.cpp)

## Project Configuration

* [platformio.ini](platformio.ini)

## Usage

compile / build
```shell script
PLATFORMIO_BUILD_FLAGS='-DDEBUG' platformio run -e 'nano'
```

upload with DEBUG enabled
```shell script
PLATFORMIO_BUILD_FLAGS='-DDEBUG' platformio run -e 'nano' --target='upload'
```

upload final version
```shell script
platformio run -e 'nano' --target='upload'
```

monitor serial port
```shell script
platformio device monitor -e 'nano'
```

## Dependencies

* [mathertel/OneButton](https://registry.platformio.org/libraries/mathertel/OneButton)
* [kiryanenko/SimpleTimer](https://registry.platformio.org/libraries/kiryanenko/SimpleTimer)
* [paulstoffregen/OneWire](https://registry.platformio.org/libraries/paulstoffregen/OneWire)
* [milesburton/DallasTemperature](https://registry.platformio.org/libraries/milesburton/DallasTemperature)

## References

* [Watchdog timer in Arduino](https://www.tutorialspoint.com/watchdog-timer-in-arduino)
* [How to properly control PWM fans with Arduino](https://fdossena.com/?p=ArduinoFanControl/i.md)
* [High-Power Control: Arduino + N-Channel MOSFET](http://adam-meyer.com/arduino/rfp30n06le-arduino)
* [Guide for DS18B20 Temperature Sensor with Arduino](https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/)
