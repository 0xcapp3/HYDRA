# HYDRA (HYDRoponic Autonoumus monitoring system)

*Individual project for the IoT course 2022 at La Sapienza University of Rome*

The project was born from the need ... . To solve that problem ... .

## Introduction

Welcome to HYDRA project!

This project is an advanced and fully automated hydroponic system for growing plants in a soilless medium using nutrient-rich water. The system utilizes a range of sensors and actuators to monitor and control the growing environment, ensuring optimal conditions for the plants at all times.

The system includes a pH sensor to measure the acidity or alkalinity of the nutrient solution, a BMP280 sensor to measure temperature and atmospheric pressure, an LED semaphore to signal when conditions are outside of the optimal range, and an OLED display to provide real-time information about the growing environment.

The system is designed to be highly energy efficient and can be easily integrated into a variety of electronic devices and systems. It can be used in a wide range of applications, including home gardening, commercial agriculture, and research.

I hope you find this project useful and we welcome any feedback or suggestions you may have. Thank you for visiting!

## What is an hydroponic system?

As anticipated before, hydroponics is a method of growing plants using nutrient-rich water instead of soil. In a hydroponic system, plants are grown in a soilless medium, such as perlite, vermiculite, or coconut coir, and are fed a nutrient-rich solution on a regular basis. This solution typically includes all of the essential minerals and nutrients that plants need to grow and thrive.

There are many different types of hydroponic systems, including nutrient film technique (NFT), deep water culture (DWC), aeroponics, and drip irrigation. These systems differ in the way that they deliver nutrients and water to the plants, as well as the type of growing medium that is used.

Hydroponic systems can be used to grow a wide variety of plants, including fruits, vegetables, herbs, and flowers. They are often used in greenhouse or indoor growing environments, and can be a more efficient and sustainable way to grow crops, as they use less water and pesticides than traditional soil-based agriculture.

## Architecture 

The IoT device is developed using RIOT-OS and a STM NUCLEO-f401re board. The cloud-based services are based on the AWS ecosystem. The system includes two sensors and two actuators. All hardware components will be examined in depth in the specific section.

The board is connected through MQTT-SN to a broker hosted on the machine the board is connected to. The connection is carried out using IPv6 and RIOT-OS tap interfaces. The board publishes on “topic_out” and subscribes to “topic_in” to receive messages from outside.

A transparent bridge written in python is used to forward messages to and from AWS IoTCore. It runs on the machine the board is connected to. It reads messages from the local broker with “topic_out” and publishes them to AWS IoTCore on the same topic. It also reads messages from AWS IoTCore with “topic_in” and publishes them on the local broker with the same topic.

Once data arrives to AWS IoTCore the computation proceeds on the AWS cloud using the following services: DynamoDB, Lambda, API Gateway, Amplify.

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220335213-afa49440-1521-4cd2-8cd1-0b1de67a65fc.png" width="75%"></img>
</p>

## Hardware

### Board

[board image]

- [board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html)

The Nucleo F401RE development board is a compact, low-cost microcontroller development board that is designed to help engineers and hobbyists build and prototype projects with the STM32F401RE microcontroller. The board is based on the STM32F401RE microcontroller, which is part of the STM32F4 series of high-performance microcontrollers from STMicroelectronics.

The STM32F401RE microcontroller is a 32-bit ARM Cortex-M4 device that is designed for high-performance applications. It has a clock speed of up to 84 MHz, 512 KB of flash memory, and 96 KB of SRAM. The microcontroller also has a range of peripherals, including 12-bit ADC, DAC, timers, communication interfaces (SPI, I2C, USART), and a USB 2.0 OTG (on-the-go) controller.

The Nucleo F401RE development board has a range of features and connectors to make it easy to use and prototype with the STM32F401RE microcontroller. It has a built-in ST-LINK/V2-1 debugger and programmer, which can be used to program and debug the microcontroller using a PC. The board also has an Arduino Uno V3 connector, which allows it to be used with a range of Arduino-compatible shields and modules.

Other features of the Nucleo F401RE development board include an on-board ST-LINK/V2-1 debugger and programmer, a USB connector for power and programming, a user button and LED, and a 32.768 kHz crystal oscillator. The board is also compatible with a range of software development tools, including the STM32CubeIDE and the Arduino Integrated Development Environment (IDE).

Overall, the Nucleo F401RE development board is a versatile and cost-effective platform for building and prototyping projects with the STM32F401RE microcontroller. It is suitable for a wide range of applications, including Internet of Things (IoT) projects, embedded systems, and robotics.

### BMP280

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220339968-0b16c6e6-f367-4c37-8be3-92db3ef12f5b.jpeg" width="45%"></img>
</p>

The BMP280 is a small, low-power digital sensor that can measure temperature and pressure. It is commonly used in weather stations, altimeters, cars, and other applications that require accurate measurement of atmospheric pressure and temperature.

The sensor is based on a microelectromechanical system (MEMS) pressure sensor, which uses a small, flexible membrane to measure pressure changes. The membrane is connected to a Wheatstone bridge circuit, which converts the pressure change into an electrical signal. This signal is then processed by the sensor's onboard microcontroller to provide a temperature and pressure reading.

