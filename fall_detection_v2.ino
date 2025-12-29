/**
 * CASA0016 — mmWave-Based Indoor Fall Detection System
 * University College London | Making, Designing & Building Connected Sensor Systems
 *
 * This sketch implements an edge-based indoor fall detection prototype using:
 * - MR60FDA2 60 GHz mmWave radar sensor (proprietary fall detection firmware)
 * - Seeed Studio XIAO ESP32C6 microcontroller
 * - WS2812 RGB LED and piezo buzzer for local feedback
 * - MQTT for lightweight status publishing
 * - WhatsApp alerts via TextMeBot HTTP API
 *
 * Reproducibility & Configuration
 * -------------------------------
 * This project requires a user-provided configuration file:
 *   arduino_secrets.h
 *
 * A template file:
 *   arduino_secrets_template.h
 * is included in this repository. To reproduce the system:
 * 1. Copy the template file
 * 2. Rename it to arduino_secrets.h
 * 3. Populate it with your own credentials (Wi-Fi, MQTT, API keys)
 *
 * IMPORTANT:
 * - Do NOT commit arduino_secrets.h to version control
 * - This prototype is for academic and exploratory use only
 * - It is NOT a medical or safety-certified device
 *
 * Design Notes
 * ------------
 * - Fall detection logic runs in closed, vendor-provided firmware
 * - This sketch reads high-level detection states only
 * - Alert logic is event-driven and rate-limited (one alert per fall)
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <hp_BH1750.h>
#include "Seeed_Arduino_mmWave.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <time.h>

#include "arduino_secrets.h"  // User-provided secrets (see template)

/* ============================= Network clients ============================= */
WiFiClient wifiClient;
PubSubClient client(wifiClient);

/* =============================== GPIO settings ============================= */
#define BUZZER_PIN D0
#define LIGHT_GPIO D0  // Kept unchanged (forced LOW in loop; BH1750 used for logging only)

/* =============================== MQTT settings ============================= */
String mqtt_topic = MQTT_TOPIC;   // from arduino_secrets.h
static const char* MQTT_CLIENT = MQTT_CLIENT_ID;

/* ============================ mmWave sensor config ========================== */
#ifdef ESP32
  #include <HardwareSerial.h>
  HardwareSerial mmwaveSerial(0);     // UART channel used by mmWave module (ESP32)
#else
  #define mmwaveSerial Serial1
#endif

SEEED_MR60FDA2 mmWave;

// Tuneable parameters (match physical installation)
uint32_t sensitivity = 15;            // Higher values can increase responsiveness (and false alarms)
float height = 2.5f;                  // Mount height (metres)
float threshold = 0.6f;               // Detection threshold (tune carefully)

// Returned radar parameters (optional debug)
float rect_XL, rect_XR, rect_ZF, rect_ZB;

/* ============================ Light sensor + LED ============================ */
hp_BH1750 BH1750;
Adafruit_NeoPixel pixels(1, D1, NEO_GRB + NEO_KHZ800);

/* ============================== System state =============================== */
typedef enum {
  EXIST_PEOPLE = 0,
  NO_PEOPLE    = 1,
  PEOPLE_FALL  = 2,
} MMMWAVE_STATUS;

MMMWAVE_STATUS status = NO_PEOPLE;
MMMWAVE_STATUS last_status = NO_PEOPLE;

float lux = 100.0f;
bool doingFallBuzz = false;

/* =============================== Wi-Fi helper ============================== */
/**
 * Connects to the first available Wi-Fi network listed in arduino_secrets.h.
 * Retries each SSID for ~10 seconds.
 */
void startWifi() {
  Serial.println("Starting WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(200);

  for (int i = 0; i < numNetworks; i++) {
    Serial.print("Trying SSID: ");
    Serial.println(ssids[i]);

    WiFi.begin(ssids[i], passwords[i]);
    const unsigned long startAttempt = millis();

    while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < 10000) {
      Serial.print(".");
      delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      return;
    }

    Serial.println("\nFailed. Trying next...");
    WiFi.disconnect(true);
    delay(200);
  }

  Serial.println("Could not connect to any WiFi.");
}

/* =============================== MQTT helper =============================== */
/**
 * Connects/reconnects to MQTT broker using credentials from arduino_secrets.h.
 * Blocks until connected.
 */
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

/* ============================ WhatsApp notification ========================= */
/**
 * Sends an alert message via TextMeBot (WhatsApp).
 * Secrets (recipient, API key, base URL, message prefix/suffix) live in arduino_secrets.h.
 */
