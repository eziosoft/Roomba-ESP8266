# Roomba-ESP8266 Control System

ESP8266-based control system for iRobot Roomba using MQTT protocol and WiFi connectivity.

## Features
- MQTT-based control interface
- WiFi configuration manager for easy setup
- Over-the-Air (OTA) firmware updates
- Real-time telemetry streaming
- Motor control with safety limits
- LED status indicators
- Sensor data monitoring
- Dock seeking/undocking commands

## Hardware Requirements
- ESP8266 microcontroller
- iRobot Roomba with serial interface
- Compatible WiFi network
- MQTT broker (local or public)

## Installation
1. Install required libraries:
   - ESP8266WiFi
   - PubSubClient
   - WiFiManager
   - ArduinoOTA

2. Connect Roomba's serial interface to ESP8266

3. Configure pins in `roombaDefines.h`:
   ```cpp
   int buttonPin = 12;
   int ledPin=D2;
   int ddPin=D5;  // Device detect pin
   ```

## MQTT Configuration
- **Broker**: `192.168.0.19` (configure in code)
- **Topics**:
  - Control: `tank/in`
  - Telemetry: `tank/out`
  - Data Stream: `tank/stream`
  - Debug: `tank/debug`

## Usage
1. Power on the Roomba
2. Connect to WiFi AP "Robot"
3. Send MQTT commands:
   - Movement: `$[len][ch1][ch2][ch3][ch4]`
   - Special commands:
     - `3`: Seek dock
     - `4`: Undock
     - `5`: Power off
     - `10`: Brush control
     - `20`: Start sensor stream

Example command format:
```python
# Channel values: -100 to 100
command = bytes([b'$', 4, 100, 50, 0, 0])
```

## OTA Updates
Supported through ArduinoOTA. Connect via Arduino IDE for wireless firmware updates.

## File Structure
- `roombaDefines.h`: Pin definitions & constants
- `roombaMQTTControl.ino`: Main control logic
- `motorFunctions.ino`: Drive system implementation
- `sensorsFunctions.ino`: Sensor data handling
- `LEDs.ino`: Status indicator controls

## License
[Specify license here]