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

void commandCb(const std_msgs::String& cmd_msg) {
  String ss = cmd_msg.data;
  
  Serial.println(ss);
}

std_msgs::Int32 mag_msg;
std_msgs::String chatter_msg;

ros::Publisher pub_mag("mag", &mag_msg);
ros::Publisher pub_chatter("chatter", &chatter_msg);
ros::Subscriber<std_msgs::String> sub_cmd("cmd_ia", &commandCb);

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
  
  // change this to connect to host name of computer
  nh.initNode("192.168.0.100");
  nh.subscribe(sub_cmd);
  nh.advertise(pub_mag);
  nh.advertise(pub_chatter);
}

long publisher_timer;

void loop()
{
  if (millis() > publisher_timer) {
    chatter_msg.data = "Hello World!";
    pub_chatter.publish(&chatter_msg);

    publishMagnet();
    
    publisher_timer = millis() + 50;
  }
  
  nh.spinOnce();
}

void publishMagnet() {
  Wire.beginTransmission(0x40);
  Wire.write(byte(255));
  Wire.endTransmission();

  Wire.requestFrom(0x40, 2);

  if (2 <= Wire.available()) {
    int reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();

    mag_msg.data = reading;
    pub_mag.publish(&mag_msg);
  }
}

