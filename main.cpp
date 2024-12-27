#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "APX"; // <-- Put the SSID here
const char* password = "3uae#smo"; // <-- Put the WiFi Password here

// MQTT broker credentials
const char* mqtt_server = "192.168.x.x"; // Replace with your MQTT broker IP
const char* mqtt_username = "xxxx"; // <-- Put the Mosquito Broker Username
const char* mqtt_password = "xxxx"; // <-- Put the Mosquito Broker Password
const char* mqtt_topic_message = "alpha215/message";
const char* mqtt_topic_effect = "alpha215/effect";

// Default DATA Field values
byte msgg[] = {0x41, 0x1B, 0x20, 0x62}; // Default: Static display

// Track if a custom message has been received
bool messageReceived = false;

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Function to send a message to the Alpha 215R
void sendToDisplay(const String& message) {
  byte msga[] = {0x00, 0x00, 0x00, 0x00, 0x00}; // Serial Sync
  Serial.write(msga, sizeof(msga));
  byte msgb[] = {0x01}; // Start of Header
  Serial.write(msgb, sizeof(msgb));
  byte msgc[] = {0x65}; // Type Code (Default 7A, 65 for 215R specifically)
  Serial.write(msgc, sizeof(msgc));
  byte msgd[] = {0x30, 0x30}; // Sign address, 00 for broadcast
  Serial.write(msgd, sizeof(msgd));
  byte msge[] = {0x02}; // Start of Text
  Serial.write(msge, sizeof(msge));
  byte msgf[] = {0x41}; // Command code, 41 for WriteTEXT
  Serial.write(msgf, sizeof(msgf));
  Serial.write(msgg, sizeof(msgg)); // DATA Field
  Serial.print(message); // Custom message
  byte closemsg[] = {0x04}; // End of message
  Serial.write(closemsg, sizeof(closemsg));
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == mqtt_topic_message) {
    Serial.println("Received message: " + message);
    sendToDisplay(message);
    messageReceived = true;
  } else if (String(topic) == mqtt_topic_effect) {
    msgg[3] = (byte)strtol(message.c_str(), NULL, 16); // Update display mode (4th DATA byte)
    Serial.println("Updated effect: " + String(msgg[3], HEX));
  }
}

// Function to reconnect to the MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    sendToDisplay("Connecting to MQTT...");
    if (client.connect("ESP8266Alpha215", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      sendToDisplay("Welcome!");
      client.subscribe(mqtt_topic_message);
      client.subscribe(mqtt_topic_effect);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      sendToDisplay("MQTT Failed");
      delay(5000);
    }
  }
}

void setup() {
  // Initialize Serial for communication with Alpha 215R
  Serial.begin(9600, SERIAL_7E1); // Match the required serial settings

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    sendToDisplay("Connecting WiFi...");
  }

  String connectedMessage = "WiFi Connected";
  Serial.println(connectedMessage);
  sendToDisplay(connectedMessage);
  delay(2000);

  String ipMessage = "IP: " + WiFi.localIP().toString();
  Serial.println(ipMessage);
  sendToDisplay(ipMessage);
  delay(2000);

  // Set up MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // If no custom message has been received, display "Welcome!"
  if (!messageReceived) {
    sendToDisplay("Welcome!");
    delay(5000);
  }
}
