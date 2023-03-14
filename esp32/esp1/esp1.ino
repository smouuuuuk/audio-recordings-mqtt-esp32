#include <WiFi.h>
#include <PubSubClient.h>

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

int buttonInputSend = 13;
bool alreadyPressedSend = false;
int timeToReleaseSend = 0;

int buttonInputReceive = 12;
bool alreadyPressedReceive = false;
int timeToReleaseReceive = 0;

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

  pinMode(buttonInputSend, INPUT_PULLDOWN);
  pinMode(buttonInputReceive, INPUT_PULLDOWN);
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
    if (client.connect("ESP32Client-1", mqtt_user, mqtt_psswd)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp1/receiveData");
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
  if (digitalRead(buttonInputSend) && !alreadyPressedSend) {
    alreadyPressedSend = true;
    client.publish("esp1/sendData", "Send Message from ESP1");
    Serial.println("Message Sent to esp1/sendData");
  } else if (alreadyPressedSend && !timeToReleaseSend && !digitalRead(buttonInputSend)) {
    timeToReleaseSend = millis() + 500;
  } else if (alreadyPressedSend && timeToReleaseSend) {
    if (millis() >= timeToReleaseSend)  {
      alreadyPressedSend = false;
      timeToReleaseSend = 0;
    }
  }
  if (digitalRead(buttonInputReceive) && !alreadyPressedReceive) {
    alreadyPressedReceive = true;
    client.publish("esp1/receiveData", "Receive Message from ESP1");
    Serial.println("Message Sent to esp1/receiveData");
  } else if (alreadyPressedReceive && !timeToReleaseReceive && !digitalRead(buttonInputReceive)) {
    timeToReleaseReceive = millis() + 500;
  } else if (alreadyPressedReceive && timeToReleaseReceive) {
    if (millis() >= timeToReleaseReceive)  {
      alreadyPressedReceive = false;
      timeToReleaseReceive = 0;
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
