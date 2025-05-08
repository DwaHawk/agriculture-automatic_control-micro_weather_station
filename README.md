# Micro Weather Station Data Collection System

This project demonstrates how to integrate multiple sensors using an ESP32 and upload the data to Google Sheets, establishing a basic IoT weather data platform.

|[繁體中文](language/README_TC.md)|

## Project Background

In agriculture, real-time environmental data is crucial for decision-making. However, sensors and central controllers are often too expensive for frontline farmers to adopt.

With the rising popularity of open-source platforms like Arduino and related peripherals, the cost of such technology has been significantly reduced. This project uses the ESP32 as the core controller to connect commonly used sensors and upload the collected data to Google Sheets for further analysis.

Special thanks to the Department of Applied Statistics and Information Science at Ming Chuan University, Taiwan for their assistance in development and real-world implementation of this system.

## ✨ Features

- ESP32 integrates multiple environmental sensors
- Real-time data upload to Google Sheets
- Combines Arduino with Google Apps Script for remote data collection

## 🧩 Sensors Used

This project uses affordable industrial or research-grade sensors. Sensors are replaceable as long as they follow the communication protocols and power limitations supported by ESP32.

👉 [Click to view the detailed sensor list](hardware/sensors_list.md)

## 🔧 Setup Instructions

### Step 1 – Google Sheets & Apps Script

1. Create a Google Sheet and name it `tableA` or any name you prefer.
2. Open Google Apps Script and paste the code from `google_scripts/weatherRecordApp.js`.
3. Deploy it as a Web App to get the Web App URL and `deploy ID`.
4. Update `GOOGLE_SCRIPT_ID` in `esp32/weather_station.ino` with your own deploy ID.

### Step 2 – Arduino Development Environment

1. Install Arduino IDE and add the ESP32 board support  
   ([Installation Guide](https://hackmd.io/@DwaHawk/ByxO0qQ5T))
2. Import the `weather_station.ino` sketch.
3. Ensure required libraries are installed (e.g., DHT, BH1750, Adafruit BMP180).
4. Update the WiFi SSID and password in the sketch.
5. Upload the sketch to your ESP32 development board.  
   > ⚠️ You may encounter issues due to board/driver mismatches—try using older drivers if necessary.

## 🧱 Notes

- If ESP32 WiFi is unstable, it is recommended to implement an automatic reconnection mechanism.
- The RG-15 rain sensor may sometimes return "NA"; consider using other types of rain gauges if needed.
- For future improvements, designing a PCB is recommended to reduce wiring errors and increase durability.

## 📜 License

- All code in this project (ESP32 and Google Apps Script) is licensed under the [MIT License](LICENSE).
- All documentation and graphical content (including this README and sensor list) is licensed under  
  [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

## 🙋 Contact

Developed by **David Huang**.  
Contributions and discussions are welcome!

📧 huang7766@gmail.com
