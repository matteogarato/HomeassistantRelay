#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ssid F("Redacted")
#define password F("Redacted")
#define RELAY_PIN 15
#define LED 2
#define mqttPort 1883
#define mqttServer "Redacted"
#define mqttUser "mqtt_user"
#define mqttPassword "Redacted"
#define mqttRelayStatusTopic "home/electrical/relay1/get"
const char *topic = "home/electrical/relay1/set";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("no wifi"));
    delay(200);
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe(topic);
}

void loop() {
  client.loop();
}

void callback(char *topic, byte *payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    if ((char)payload[i] == '1') {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED, LOW);
      Serial.println("Turn Off");
      client.publish(mqttRelayStatusTopic, "1");
    } else {
      digitalWrite(LED, HIGH);
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Turn On");
      client.publish(mqttRelayStatusTopic, "0");
    }
  }
}
