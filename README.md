# 🛡️ mmWave-Based Indoor Fall Detection System  
**CASA0016 – Making, Designing & Building Connected Sensor Systems**

An edge-based, privacy-preserving indoor fall detection prototype using a 60 GHz mmWave radar sensor, MQTT communication, and real-time WhatsApp alerts.

---

## 📌 Project Overview

Falls are a major cause of injury among older adults, particularly in indoor environments where rapid detection is critical.  
This project designs, builds, and evaluates an **indoor fall detection prototype** using a **60 GHz mmWave radar sensor** integrated with an **ESP32-based microcontroller**.

Unlike vision-based approaches, the system enables **continuous, non-contact monitoring** while preserving user privacy. Fall detection is performed locally using embedded radar firmware, with **edge-based event handling** triggering both local alerts and remote notifications.

⚠️ **Disclaimer:** This prototype is not a medical device and is intended for educational and exploratory purposes only.

---

## 🧠 Key Capabilities

- Privacy-preserving, non-contact sensing  
- Edge-based fall detection (no raw motion data transmitted)  
- Immediate local alerts (RGB LED + buzzer)  
- Remote notifications via WhatsApp  
- Lightweight IoT communication using MQTT  
- Designed for continuous indoor deployment  

---

## 🏗️ System Architecture

The system follows an **edge-centric, event-driven architecture**:

### 1️⃣ Sensing & Classification (Edge)
- 60 GHz mmWave radar performs embedded fall detection
- Abstracted states: `no presence`, `presence`, `fall`
- No transmission of raw sensor data

### 2️⃣ Local Feedback
- RGB LED indicates system state
- Audible buzzer activates on confirmed fall events

### 3️⃣ Communication & Alerts
- MQTT publishes lightweight status messages
- WhatsApp alerts sent via HTTP API
- NTP time synchronisation for alert timestamps
- One-time alert logic prevents notification flooding

📷 See `/media/` for architecture diagrams and system visuals.

---

## 🔩 Hardware Components

| Component | Description |
|---------|-------------|
| MR60FDA2 | 60 GHz mmWave fall detection module |
| XIAO ESP32C6 | Embedded microcontroller with Wi-Fi |
| RGB LED (WS2812B) | Visual system state indication |
| Grove Buzzer | Audible fall alert |
| BH1750 | Ambient light sensor (contextual only) |
| Power Supply | 5 V USB (always-on indoor deployment) |

**Mounting:** Ceiling-mounted at ~2.5 m in a residential room.

---

## 🧱 Physical Implementation

**Figure:** Physical implementation of the prototype  
- **(a)** Fusion 360 enclosure design  
- **(b)** 3D-printed enclosure with device placement  
- **(c)** Grove buzzer for audible alerts  

📂 CAD files and enclosure images are available in `/enclosure/`.

---

## 💻 Software Stack

- Arduino IDE  
- Seeed Arduino mmWave Library  
- Adafruit NeoPixel Library  
- WiFi & PubSubClient (MQTT)  
- HTTPClient (WhatsApp API)  
- NTP time synchronisation  

Core logic is implemented in a **single Arduino sketch** for clarity and reproducibility.

---

## 🔁 Reproducibility Guide

### 🔌 Wiring

Most sensing and processing components are **integrated within the MR60FDA2 kit**.  
External wiring is limited to:

- Grove Buzzer  
- Power supply (USB)

📷 Wiring diagram available in `/media/wiring_diagram.png`

---

### 📍 Pin Mapping

| Component | Pin |
|---------|-----|
| Grove Buzzer | GPIO (external) |
| RGB LED | Integrated |
| mmWave Radar | Integrated |
| BH1750 | Integrated |

---

### ⚙️ Installation Parameters

| Parameter | Typical Value |
|---------|---------------|
| Mounting height | ~2.5 m |
| Detection threshold | Configurable |
| Sensitivity | Medium |
| Alert mode | One-time per fall |

---

### ▶️ Setup Order

1. Mount device on ceiling  
2. Power via USB  
3. Configure Wi-Fi and MQTT credentials  
4. Set installation height and detection threshold  
5. Deploy system and observe LED state  
6. Test fall detection scenarios  

---

## 🧪 Evaluation Summary

Testing was conducted in a **room-scale indoor environment** under three scenarios:

1. No occupant present  
2. Normal movement (walking, standing, sitting)  
3. Simulated fall events  

### Key Observations
- Immediate fall detection and alert triggering  
- Stable MQTT publishing  
- No false positives during normal movement  
- Sensitivity to threshold configuration  
- Limited discrimination between falls and slow postural transitions  

📺 A demonstration video is available via the YouTube link in `/media/`.

---

## 🔍 Reflection & Limitations

- Detection behaviour is sensitive to threshold configuration  
- Proprietary radar firmware limits access to raw motion data  
- The system cannot reliably distinguish intentional lying from real falls  
- Design constraints reflect real-world IoT deployment trade-offs  

The project intentionally focuses on **system-level design**, prioritising:
- deployment realism  
- privacy preservation  
- robustness  
- reproducibility  

---

## 🚀 Future Work

- Learning-based fall classification using temporal motion features  
- Access to raw mmWave data for improved semantic accuracy  
- Multi-sensor fusion (e.g. inertial validation)  
- Battery-powered operation  
- Multi-room monitoring dashboard  

---

## 📂 Repository Structure

fall-detection/
├── code/ # Arduino sketches
├── enclosure/ # Fusion 360 and STL files
├── media/ # Diagrams, photos, videos
├── docs/ # Additional documentation
└── README.md


---

## 📄 Coursework Context

- **Module:** CASA0016 – Making, Designing & Building Connected Sensor Systems  
- **Institution:** UCL Centre for Advanced Spatial Analysis, MSc Connected Environments 
- **Author:** Gilang Pamungkas  
- **Date:** December 2025  

📘 The full technical report accompanies this repository.

---

## 📚 References

See the reference list in the accompanying coursework report.