void sendWhatsAppAlert() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WhatsApp] WiFi not connected!");
    return;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[WhatsApp] Failed to obtain time!");
    return;
  }

  char timeString[40];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);

  // Build URL-encoded message using configurable prefix/suffix
  String message = WHATSAPP_MESSAGE_PREFIX;
  message += String(timeString);
  message += WHATSAPP_MESSAGE_SUFFIX;

  // Minimal extra encoding (kept consistent with your original behaviour)
  message.replace(" ", "%20");
  message.replace(":", "%3A");

  // Build request URL from secrets
  String url = String(TEXTMEBOT_BASE_URL);
  url += "recipient=" + String(TEXTMEBOT_RECIPIENT);
  url += "&apikey=" + String(TEXTMEBOT_APIKEY);
  url += "&text=" + message;

  Serial.println("[WhatsApp] Sending alert...");
  // Note: printing the full URL may reveal secrets in Serial logs.
  // Uncomment only when debugging locally.
  // Serial.println(url);

  HTTPClient http;
  http.begin(url);
  const int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("[WhatsApp] Response: %d\n", httpCode);
    Serial.println(http.getString());
  } else {
    Serial.printf("[WhatsApp] Error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

/* =========================== Local feedback helpers ========================= */
void updateLedForStatus(MMMWAVE_STATUS s) {
  if (s == NO_PEOPLE)    pixels.setPixelColor(0, pixels.Color(0, 0, 255));   // Blue
  if (s == EXIST_PEOPLE) pixels.setPixelColor(0, pixels.Color(0, 255, 0));   // Green
  if (s == PEOPLE_FALL)  pixels.setPixelColor(0, pixels.Color(255, 0, 0));   // Red
  pixels.show();
}

void startFallBuzzer() {
  tone(BUZZER_PIN, 700);
  doingFallBuzz = true;
}

void stopFallBuzzer() {
  noTone(BUZZER_PIN);
  doingFallBuzz = false;
}

/* =================================== Setup ================================== */
void setup() {
  Serial.begin(115200);

  // ---- mmWave module init + configuration ----
  mmWave.begin(&mmwaveSerial);
  mmWave.setUserLog(0);
  mmWave.setInstallationHeight(height);
  mmWave.setThreshold(threshold);
  mmWave.setSensitivity(sensitivity);
  mmWave.getRadarParameters(height, threshold, sensitivity, rect_XL, rect_XR, rect_ZF, rect_ZB);

  // ---- Buzzer ----
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // ---- Network ----
  startWifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  reconnectMQTT();

  // ---- NTP time ----
  // Offsets (including daylight) are configured in arduino_secrets.h
  configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, "pool.ntp.org", "time.nist.gov");

  // ---- LED ----
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(8);
  pixels.setPixelColor(0, pixels.Color(125, 125, 125));  // boot indicator
  pixels.show();

  // ---- BH1750 (context only) ----
  BH1750.begin(BH1750_TO_GROUND);
  BH1750.calibrateTiming();
  BH1750.start(BH1750_QUALITY_HIGH2, 254);

  // ---- LIGHT_GPIO ----
  pinMode(LIGHT_GPIO, OUTPUT);
}

/* ==================================== Loop ================================== */
void loop() {
  // Maintain connectivity
  if (WiFi.status() != WL_CONNECTED) startWifi();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // -------------------- Read mmWave abstracted states --------------------
  if (mmWave.update(100)) {
    bool is_human = false;
    bool is_fall = false;

    mmWave.getHuman(is_human);
    mmWave.getFall(is_fall);

    if (!is_human && !is_fall) status = NO_PEOPLE;
    else if (is_fall)          status = PEOPLE_FALL;
    else                       status = EXIST_PEOPLE;
  }

  // ---------------------------- Serial logging ---------------------------
  switch (status) {
    case NO_PEOPLE:    Serial.println("Waiting for people"); break;
    case EXIST_PEOPLE: Serial.println("PEOPLE detected");    break;
    case PEOPLE_FALL:  Serial.println("FALL detected");      break;
    default: break;
  }

  // ------------------ One-time WhatsApp alert per fall -------------------
  static bool fallAlertSent = false;
  if (status == PEOPLE_FALL && !fallAlertSent) {
    sendWhatsAppAlert();
    fallAlertSent = true;
  }
  if (status != PEOPLE_FALL) {
    fallAlertSent = false;
  }

  // ------------------ Update LED + buzzer on state change ----------------
  if (status != last_status) {
    updateLedForStatus(status);

    if (status == PEOPLE_FALL) startFallBuzzer();
    if (status != PEOPLE_FALL && doingFallBuzz) stopFallBuzzer();

    last_status = status;
  }

  // ---------------------- Read BH1750 (informational) ---------------------
  if (BH1750.hasValue()) {
    lux = BH1750.getLux();
    BH1750.start(BH1750_QUALITY_HIGH2, 254);
  }
  Serial.print("LUX: ");
  Serial.println(lux);

  // Keep original behaviour: no darkness-triggered action
  digitalWrite(LIGHT_GPIO, LOW);

  // --------------------------- MQTT status publish ------------------------
  char payload[64];
  snprintf(payload, sizeof(payload), "{\"status\":%d,\"lux\":%.1f}", status, lux);
  client.publish(mqtt_topic.c_str(), payload);
}
