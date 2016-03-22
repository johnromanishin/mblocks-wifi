
#define ESP8266_LED 5
#define readings 6
//#include <SoftwareSerial.h>
//SoftwareSerial ESPserial (2, 14); // RX | TX
#include <Wire.h>


void setup() 
{
    pinMode(ESP8266_LED, OUTPUT);
    //Serial.begin(9600);     // communication with the host computer
    delay(1000);
    Serial.begin(115200);
    // Start the software serial for communication with the ESP8266
    //ESPserial.begin(115200);  
    Wire.begin();                // join i2c bus (address optional for master) 
    }

unsigned int reading = 0;
int reading1 = 0;

void loop()
{
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
//Serial.println("WTF BRO");
if (reading< 4000)
{
  Serial.println("fbrgbled rb tb 1 2 3 4");
}
if (reading >= 14000 && reading <= 14200)
{
  Serial.println("bldcspeed f 5000");
  delay(2000);
  Serial.println("bldcstop b");
}
if (reading> 4000)
{
  Serial.println("fbrgbled g tb 1 2 3 4");
}
//if reading
//Serial.println("fbrgbled rb tb 1 2 3 4");
//digitalWrite(ESP8266_LED, HIGH);
//delay(250);
//Serial.println("fbrgbled rb tb 1 2 3 4");
//delay(250);
//Serial.println("fbrgbled off tb 1 2 3 4");
}

//fbrgbled rb tb 1 2 3 4
