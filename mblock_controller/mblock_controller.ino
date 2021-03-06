#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>

#include <ESP8266WiFi.h>
#include <Wire.h>

#define USE_WIFICON

const int LED_PIN = 5;
const int ANALOG_PIN = A0;
const int DIGITAL_PIN = 12;

int magSensorAddress = 0x40;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "StataCenter";  // change this to local connection
const char WiFiPSK[] = "";

ros::NodeHandle nh;

void messageCb(const std_msgs::String& cmd_msg) {
  String ss = cmd_msg.data;
  
  Serial.println(ss);
}

std_msgs::Int32 mag_msg;
ros::Publisher pub_mag("magnet", &mag_msg);
ros::Subscriber<std_msgs::String> sub("send_cmd", &messageCb);

void setupWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSSID, WiFiPSK);

  Serial.println(WiFi.localIP());
}

void setup() 
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);
  
  delay(1000);
  Serial.begin(115200);
  Wire.begin();

  setupWiFi();

  // change this to connect to host name of computer
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_mag);
}

long publisher_timer;

void loop()
{
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

      mag_msg.data = magReading;
      pub_mag.publish(&mag_msg);
    }

    publisher_timer = millis() + 1000;
  }
  
  nh.spinOnce();
}
