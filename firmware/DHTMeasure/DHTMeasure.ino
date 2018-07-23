#include "DHT.h"
#include <ESP8266WiFi.h>

// DHT
#define DHTPIN D1
#define DHTTYPE DHT11
#define DHT_TIMEOUT 2000

// Wifi
#define PORT 3000
#define WIFIPIN D2
#define WIFI_TIMEOUT 5000

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

// WiFi Connection timeStapm
unsigned long conn_timeStamp = -1;
// Measure timeStamp
unsigned int msr_timeStamp = -1;

bool readDht(measure *msr) {
  float humi = dht.readHumidity();
  float cels = dht.readTemperature();
  float fahr = dht.readTemperature(true);
  
  msr->h = humi;
  msr->t = cels;
  msr->f = fahr;

  Serial.print("\n\t\n--------------------MEASURE------------------\n");
  Serial.print("Humidity: ");
  Serial.print(msr->h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(msr->t);
  Serial.print(" *C ");
  Serial.print(msr->f);
  Serial.print(" *F");
  Serial.print("\n---------------------------------------------\t\n\n");
  
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

  // URL for request
  String url = "/";

  // POST data
  String data = "humi=" + String(msr.h) + 
                "&cels=" + String(msr.t) + 
                "&fahr=" + String(msr.f);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

  digitalWrite(WIFIPIN, HIGH);
  // Sending the request to the server
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
    // Read all the lines of the reply from server and print them to Serial
    String line = client.readStringUntil('\r');
    Serial.print(line);
    
    if (millis() - timeout > WIFI_TIMEOUT) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      digitalWrite(WIFIPIN, LOW);
      return true;
    }
    digitalWrite(WIFIPIN, LOW);
  }
  
  Serial.println();
  Serial.println("closing connection");

  return true;
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(10);

  // Wifi indicator
  pinMode(WIFIPIN, OUTPUT);
  digitalWrite(WIFIPIN, LOW);

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
  if(msr_timeStamp == -1)
    msr_timeStamp = millis();
  
  if (millis() - msr_timeStamp > DHT_TIMEOUT) {
    msr_timeStamp = -1;
    
    if(!readDht(&msr))
      Serial.println("Measure was falled");  
  }

  if(conn_timeStamp == -1)
    conn_timeStamp = millis();
    
  if (millis() - conn_timeStamp > WIFI_TIMEOUT) {
    conn_timeStamp = -1;
    
    if(!conn())
      Serial.println("Connection was falled");
  }
}

