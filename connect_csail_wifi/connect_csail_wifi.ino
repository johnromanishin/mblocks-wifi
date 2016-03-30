//I changed the digital to 8 and now can tell that it posted online!

// Include the ESP8266 WiFi library. (Works a lot like the
// Arduino WiFi library.)
#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "StataCenter";
const char WiFiPSK[] = "";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 5; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

////////////////
// Phant Keys //
////////////////
const char PhantHost[] = "data.sparkfun.com";
const char PublicKey[] = "KJ7ZLKad2jIgDaOXNZpq";
const char PrivateKey[] = "vzrPJ8wdomT1vB742wlJ";

const int httpPort = 80;

/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 60000;
unsigned long lastPost = 0;

void setup() 
{
  initHardware(); // Setup input/output I/O pins
  connectWiFi(); // Connect to WiFi
  digitalWrite(LED_PIN, LOW); // LED on to indicate connect success
}

void loop() 
{
  // This conditional will execute every lastPost milliseconds
  // (assuming the Phant post succeeded).
  if ((lastPost + postRate <= millis()) || lastPost == 0)
  {
    Serial.println("Posting to Phant!");
    if (getFromPhant())
    {
      lastPost = millis();
    }
    else // If the Phant post failed
    {
      delay(500); // Short delay, then try again
      Serial.println("GET failed. Will try again.");
    }
  }
}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initHardware()
{
  Serial.begin(9600);
  pinMode(DIGITAL_PIN, INPUT_PULLUP); // Setup an input to read
  pinMode(LED_PIN, OUTPUT); // Set LED as output
  digitalWrite(LED_PIN, HIGH); // LED off
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

int getFromPhant()
{
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(LED_PIN, LOW);

  WiFiClient client;
  if (!client.connect(PhantHost, httpPort)) {
      return 0;
  }
  
  client.print("GET /output/");
  client.print(PublicKey);
  client.print(".csv");
  client.print("?limit=1");
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(PhantHost);
  client.println("Connection: close");
  client.println();

  while (client.available()) {
      String line = client.readStringUntil('\r');
  }
  
  // Before we exit, turn the LED off.
  digitalWrite(LED_PIN, HIGH);

  return 1; // Return success
}

