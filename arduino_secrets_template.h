#pragma once
/**
 * arduino_secrets_template.h (SAFE TO COMMIT)
 *
 * HOW TO USE
 * 1) Copy this file and rename the copy to: arduino_secrets.h
 * 2) Fill in your own Wi-Fi / MQTT / WhatsApp credentials
 * 3) Keep arduino_secrets.h PRIVATE (add to .gitignore)
 *
 * NOTE
 * - This template contains placeholders only.
 * - Do NOT commit real credentials to GitHub.
 */

/* ================================ Wi-Fi ================================== */
// Multiple networks are supported; the sketch will try them in order.
const int numNetworks = 1;

const char* ssids[] = {
  "YOUR_WIFI_SSID"
};

const char* passwords[] = {
  "YOUR_WIFI_PASSWORD"
};

/* ================================= MQTT ================================== */
#define MQTT_SERVER    "YOUR_MQTT_BROKER"
#define MQTT_PORT      1883

#define MQTT_USER      "YOUR_MQTT_USERNAME"
#define MQTT_PASS      "YOUR_MQTT_PASSWORD"

#define MQTT_CLIENT_ID "MMWAVE01"
#define MQTT_TOPIC     "student/CASA0016/gilangpamungkas"

/* ============================== NTP / TIME ================================ */
// Time is used to timestamp WhatsApp alerts.
//
// UK winter (GMT):  0, 0
// UK summer (BST):  3600, 0
#define NTP_GMT_OFFSET_SEC       0
#define NTP_DAYLIGHT_OFFSET_SEC  0

/* ========================== TextMeBot (WhatsApp) ========================== */
#define TEXTMEBOT_BASE_URL  "http://api.textmebot.com/send.php?"

// Recipient phone number (URL-encoded, include country code)
#define TEXTMEBOT_RECIPIENT "%2B1234567890"

// API key for TextMeBot
#define TEXTMEBOT_APIKEY    "YOUR_TEXTMEBOT_API_KEY"

// Message parts (URL-encoded recommended)
#define WHATSAPP_MESSAGE_PREFIX  "EMERGENCY%20ALERT%3A%20Fall%20detected%20at%20"
#define WHATSAPP_MESSAGE_SUFFIX  ".%20Immediate%20assistance%20may%20be%20required."
