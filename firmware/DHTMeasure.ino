/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */ 
#include "DHT.h"
#include <ESP8266WiFi.h>

#define DHTPIN D1
#define DHTTYPE DHT11
#define PORT 3000

DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = "Yamaha_R1";
const char* password = "448$$katakata$$";
const char* host = "192.168.1.9";

struct measure {
  float h = 0.0;
  float t = 0.0;
  float f = 0.0;
};

measure msr;

// WiFi Connection delay Counter
unsigned int connVal = 0;
// Measure delay Counter
unsigned int msrVal = 0;

bool readDht(measure *msr) {
  float humi = dht.readHumidity();
  float cels = dht.readTemperature();
  float fahr = dht.readTemperature(true);
  
  /*if(isnan(humi) || isnan(cels) || isnan(fahr))
    return false;*/
  
  msr->h = humi;
  msr->t = cels;
  msr->f = fahr;
  
  return true;
}

bool conn() {
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int port = PORT;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return false;
  }
  
  // We now create a URI for the request
  String url = "/";
 /* 
  url += "?humi=";
  url += msr.h;
  url += "&cels=";
  url += msr.t;
  url += "&fahr=";
  url += msr.f; 
 */

  // POST data
  String data = "humi=" + String(msr.h) + 
                "&cels=" + String(msr.t) + 
                "&fahr=" + String(msr.f);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

  digitalWrite(D2, HIGH);
  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Accept: */*\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + data.length() + "\r\n" +
               //"\r\n");
               "Connection: close\r\n\r\n");
  client.println(data);             
                           
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      digitalWrite(D2, LOW);
      return true;
    }
    digitalWrite(D2, LOW);
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  return true;
}
  
void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(10);

  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(msrVal++ == 2000) {
    if(!readDht(&msr))
      Serial.println("Measure was falled");

    Serial.print("\n\t\n--------------------MEASURE------------------\n");
    Serial.print("Humidity: ");
    Serial.print(msr.h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(msr.t);
    Serial.print(" *C ");
    Serial.print(msr.f);
    Serial.print(" *F");
    Serial.print("\n---------------------------------------------\t\n\n");
      
    msrVal = 0;  
  }

  connVal++;
  if(connVal == 5000) {
    if(!conn())
      Serial.println("Connection was falled");
    
    connVal = 0;
  }
}

