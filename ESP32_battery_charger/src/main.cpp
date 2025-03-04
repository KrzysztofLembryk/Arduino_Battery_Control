#include <Arduino.h>
// WiFi.h has WiFiClient.h, WiFiServer.h  included
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"

void init_WiFi();
void handle_comm_with_server(WiFiClient &client, HTTPClient &http);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  // purpose of this for loop is to have a few seconds to open Serial Monitor
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    // flush() ensures that all data is sent to the hardware before proceeding
    Serial.flush();
    delay(1000);
  }

  init_WiFi();
}

void loop()
{
  WiFiClient client;
  HTTPClient http;

  while (true)
  {
    handle_comm_with_server(client, http);
    delay(10000);
  }
}

void handle_comm_with_server(WiFiClient &client, HTTPClient &http)
{
    if (http.begin(client, SERVER_ADDRESS))
    { 
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          JsonDocument doc;
          const String recv_data = http.getString();
          DeserializationError json_error = deserializeJson(doc, recv_data);

          if (json_error) 
          {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(json_error.f_str());
          }
          else 
          {
            JsonArray charging_times_arr = doc[CHARGING_TIME].as<JsonArray>();
            JsonArray is_charging_arr = doc[IS_CHARGING].as<JsonArray>();
          }
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
    else
    {
      Serial.println("[HTTP] Unable to connect");
    }
}

void init_WiFi()
{
  Serial.print("\nConnecting to " + String(NETWORK_SSID) + '\n');

  // WIFI_STA - Station mode  meaning we will connect to an existing network
  // WIFI_AP - Access point mode (host)
  WiFi.mode(WIFI_STA);
  WiFi.begin(NETWORK_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("IP address: " + String(WiFi.localIP()));

  // We try to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}