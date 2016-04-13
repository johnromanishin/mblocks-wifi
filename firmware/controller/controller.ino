#define USE_WIFICON

#include <ros.h>

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

int magSensorAddress = 0x40;
int prevMagReading = -1;

bool cmd_ready = true;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "TP-LINK_9B88A0";  // change this to local connection
const char WiFiPSK[] = "279B88A0";

int status = WL_IDLE_STATUS;
IPAddress ip_address;

//////////////////////
// ROS Definitions  //
//////////////////////
ros::NodeHandle nh;

void commandIACb(const std_msgs::String& cmd_msg) {
  String ss = cmd_msg.data;
  
  Serial.println(ss);
}

void commandCPCb(const std_msgs::String& cmd_msg) {
  String ss = cmd_msg.data;
  
  Serial.println(ss);
}

std_msgs::Int32 mag_msg;
std_msgs::String serial_msg;

ros::Publisher pub_mag("cube1/mag", &mag_msg);
ros::Publisher pub_ser("cube1/serial", &serial_msg);

ros::Subscriber<std_msgs::String> sub_cmd_ia("cube1/cmd_ia", &commandIACb);
ros::Subscriber<std_msgs::String> sub_cmd_cp("cube1/cmd_cp", &commandCPCb);

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
  Wire.begin();
  setupWiFi();

  delay(2000);

  nh.initNode("192.168.0.103");
  nh.subscribe(sub_cmd_ia);
  nh.subscribe(sub_cmd_cp);
  nh.advertise(pub_mag);
  nh.advertise(pub_ser);

  delay(1000);
}

String ser_str;
long publisher_timer;

void loop()
{
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c != '\n') {
      ser_str += String(c);
    } else {
      serial_msg.data = ser_str.c_str();
      pub_ser.publish(&serial_msg);
      ser_str = "";
    }
  }
  
  if (millis() > publisher_timer) {
    publishMagnet();    
    publisher_timer = millis() + 50;
  }
  
  nh.spinOnce();
}

void publishMagnet() {
  mag_msg.data = readMagnet();
  pub_mag.publish(&mag_msg);
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

