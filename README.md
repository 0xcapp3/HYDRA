# HYDRA (HYDRoponic Autonoumus monitoring system)

## Introduction

Welcome to HYDRA project!

[hydra video/gif]

This project is an advanced and fully automated hydroponic system for growing plants in a soilless medium using nutrient-rich water. The system utilizes a range of sensors and actuators to monitor and control the growing environment, ensuring optimal conditions for the plants at all times.

The system includes a pH sensor to measure the acidity or alkalinity of the nutrient solution, a BMP280 sensor to measure temperature and atmospheric pressure, an LED semaphore to signal when conditions are outside of the optimal range, and an OLED display to provide real-time information about the growing environment.

The system is designed to be highly energy efficient and can be easily integrated into a variety of electronic devices and systems. It can be used in a wide range of applications, including home gardening, commercial agriculture, and research.

I hope you find this project useful and we welcome any feedback or suggestions you may have. Thank you for visiting!

## What is an hydroponic system?

As anticipated before, hydroponics is a method of growing plants using nutrient-rich water instead of soil. In a hydroponic system, plants are grown in a soilless medium, such as perlite, vermiculite, or coconut coir, and are fed a nutrient-rich solution on a regular basis. This solution typically includes all of the essential minerals and nutrients that plants need to grow and thrive.

There are many different types of hydroponic systems, including nutrient film technique (NFT), deep water culture (DWC), aeroponics, and drip irrigation. These systems differ in the way that they deliver nutrients and water to the plants, as well as the type of growing medium that is used.

Hydroponic systems can be used to grow a wide variety of plants, including fruits, vegetables, herbs, and flowers. They are often used in greenhouse or indoor growing environments, and can be a more efficient and sustainable way to grow crops, as they use less water and pesticides than traditional soil-based agriculture.

## Hardware

### Board

[board image]

- [board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html)

The Nucleo F401RE development board is a compact, low-cost microcontroller development board that is designed to help engineers and hobbyists build and prototype projects with the STM32F401RE microcontroller. The board is based on the STM32F401RE microcontroller, which is part of the STM32F4 series of high-performance microcontrollers from STMicroelectronics.

The STM32F401RE microcontroller is a 32-bit ARM Cortex-M4 device that is designed for high-performance applications. It has a clock speed of up to 84 MHz, 512 KB of flash memory, and 96 KB of SRAM. The microcontroller also has a range of peripherals, including 12-bit ADC, DAC, timers, communication interfaces (SPI, I2C, USART), and a USB 2.0 OTG (on-the-go) controller.

The Nucleo F401RE development board has a range of features and connectors to make it easy to use and prototype with the STM32F401RE microcontroller. It has a built-in ST-LINK/V2-1 debugger and programmer, which can be used to program and debug the microcontroller using a PC. The board also has an Arduino Uno V3 connector, which allows it to be used with a range of Arduino-compatible shields and modules.

Other features of the Nucleo F401RE development board include an on-board ST-LINK/V2-1 debugger and programmer, a USB connector for power and programming, a user button and LED, and a 32.768 kHz crystal oscillator. The board is also compatible with a range of software development tools, including the STM32CubeIDE and the Arduino Integrated Development Environment (IDE).

Overall, the Nucleo F401RE development board is a versatile and cost-effective platform for building and prototyping projects with the STM32F401RE microcontroller. It is suitable for a wide range of applications, including Internet of Things (IoT) projects, embedded systems, and robotics.

### Sensors
#### BMP280

[sensor image]

The BMP280 is a small, low-power digital sensor that can measure temperature and pressure. It is commonly used in weather stations, altimeters, cars, and other applications that require accurate measurement of atmospheric pressure and temperature.

The sensor is based on a microelectromechanical system (MEMS) pressure sensor, which uses a small, flexible membrane to measure pressure changes. The membrane is connected to a Wheatstone bridge circuit, which converts the pressure change into an electrical signal. This signal is then processed by the sensor's onboard microcontroller to provide a temperature and pressure reading.

The BMP280 has a temperature measurement range of -40 to 85 degrees Celsius, with an accuracy of +/- 1 degree Celsius. It has a pressure measurement range of 300 to 1100 hPa (hectopascals), with an accuracy of +/- 1 hPa. The sensor has a high resolution of 0.18 hPa for pressure measurement, which allows for highly accurate readings at different altitudes.

In addition to its temperature and pressure measurement capabilities, the BMP280 also has a built-in altimeter function, which can calculate the altitude based on the measured pressure. This feature can be useful in applications such as weather stations, where altitude is an important factor in weather prediction.

Generally speaking, the BMP280 is available in a compact, surface-mount package, making it easy to integrate into a variety of electronic devices. It operates on a voltage of 1.8 to 3.6 volts and has a low power consumption of only 1.8 microamps in power-down mode. This makes it well-suited for use in battery-powered devices or applications where power consumption is a concern.

##### I2C interface

The I2C interface uses the following pins:

- SCK: serial clock (SCL)
- SDI: data (SDA)
- SDO: the I2C address decides the pin. If SDO connects to GND (0), the address is 0x76, if it connects to VDDIO (1), the address is 0x77. In this module, we have connected  it to VDDIO, so the address should be 0x77.
- CSB: Must be connected to VDDIO to select I2C interface 

...

[add image tech_data_bmp280]

#### SEN0161

[sensor image]

A pH sensor is a device that is used to measure the acidity or alkalinity of a solution. It works by detecting the concentration of hydrogen ions in a solution and expressing the results on a pH scale, which ranges from 0 to 14. A pH of 7 is neutral, while a pH below 7 is considered acidic and a pH above 7 is considered alkaline.

There are several different types of pH sensors, including glass electrodes, plastic electrodes, and ISFET (ion-sensitive field-effect transistor) sensors. These sensors work by detecting the electrical potential of the hydrogen ions in a solution and converting it into a measurable voltage. The voltage is then processed by the sensor's onboard electronics to provide a pH reading.

pH sensors are commonly used in a wide variety of applications, including water treatment, food and beverage processing, pharmaceutical manufacturing, and environmental monitoring. They are also used in hydroponic systems to monitor the pH of the nutrient solution and ensure optimal growing conditions for plants.

...

[technical data]

useful stuff:
    - [dfrobot](https://wiki.dfrobot.com/PH_meter_SKU__SEN0161_)
    - 

### Actuators
#### Semaphore

[semaphore image]

The LED semaphore is an actuator that can be used to signal when the pH, temperature or pressure in the growing environment is outside of the optimal range for the plants. The semaphore can be programmed to display different colors or patterns, depending on the specific conditions in the system.

#### Oled display

[oled image]

The OLED display is used to display real-time information about the pH, temperature, and atmospheric pressure in the growing environment. This information can be used by the grower to make informed decisions about the care and maintenance of the plants.

##### u8g2 library

## How to run

### Setup

You first need to add your AWS certificates in the `bridge/` folder, the following are needed:
- `bridge/root-CA.crt`
- `bridge/<name>.private.key`
- `bridge/<name>.cert.pem`

### Startup scripts

To start HYDRA both `flash_run.sh` and `term_run.sh` scripts are provided. The first will flash the firmware and then execute it whereas the second one ... .



It requires `tmux` to be installed, and the device to be connected via usb. When run, the script will start mosquitto, the MQTT transparent bridge, the EthOS serial connection and the webserver.

## Demo video

A video presentation and demo of the project can be found [here](...)
