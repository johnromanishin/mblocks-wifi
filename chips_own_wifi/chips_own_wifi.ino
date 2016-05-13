#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

#define ESP8266_LED 5
#define readings 6
#include <Wire.h>

WiFiServer server(80);

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
  pinMode(ESP8266_LED, OUTPUT);
  delay(1000);
  Serial.begin(115200);
  Wire.begin();                // join i2c bus (address optional for master) 
}

unsigned int reading = 0;
int reading1 = 0;

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);
  Wire.beginTransmission(0x40); // transmit to device #64
  Wire.write(byte(255));      // sets register pointer to echo #1 register
  Wire.endTransmission();      // stop transmitting
  
   // step 4: request readingfrom sensor
   Wire.requestFrom(0x40, 2);     // request 2 bytes from slave device #112
  
   // step 5: receive reading from sensor
   if(2 <= Wire.available())    // if two bytes were received
   {
     reading = Wire.read();  // receive high byte (overwrites previous reading)
     //Serial.println(reading);   // print the reading
     Serial.println(", ");
     reading = reading << 6;    // shift high byte to be high 8 bits
     reading |= Wire.read(); // receive low byte as lower 8 bits
     Serial.println(reading);   // print the reading
   }
   Serial.println(reading);   // print the reading
  delay(1000); // wait a bit since people have to read the output :)
  Serial.println("vbat");
  int incomingByte = 0; 
  char inData[1000]; // Allocate some space for the string
  int index = 0;
  while (Serial.available() > 0) {
    if (index <1000){
          // read the incoming byte:
          char incomingByte = Serial.read();
          inData[index] = incomingByte;
          index++;
          inData[index] = '\0'; //null terminate string
          // say what you got:
  
    }
  }
  
  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"high":"low";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:
    s += "Analog Pin = ";
    s += String(analogRead(ANALOG_PIN));
    s += "<br>"; // Go to the next line.
    s += "Digital Pin 12 = ";
    s += String(digitalRead(DIGITAL_PIN));
    s += "<br>"; // Go to the next line.
    s += "battery stuff = ";
    s += String(inData);
  }
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
  
  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;
  
  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, AP_NameString.length() + 1, 0);
  
  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);
  
  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
