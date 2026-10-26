# STM32-Based Environmental Monitoring System

## Overview
A simulation-based embedded systems project using the STM32 Blue Pill and DHT22 sensor to monitor temperature and humidity.

## Features
- Real-time temperature and humidity readings
- Temperature and humidity threshold alerts
- OLED display for measurements and status
- Green and red LED status indicators
- Buzzer alerts when thresholds are exceeded
- UART serial output for monitoring sensor data
- Basic sensor error handling

## Hardware and Tools
- STM32 Blue Pill (STM32F103C8T6)
- DHT22 temperature and humidity sensor
- SSD1306 OLED display
- Red and green LEDs
- Buzzer
- Wokwi simulator
- Arduino C++ and embedded libraries

Wokwi Simulation
https://wokwi.com/projects/478930329352577025

Simulation Results
- Initial sensor reading: 24.0°C
- Initial humidity reading: 40.0%
- Reported status: NORMAL

## Learning Outcomes
- Practised microcontroller GPIO control
- Learned sensor interfacing concepts
- Worked with I2C display communication
- Used serial output for debugging and monitoring
- Implemented threshold-based alert logic
- Practised basic embedded programming

## Source Files
- `stm32-analog.ino` — program code
- `diagram.json` — circuit configuration
- `libraries.txt` — required libraries

## Author
Nisarga H G
