# Surf Condition Indicator

## 🏄‍♂️ Overview
The **Surf Condition Indicator** is an IoT-powered decoration using an ESP32, NeoPixel LED strip, and servo motor to display real-time wave height and wind speed. Data is fetched hourly from OpenWeatherMap and Stormglass APIs, providing an interactive way to visualize ocean conditions.

🎥 **Demo Video**: [Watch here](https://youtu.be/dNDxYnmZQeg)

## 🛠️ Features
- **Live Weather Data**: Fetches wave height and wind speed from online APIs.
- **LED Wave Animation**: Blue LEDs simulate wave movement.
- **Wind Speed Servo Control**: Adjusts servo motor based on wind conditions.
- **Hourly Updates**: Fetches new data every hour.
- **Wi-Fi Connectivity**: Connects to the internet to retrieve weather information.

## 🎯 How It Works
1. **Connects to Wi-Fi** to retrieve data from OpenWeatherMap and Stormglass APIs.
2. **Controls LED Strip** to display wave height using moving blue LEDs.
3. **Adjusts Servo Motor** to reflect wind speed changes.
4. **Updates Automatically** every hour to reflect real-time conditions.

## 🔧 Hardware Requirements
- **ESP32**
- **WS2812B NeoPixel LED Strip**
- **SG90 Servo Motor**
- **Wi-Fi Connection**

## 📝 Circuit Connections
| Component         | Pin on ESP32 |
|------------------|-------------|
| NeoPixel LED Strip | 26 |
| Servo Motor       | 23 |

## 🚀 Setup & Installation
1. **Install Arduino IDE** (if not already installed).
2. **Add Required Libraries**:
   - `WiFi`
   - `ArduinoJson`
   - `Adafruit NeoPixel`
   - `ESP32Servo`
3. **Upload the Code** to ESP32.
4. **Connect the Components** as per the table above.
5. **Fill in your Wi-Fi credentials** in the code.
6. **Power Up & Enjoy Live Surf Data!**

## 🌊 API Usage
- **OpenWeatherMap API**: Fetches wind speed.
- **Stormglass API**: Fetches wave height.
- **WorldTimeAPI**: Retrieves current time for synchronization.

## 📌 Authors
- **Oren Savich**
- **Rotem Peled**
