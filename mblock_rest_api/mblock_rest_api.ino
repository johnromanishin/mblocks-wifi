#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int LED_PIN = 5;
const int ANALOG_PIN = A0;
const int DIGITAL_PIN = 12;

int magSensorAddress = 0x40;

const char *ssid = "StataCenter"
const char *password = "";
const char *mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
  delay();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  Serial.println();

  for (int i = 0; i < length; ++i) {
    Serial.print((char)payload[i]);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);
  
  delay(1000);
  Serial.begin(115200);
  Wire.begin();

  setupWifi();
  client.setServer(mqtt_server, 1833);
  client.setCallback(callback);
}

long publisher_timer;

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe("send_cmd");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() > publisher_timer) {
    Wire.requestFrom(magSensorAddress, 2);
    if (2 <= Wire.available()) {
      byte msb;
      byte lsb;
      int magReading;

      msb = Wire.read();
      lsb = Wire.read();
      magReading = (msb << 6) | lsb;
      Serial.println(magReading);

      client.publish("magnet", msg);
    }

    publisher_timer = millis() + 1000;
  }
}
