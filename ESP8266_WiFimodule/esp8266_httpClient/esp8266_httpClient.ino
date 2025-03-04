/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
//#include <WiFi.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

const char *network_name = "RadCode";
const char *password = "maskopatol";

const uint32_t CONNECT_TIMEOUT = 10000;
unsigned long previousMillis = 0;
const long interval = 15000;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  // purpose of this for loop is to have a few seconds to open Serial Monitor
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    // flush() ensures that all data is sent to the hardware before proceeding
    Serial.flush();
    delay(1000);
  }

  init_WiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >=interval){
    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println("Configured SSID cannot be reached");
        break;
      case WL_CONNECTED:
        Serial.println("Connection successfully established");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }

  // delay(1000);
}

void do_http_communication()
{
  // wait for WiFi connection
  // if ((WiFiMulti.run(CONNECT_TIMEOUT) == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "https://webhook.site/e5256b1d-b786-46bd-8800-7238f90fc436")) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        // if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        //   String payload = http.getString();
        //   Serial.println(payload);
        // }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  // }
  // else 
  // {
  //   Serial.println("WiFi not connected!");
  // }
}

void init_WiFi()
{
  // WIFI_STA - sets ESP8266 to Station mode, means it connects to an existing
  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(network_name, password);
  
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  //The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // SSId - Name of the WiFi network, PASSWORD - Password of the WiFi network
  // WiFiMulti.addAP("RadCode", "maskopatol");
}

void print_ssid_rssi(int n)
{
if (n == 0) {
      Serial.println("no networks found");
  } 
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.flush();
      delay(10);
    }
  }
}