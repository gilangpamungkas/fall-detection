# mmWave-Based Indoor Fall Detection System  
**CASA0016 – Making, Designing & Building Connected Sensor Systems**

## Overview

This repository documents the design, implementation, and evaluation of an **indoor fall detection prototype** using a **60 GHz mmWave radar sensor (MR60FDA2)** integrated with a **Seeed Studio XIAO ESP32C6** microcontroller. The project explores how edge-based sensing, lightweight communication protocols, and simple alert mechanisms can be combined to support safety monitoring in connected indoor environments.

The system detects human presence and fall events using embedded radar firmware and provides **local alerts (RGB LED and buzzer)** as well as **remote notifications via WhatsApp**, without transmitting raw sensor data.

> ⚠️ This prototype is **not a medical device** and is intended for educational and exploratory purposes only.

---

## System Architecture

The system follows an **edge-centric, event-driven architecture**:

### Sensing & Classification (Edge)
- MR60FDA2 mmWave radar performs proprietary signal processing and fall detection internally
- ESP32C6 retrieves abstracted detection states (`no presence`, `presence`, `fall`) via the Seeed Arduino mmWave library
- Installation parameters (height, threshold, sensitivity) are configured in software to align with physical setup

### Local Feedback
- WB2812B RGB LED provides visual state indication
- Piezo buzzer is activated only when a fall is detected

### Communication & Alerts
- MQTT publishes lightweight status messages for external logging or dashboards
- WhatsApp alerts are sent via the TextMeBot HTTP API
- NTP time synchronisation is used to timestamp alerts
- One-time alert logic prevents repeated notifications for the same fall event

This layered design separates sensing, decision logic, communication, and user feedback, enabling future extensions without redesigning the core system.

---

## Hardware Components

<img src="images/hardware_setup.jpg" 
     alt="Hardware setup: MR60FDA2 mmWave sensor with ESP32C6" 
     width="600"/>

- MR60FDA2 60 GHz mmWave Fall Detection Module  
- Seeed Studio XIAO ESP32C6  
- WB2812B RGB LED  
- Piezo buzzer  
- BH1750 ambient light sensor (contextual data only)  
- 5V USB power supply (always-on indoor deployment)

The sensor is mounted on the ceiling at approximately **2.5 metres** in a room-scale indoor environment.

---

## Software Stack

- Arduino IDE  
- Seeed Arduino mmWave Library  
- Adafruit NeoPixel Library  
- hp_BH1750 Library  
- WiFi & PubSubClient (MQTT)  
- HTTPClient (WhatsApp API)  
- NTP time synchronisation  

Core logic is implemented in a single Arduino sketch for clarity and reproducibility.

---

## Evaluation Summary

The system was evaluated through **controlled indoor room-scale testing** across three scenarios:

1. No occupant present  
2. Normal human movement (walking, standing, sitting)  
3. Simulated fall events (rapid posture change)

Key observations:
- Fall events triggered immediate local alerts and near real-time WhatsApp notifications
- MQTT status publishing remained stable throughout testing
- No false positives during normal walking or standing
- Detection behaviour was sensitive to threshold configuration

Evaluation focused on **functional behaviour and system integration**, not clinical or statistical accuracy.

---

## Reflection & Limitations

Several limitations were identified during testing:

- **Threshold sensitivity**: High threshold values caused false positives during abrupt but non-hazardous actions (e.g. sitting quickly)
- **Semantic ambiguity**: The system cannot reliably distinguish between real falls and slow, intentional transitions to a lying posture
- **Closed firmware**: Proprietary radar algorithms limit access to raw motion data and advanced customisation

These constraints reflect trade-offs inherent in commercially available, rule-based mmWave sensing modules.

Recent research (e.g. Huh et al., 2025) demonstrates that **machine-learning-based analysis of mmWave motion data** can significantly improve discrimination between falls and non-fall activities. However, such approaches require access to richer data representations and labelled datasets, which are beyond the scope of this prototype.

---

## Future Work

Potential extensions include:
- Learning-based fall classification using temporal motion features  
- Multi-sensor fusion (e.g. inertial or vision-based validation)  
- Multi-device monitoring dashboard via MQTT  
- Battery-powered operation and energy optimisation  
- Environment-aware calibration and configuration tools  

---

## Repository Structure

