# 🩺 OxyBeat - Your Desk-Side Health Companion

A compact, always-on health monitor built on the ESP32, purpose-built for anyone who spends long hours in front of a screen.

---

## 🗂️ Table of Contents
- [What is OxyBeat?](#what-is-oxybeat)
- [The Problem We Solve](#the-problem-we-solve)
- [What We Set Out to Learn](#what-we-set-out-to-learn)
- [What It Can Do](#what-it-can-do)
- [Hardware](#hardware)
- [How the System Fits Together](#how-the-system-fits-together)
- [Software and Libraries](#software-and-libraries)
- [Pin Map](#pin-map)
- [Running the Project](#running-the-project)
- [The Team](#the-team)
- [References](#references)
- [License](#license)
- [Acknowledgements](#acknowledgements)

---

## 🔬 What is OxyBeat?

OxyBeat is a desk-side health monitoring device developed as a first-year, first-semester group project at **SLIIT**. It sits quietly next to your computer and keeps tabs on your pulse, blood oxygen levels, body temperature, room conditions, and even how close you are to your screen - all without you having to do a thing.

Readings are shown on a built-in OLED screen and pushed live to your phone through the **Blynk IoT platform**, so your health data is always within reach.

---

## ⚠️ The Problem We Solve

Desk workers and students often go hours without thinking about their physical wellbeing - poor posture, screen proximity, fluctuating room conditions, and unnoticed health changes can all add up. OxyBeat was built to quietly watch over these things in the background, surfacing alerts and data only when something needs attention.

---

## 🎯 What We Set Out to Learn

- Interfacing optical sensors for non-invasive health measurement
- Working with digital environmental sensors over I2C and OneWire protocols
- Implementing real-time signal processing for BPM and SpO2 estimation
- Connecting embedded hardware to the cloud using Wi-Fi and the Blynk platform
- Designing a modular, multi-sensor embedded system on the ESP32

---

## ⚡ What It Can Do

- **Pulse Tracking** - Measures your heart rate continuously using infrared light, showing live BPM on screen
- **Blood Oxygen Estimation** - Calculates SpO2 percentage by analysing the ratio of red and infrared light absorbed by your finger
- **Body Temperature Watch** - Reads your body temperature and sounds a buzzer the moment it strays outside a safe range
- **Room Temperature Indicator** - Monitors the ambient temperature around your desk and lights up a green LED when it's within a comfortable range
- **Humidity Check** - Keeps an eye on air moisture levels and gives a green LED signal when conditions are healthy
- **Screen Distance Guard** - Fires an ultrasonic pulse to measure how far you're sitting from your monitor - a red LED kicks in if you're too close
- **On-Device Display** - All readings are rendered live on a compact OLED screen mounted on the device itself
- **Phone Dashboard** - Everything is streamed to the Blynk app on your smartphone, complete with event history and trend tracking

---

## 🔧 Hardware

| Component | Qty | What It Does |
|---|---|---|
| ESP32 | 1 | Brains of the operation - handles all processing, display, and Wi-Fi |
| MAX30102 Pulse Oximeter | 1 | Measures heart rate and SpO2 optically |
| DS18B20 Temperature Sensor | 1 | Reads body temperature via OneWire |
| DHT11 Sensor | 1 | Captures room temperature and humidity |
| HC-SR04 Ultrasonic Sensor | 1 | Detects distance between user and screen |
| SH1106 OLED Display (128×64) | 1 | Shows all sensor data locally |
| Green LED | 2 | Visual indicators for humidity and room temperature |
| Red LED | 1 | Proximity warning light |
| Buzzer | 1 | Audible alarm for abnormal body temperature |

---

## 🏗️ How the System Fits Together

- The **MAX30102** samples infrared and red light through the fingertip, feeding a rolling data window that calculates BPM and SpO2 in real time
- The **DS18B20** polls body temperature over a single wire and triggers the buzzer if a threshold is crossed
- The **DHT11** reports both ambient temperature and humidity, each independently driving a status LED
- The **HC-SR04** emits an ultrasonic burst, times the echo, and converts it to a distance - the red LED activates if the user is sitting too close
- The **OLED screen** refreshes each cycle with the latest values from every sensor
- The **ESP32** maintains a Wi-Fi connection and pushes all data to Blynk every second, while also logging notable events for later review

---

## 💾 Software and Libraries

| Library | Purpose |
|---|---|
| `U8g2` | Drives the SH1106 OLED display |
| `Wire` | I2C communication for OLED and MAX30102 |
| `MAX30105` (SparkFun) | Interface for the pulse oximeter sensor |
| `OneWire` | Single-wire protocol for DS18B20 |
| `DallasTemperature` | Simplified temperature reads from DS18B20 |
| `DHT` (Adafruit) | Temperature and humidity reads from DHT11 |
| `WiFi` | ESP32 wireless connectivity |
| `BlynkSimpleEsp32` | Cloud data streaming via Blynk |

**IDE:** Arduino IDE (C++)

---

## 📌 Pin Map

| GPIO Pin | Connected To | Direction |
|---|---|---|
| `21` | OLED & MAX30102 - SDA | I2C Data |
| `22` | OLED & MAX30102 - SCL | I2C Clock |
| `25` | DS18B20 | OneWire Data |
| `26` | DHT11 | Digital Data |
| `18` | HC-SR04 Trigger | Output |
| `19` | HC-SR04 Echo | Input |
| `12` | Green LED (Humidity) | Output |
| `13` | Red LED (Proximity) | Output |
| `14` | Buzzer | Output |
| `27` | Green LED (Room Temp) | Output |

---

## 🚀 Running the Project

**Prerequisites**
- Arduino IDE with the ESP32 board package installed
- All libraries listed above installed via the Library Manager

**Steps**

1. Clone this repository:
   ```bash
   git clone https://github.com/PoornaPalihakkara/OxyBeat
   cd OxyBeat
   ```

2. Open `OxyBeat.ino` in Arduino IDE

3. Fill in your Blynk credentials:
   ```cpp
   #define BLYNK_TEMPLATE_ID   "your_template_id"
   #define BLYNK_TEMPLATE_NAME "your_template_name"
   char auth[] = "your_auth_token";
   char ssid[] = "your_wifi_name";
   char pass[] = "your_wifi_password";
   ```

4. Select `ESP32 Dev Module` under `Tools > Board`

5. Upload the sketch to your ESP32

6. Open the Blynk app, load your dashboard, and power up the device

---

## 👨‍💻 The Team

| Name | Responsibility |
|---|---|
| **Poorna** (ME) | MAX30102 pulse oximeter, SpO2 signal processing & OLED display |
| **Gayathri** | DHT11 humidity sensing & green LED control |
| **Yenuli** | HC-SR04 ultrasonic proximity detection & red LED warning |
| **Dishan** | DS18B20 body temperature sensing & buzzer alert logic |
| **Dinali** | DHT11 room temperature sensing & green LED control |
| **Vinuka** | Blynk IoT integration, Wi-Fi connectivity & cloud data streaming |

> First Year — First Semester Group Project &nbsp;|&nbsp; **SLIIT, Sri Lanka — 2025**

---

## 📚 References

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [SparkFun MAX30105 Library](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)
- [U8g2 Graphics Library](https://github.com/olikraus/u8g2)
- [Blynk IoT Documentation](https://docs.blynk.io/)
- [DS18B20 with ESP32 — Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/)
- [DHT11 Sensor Guide](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/)
- [HC-SR04 Ultrasonic Sensor Guide](https://randomnerdtutorials.com/esp32-hc-sr04-ultrasonic-arduino/)

---

## 📜 License

This project was developed as part of academic coursework at **SLIIT** and is intended for educational purposes. Feel free to reuse or build upon it - just make sure to credit the original team. Released under the [MIT License](LICENSE).

© 2025 _Y1S1Mtr8_ — **OxyBeat**

---

## 🤝 Acknowledgements

- SLIIT lecturers and instructors for their guidance throughout the semester
- Y1S1Mtr8 teammates for the effort, late nights, and collaboration
- The Arduino, ESP32, and Blynk open-source communities
- Random Nerd Tutorials and similar resources that made hardware prototyping approachable
- Everyone who tested the device and gave feedback during development

---

Found this useful? Give the repo a ⭐ - it means a lot!  
Feedback, forks, and ideas are always welcome.

---

🛠️ Designed and built with dedication by **_Y1S1Mtr8 | SLIIT 2025_**

---

## 🔗 GitHub Repository
> https://github.com/PoornaPalihakkara/OxyBeat
