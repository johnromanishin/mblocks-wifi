
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
long reset_timer;

// Main program begins here
void setup() 
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WiFiSSID);
    status = WiFi.begin(WiFiSSID, WiFiPSK);

    delay(5000);
  }
  
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFiSSID);
  
  ip_address = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip_address);

  delay(1000);
  
  Wire.begin(2, 14);

  delay(1000);
  
  pub_stat = new ros::Publisher("cube1/status", &status_msg);
  pub_ser = new ros::Publisher("cube1/serial", &serial_msg);

  nh.initNode("192.168.0.102");
  nh.advertise(*pub_stat);
  nh.advertise(*pub_ser);
}

void loop()
{
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

int readLightSensor(int i) {
  Wire.beginTransmission(byte(i));
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.requestFrom(i, 2);

  int reading = 0;
  int reading1 = 0;
  if (2 <= Wire.available()) {
    reading = Wire.read();
    reading = reading << 2;
    reading1 = Wire.read();
    reading1 = reading1 >> 6;
  }

  return reading | reading1;
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


