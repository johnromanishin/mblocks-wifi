
#define USE_WIFICON

#include <ArduinoHardware.h>
#include <ros.h>

#include <mblocks_wifi/Status.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>

#include <ESP8266WiFi.h>
#include <Wire.h>

//////////////////////
// Constants        //
//////////////////////
const int LED_PIN = 5;
const int ANALOG_PIN = A0;
const int DIGITAL_PIN = 12;

//////////////////////
// WiFi Definitions //
//////////////////////
//const char WiFiSSID[] = "TP-LINK_9B88A0"; // Sebastian's router
//const char WiFiPSK[] = "279B88A0";// Sebastian's router
const char WiFiSSID[] = "TP-LINK_9B7022"; // My Router
const char WiFiPSK[] = "279B7022"; // My Router

int status = WL_IDLE_STATUS;
IPAddress ip_address;

//////////////////////
// ROS Definitions  //
//////////////////////
ros::NodeHandle nh;

mblocks_wifi::Status status_msg;
std_msgs::String serial_msg;

ros::Publisher *pub_stat;
ros::Publisher *pub_ser;

//////////////////////
// Global variables //
//////////////////////
String mac_address;
String ser_str;

long publisher_timer;

// Main program begins here
void setupWiFi()
{
  WiFi.mode(WIFI_STA);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WiFiSSID);
    status = WiFi.begin(WiFiSSID, WiFiPSK);

    delay(1000);
  }
  
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFiSSID);
  
  ip_address = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip_address);
}

void setup() 
{
  Serial.begin(115200);
  Wire.begin(2, 14);

  setupWiFi();

  delay(2000);

  pub_stat = new ros::Publisher("cube1/status", &status_msg);
  pub_ser = new ros::Publisher("cube1/serial", &serial_msg);

  nh.initNode("192.168.0.102");
  nh.advertise(*pub_stat);
  nh.advertise(*pub_ser);

  delay(1000);
}

void loop()
{
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c != '\n') {
      ser_str += String(c);
    } else {
      serial_msg.data = ser_str.c_str();
      pub_ser->publish(&serial_msg);
      ser_str = "";
    }
  }
  
  if (millis() > publisher_timer) { 
    publishStatus();    
    publisher_timer = millis() + 1000;
  }
  
  nh.spinOnce();
}

void publishStatus() {
  status_msg.magnet_sense = readMagnet();
  for (int i = 1; i <= 6; ++i) {
    status_msg.face_light[i - 1] = readLightSensor(i);
  }
  pub_stat->publish(&status_msg);
}

String readMacAddress() {
  Serial.println("blemac");
  
  while (Serial.available() < 12);

  String mac;
  for (int i = 0; i < 12; ++i) {
    char c = Serial.read();
    mac += String(c);
  }

  return mac;
}

int readLightSensor(int i) {
  Wire.beginTransmission(byte(i));
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.requestFrom(i, 2);

  int reading = 0;
  if (2 <= Wire.available()) {
    reading = Wire.read() << 2;
    reading |= Wire.read() >> 6;
  }

  return reading;
}

int readMagnet() {
  Wire.beginTransmission(0x40);
  Wire.write(byte(255));
  Wire.endTransmission();
  Wire.requestFrom(0x40, 2);

  int reading = 0;
  if (2 <= Wire.available()) {
    reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();
  }

  return reading;
}
