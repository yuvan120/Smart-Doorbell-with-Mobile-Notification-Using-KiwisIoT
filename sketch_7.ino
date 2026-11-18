#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Use the broker settings provided by KiwisIoT
const char* mqtt_server = "YOUR_MQTT_BROKER";
const int mqtt_port = YOUR_MQTT_PORT;

// Use your actual device topic and credentials
const char* mqtt_topic = "YOUR_DEVICE_TOPIC";

#define BUTTON_PIN 18
#define BUZZER_PIN 19
#define LED_PIN 21

WiFiClient espClient;
PubSubClient client(espClient);

bool lastButtonState = HIGH;
unsigned long lastPressTime = 0;

void connectWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    if (client.connect("SmartDoorbellESP32")) {
      break;
    }
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH &&
      buttonState == LOW &&
      millis() - lastPressTime > 1000) {

    lastPressTime = millis();

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);

    // Replace with the payload required by your
    // KiwisIoT device integration.
    client.publish(mqtt_topic, "DOORBELL_PRESSED");

    Serial.println("Doorbell Pressed!");

    delay(500);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  lastButtonState = buttonState;
}