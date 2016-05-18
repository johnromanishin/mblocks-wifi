#define USE_WIFICON

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
const char WiFiSSID[] = "TP-LINK_9B88A0";
const char WiFiPSK[] = "279B88A0";

int status = WL_IDLE_STATUS;
IPAddress ip_address;

//////////////////////
// ROS Definitions  //
//////////////////////
ros::NodeHandle nh;

mblocks_wifi::Status status_msg;
std_msgs::String serial_msg;

ros::Publisher pub_stat("cube1/status", &status_msg);
ros::Publisher pub_ser("cube1/serial", &serial_msg);

//////////////////////
// Global variables //
//////////////////////
String mac_address;
String ser_str;

long publisher_timer;

// Main program begins here
void setup() 
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID, WiFiPSK);

  Wire.begin();
  
  delay(10000);
  
  nh.initNode("192.168.0.103");
  nh.advertise(pub_stat);
  nh.advertise(pub_ser);
}

void loop()
{ 
  nh.spinOnce();
}

void publishStatus() {
  status_msg.magnet_sense = readMagnet();
  for (int i = 1; i <= 6; ++i) {
    status_msg.face_light[i - 1] = readLightSensor(i);
  }
  pub_stat.publish(&status_msg);
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


