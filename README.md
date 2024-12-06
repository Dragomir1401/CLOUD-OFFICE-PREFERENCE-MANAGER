# PRIOT Project - Documentation

## Introduction

This documentation describes the implementation of an IoT system for managing employee access and monitoring the environment (temperature and humidity). The project combines a sensor network with an ESP32 hub that communicates with a central server to store and visualize data.

Employees receive alerts when the temperature or humidity is not optimal for their inputted preferences. The system also logs access events, allowing administrators to monitor employee activity: logins, logouts, access attempts, time spent in the office, etc.

### Key Objectives:

- Manage employee access through an RFID system.
- Monitor the environment using a DHT11 sensor.
- Visualize collected data via an interactive web interface.

---

### Architecture

#### Overview

The system's architecture integrates multiple hardware components connected to an ESP32 microcontroller and communicates with a Flask server for centralized data management and visualization. The design includes:

- **Hardware Peripherals**:
  - **RFID Module**: For employee identification and access management.
  - **Joystick**: Enables administrative navigation through the system's menu directly on the ESP32.
  - **RTC (Real-Time Clock)**: Logs accurate time and date for employee activities.
  - **RGB LED**: Provides visual feedback for login, logout, and error events.
  - **Buzzer**: Supplies auditory feedback for successful or failed operations.
  - **DHT11 Sensor**: Captures temperature and humidity data during employee login.
  - **LCD Display**: Shows system status, navigation options, and access logs locally.

- **Protocols**:
  - **I2C**: Used for communication with the RTC module and LCD screen.
  - **SPI**: Enables high-speed communication with the RFID reader.
  - **Analog/Digital I/O**: Interfaces with the joystick, RGB LED, buzzer, and DHT11 sensor.
  - **Wi-Fi**: ESP32 connects to the Flask server using Wi-Fi for data transmission.
  - **HTTP**: Facilitates communication between the ESP32 and Flask server for sending and receiving JSON-formatted data.

#### Hardware and Network Schema

```plaintext
+-----------------------------+           HTTP POST/GET           +-----------------------+
|          ESP32              | <--------------------------------> |   Flask Server (PC)   |
|                             |                                    |                       |
|   +----------------------+  |                                    |                       |
|   |  RFID Reader (SPI)   |  |                                    |     Web Interface     |
|   +----------------------+  |                                    |   (HTML + CSS/JS)     |
|                             |                                    +-----------------------+
|   +----------------------+  |
|   |   Joystick (GPIO)    |  |
|   +----------------------+  |
|                             |
|   +----------------------+  |
|   |RTC Module(three-wire)|  |                  WIFI               +-------------------+
|   +----------------------+  | <---------------------------------->|  Wi-Fi Router     |
|                             |                                     | (Local Network)   |
|   +----------------------+  |                                     +-------------------+
|   |   RGB LED (GPIO/PWM) |  |
|   +----------------------+  |
|                             |
|   +----------------------+  |
|   |    Buzzer (GPIO)     |  |
|   +----------------------+  |
|                             |
|   +----------------------+  |
|   |    DHT11 (GPIO)      |  |
|   +----------------------+  |
|                             |
|   +----------------------+  |
|   |  LCD Screen (I2C)    |  |
|   +----------------------+  |
+-----------------------------+
