#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include "secrets.h"

// Replace the next variables with your SSID/Password combination
const char* ssid = SECRET_SSID;
const char* password = SECRET_WIFI_PASS;

// Add your MQTT Broker IP address, example:
const char* mqtt_server = SECRET_MQTT_SERVER;

const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_psswd = SECRET_MQTT_PASS;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int buttonInput = 13;
bool alreadyPressed = false;
int timeToRelease = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(buttonInput, INPUT_PULLDOWN);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_psswd)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("testTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void checkIfButtonPressed(){
  if (digitalRead(buttonInput) && !alreadyPressed) {
    alreadyPressed = true;
    client.publish("esp1/testTopic", "Test Message from ESP1");
    Serial.println("Message Sent to esp1/testTopic");
  } else if (alreadyPressed && !timeToRelease) {
    timeToRelease = millis() + 500;
  } else if (alreadyPressed && timeToRelease) {
    if (millis() >= timeToRelease)  {
      alreadyPressed = false;
      timeToRelease = 0;
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  checkIfButtonPressed();  
  
}