The BMP280 has a temperature measurement range of -40 to 85 degrees Celsius, with an accuracy of +/- 1 degree Celsius. It has a pressure measurement range of 300 to 1100 hPa (hectopascals), with an accuracy of +/- 1 hPa. The sensor has a high resolution of 0.18 hPa for pressure measurement, which allows for highly accurate readings at different altitudes.

In addition to its temperature and pressure measurement capabilities, the BMP280 also has a built-in altimeter function, which can calculate the altitude based on the measured pressure. This feature can be useful in applications such as weather stations, where altitude is an important factor in weather prediction.

Generally speaking, the BMP280 is available in a compact, surface-mount package, making it easy to integrate into a variety of electronic devices. It operates on a voltage of 1.8 to 3.6 volts and has a low power consumption of only 1.8 microamps in power-down mode. This makes it well-suited for use in battery-powered devices or applications where power consumption is a concern.

#### I2C interface

The I2C interface uses the following pins:

- **SCK**: serial clock (SCL)

- **SDI**: data (SDA)

- **SDO**: the I2C address decides the pin. If SDO connects to GND (0), the address is 0x76, if it connects to VDDIO (1), the address is 0x77. In this module, we have connected it to VDDIO, so the address should be 0x77.

- **CSB**: Must be connected to VDDIO to select I2C interface

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220340205-9d057c5b-aa1f-4bec-9021-a9b58046bad3.jpeg" width="55%"></img>
</p>


### pH Sensor

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220337549-eed767c6-2964-445a-b887-6397c32dfe24.jpeg" width="45%"></img>
</p>

A pH sensor is a device that is used to measure the acidity or alkalinity of a solution. It works by detecting the concentration of hydrogen ions in a solution and expressing the results on a pH scale, which ranges from 0 to 14. A pH of 7 is neutral, while a pH below 7 is considered acidic and a pH above 7 is considered alkaline.

There are several different types of pH sensors, including glass electrodes, plastic electrodes, and ISFET (ion-sensitive field-effect transistor) sensors. These sensors work by detecting the electrical potential of the hydrogen ions in a solution and converting it into a measurable voltage. The voltage is then processed by the sensor's onboard electronics to provide a pH reading.

pH sensors are commonly used in a wide variety of applications, including water treatment, food and beverage processing, pharmaceutical manufacturing, and environmental monitoring. They are also used in hydroponic systems to monitor the pH of the nutrient solution and ensure optimal growing conditions for plants.

### Specifications

- Module Power : 5.00V
- Module Size : 43mm×32mm
- Measuring Range:0-14PH
- Measuring Temperature :0-60 ℃
- Accuracy : ± 0.1pH (25 ℃)
- Response Time : ≤ 1min
- pH Sensor with BNC Connector
- PH2.0 Interface ( 3 foot patch )
- Gain Adjustment Potentiometer
- Power Indicator LED

### Useful links
  - [dfrobot](https://wiki.dfrobot.com/PH_meter_SKU__SEN0161_)

### Semaphore

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220322270-7758ffcf-c7bb-471a-ba75-7d19e4b60a81.jpg" width="35%"></img>
</p>

The LED semaphore is an actuator that can be used to signal when the pH, temperature or pressure in the growing environment is outside of the optimal range for the plants. The semaphore can be programmed to display different colors or patterns, depending on the specific conditions in the system.

### Oled display

<p align="center">
  <img src="https://user-images.githubusercontent.com/29161223/220323008-dfbad0b8-dd4d-464a-97da-e4811ee15c3a.jpg" width="35%"></img>
</p>

The OLED display is used to display real-time information about the pH, temperature, and atmospheric pressure in the growing environment. This information can be used by the grower to make informed decisions about the care and maintenance of the plants.

#### U8g2 library

U8g2 is a monochrome graphics library for embedded devices ... .

## Network

In the network there will be transmitted different data (i.e. temperature, pH and pressure) coming from the board and the dispense message going to the board. These messages are small, so even a narrow band will be suitable for our use.

Latencies are short enough to not affect the usability of the system and are compliant with the objectives set before the development.

Data is transmitted every `n` seconds depending on needs. Clearly there will be overhead due to headers necessary to transmit the messages. MQTT-SN was chosen as the protocol to transmit messages because of its characteristics suitable for IoT applications, in particular for its small overhead.

## How to run
### Setup

You first need to add your AWS certificates in the `bridge/` folder, the following are needed:
- `bridge/root-CA.crt`
- `bridge/<name>.private.key`
- `bridge/<name>.cert.pem`

Moreover be sure to have `pipenv` installed.

### Startup scripts

To start HYDRA both `flash_run.sh` and `term_run.sh` scripts are provided. The first will flash the firmware and then execute it whereas the second one will only start the device. Both will initialize the EthOS serial connection.

Along that it is necessary to start mosquitto with the following command inside its folder: `./broker_mqtts conf.conf` (?).

At last, but not least, for the MQTT transparent bridge we need to create a python virtual environemt using `pipenv` and the dedicated `Pipfile`. At first go inside bridge folder and run `pipenv shell`, then `pipenv install`. Once we have our environment ready we can run it with the following command: `python3 transparent_bridge.py`.

## Demo video

A video presentation and demo of the project can be found [here](...)
