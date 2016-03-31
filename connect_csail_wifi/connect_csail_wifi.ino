/*
  Connect to data.sparkfun.com and download the last command.
 */


#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "StataCenter"; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Phant server data
char server[] = "data.sparkfun.com";    // name address for Google (using DNS)
char PublicKey[] = "KJ7ZLKad2jIgDaOXNZpq";
char PrivateKey[] = "vzrPJ8wdomT1vB742wlJ";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L;

String responseLine;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
 
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while(true);
  }
 
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
 
    // wait 10 seconds for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    if (c == '\n') {
      parseResponse(responseLine);
      responseLine = "";
    } else if (c != '\r') {
      responseLine += c;
    }
  }

  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

String lastTimestamp;
bool cmdNext = false;

void parseResponse(String line) {
  if (cmdNext) {
    int id = line.indexOf(',');
    Serial.println("CMD: " + line.substring(0, id));
    Serial.println("TIME: " + line.substring(id + 1));
    lastTimestamp = line.substring(id + 1);
    cmdNext = false;  
  }
  if (line.startsWith("cmd")) {
    cmdNext = true;
  } 
}

void httpRequest() {
  client.stop();
 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.print("GET /output/");
    client.print(PublicKey);
    client.print(".csv?limit=1");
    if (!lastTimestamp.equals("")) {
      client.print("&gt[timestamp]=");
      client.print(lastTimestamp);
    }
    client.println(" HTTP/1.1");
    client.println("Host: data.sparkfun.com");
    client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
  } else {
    Serial.println("connection failed");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
