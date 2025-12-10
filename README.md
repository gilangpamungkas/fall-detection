🛡️ Fall Detection System for Elderly Using mmWave Radar & IoT Alerts

A privacy-safe, contactless fall-detection device built using the Seeed MR60FDA2 mmWave sensor, ESP32, MQTT telemetry, NeoPixel indicators, buzzer alert, and WhatsApp emergency notifications.
Designed to detect human presence, identify falls in real time, and notify caregivers instantly.

🚀 Features

mmWave radar fall detection (no camera, privacy-safe)

Real-time WhatsApp alert with timestamp

LED status indicator

Blue → No people

Green → Human detected

Red → Fall detected

Buzzer alarm when a fall occurs

MQTT JSON telemetry (status, lux)

Light sensor (BH1750) for environment monitoring

WiFi auto-reconnect + MQTT auto-reconnect

🧠 How It Works (Flow Overview)
mmWave Sensor → ESP32 Logic → Status Classification
          ↓
     ┌──────────┬───────────┬─────────────┐
     │          │           │             │
 LED Indicator  Buzzer   WhatsApp Alert   MQTT Telemetry


Sensor detects human / no human / fall

ESP32 triggers buzzer + LED changes

On fall: sends WhatsApp emergency message

Publishes status to MQTT topic:
student/CASA0014/luminaire/21

🧩 Hardware Used

ESP32 (ESP32-C6, ESP32-S3, or equivalent)

Seeed Studio MR60FDA2 mmWave Radar Module

BH1750 light sensor

NeoPixel LED (GRB)

Piezo buzzer

3D-printed enclosure (optional)

📁 Project Structure
/src
  ├── main.cpp         # Core fall detection + alert logic
  ├── arduino_secrets.h # WiFi & MQTT credentials
/design
  ├── enclosure.f3d    # Fusion 360 enclosure (optional)
  └── images/          # Demo photos, diagrams
README.md

📡 MQTT Payload Example
{
  "status": 2,
  "lux": 45.3
}


Status codes:
0 → EXIST_PEOPLE
1 → NO_PEOPLE
2 → PEOPLE_FALL

📲 WhatsApp Alert Example

A real-time message sent via TextMeBot API:

EMERGENCY ALERT: Fall detected in bathroom at 2025-12-09 21:45:12.
Immediate assistance may be required.

⚠️ Limitations

Requires stable WiFi for alerting

Accuracy depends on sensor height & placement

Uses third-party WhatsApp API (not official WhatsApp Business API)

Cannot assess condition after fall (only detects event)

📜 License

MIT License

🙌 Acknowledgements

UCL CASA0014 Connected Environments

Seeed Studio for MR60FDA2 mmWave library

TextMeBot API for WhatsApp messaging
