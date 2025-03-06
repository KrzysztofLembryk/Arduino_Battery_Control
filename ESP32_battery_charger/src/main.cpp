#include <Arduino.h>
// WiFi.h has WiFiClient.h, WiFiServer.h  included
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"

void wait_for_few_seconds();
void init_WiFi();
void recv_charging_data_from_server(int *charging_times_arr,
                             bool *is_charging_arr);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  // we give time to open the serial monitor - for DEVELOPMENT purposes
  wait_for_few_seconds();

  init_WiFi();
}

void loop()
{
  static unsigned long current_millis = 0;
  static unsigned long prev_millis = 0;

  // TODO Need to add real time clock or getting time from server
  static int charging_times_arr[ARR_LEN];
  static bool is_charging_arr[ARR_LEN];

  current_millis = millis();

  if (current_millis - prev_millis >= INTERVAL_GET_DATA_FROM_SERVER)
  {
    prev_millis = current_millis;
    recv_charging_data_from_server(charging_times_arr,
                            is_charging_arr);

    Serial.println(is_charging_arr[0]);
    Serial.println(charging_times_arr[0]);
  }
  
}

void recv_charging_data_from_server(int *charging_times_arr,
                             bool *is_charging_arr)
{
  static WiFiClient client;
  static HTTPClient http;

  if (http.begin(client, SERVER_ADDRESS + CHARGING_DATA_ENDPOINT))
  {
    Serial.print("[HTTP] GET...\n");

    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      static String recv_data;
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        JsonDocument doc;
        recv_data = http.getString();
        Serial.println(recv_data.length());
        DeserializationError json_error = deserializeJson(doc, recv_data);

        if (json_error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(json_error.f_str());
        }
        else
        {
          for (int i = 0; i < ARR_LEN; i++)
          {
            charging_times_arr[i] = doc[CHARGING_TIME][i].as<int>();
            is_charging_arr[i] = doc[IS_CHARGING][i].as<bool>();
          }
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
  Serial.print("\nConnecting to ");
  Serial.println(NETWORK_SSID);

  // WIFI_STA - Station mode  meaning we will connect to an existing network
  // WIFI_AP - Access point mode (host)
  WiFi.mode(WIFI_STA);
  WiFi.begin(NETWORK_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // We try to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void wait_for_few_seconds()
{
  // purpose of this for loop is to have a few seconds to open Serial Monitor
  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    // flush() ensures that all data is sent to the hardware before proceeding
    Serial.flush();
    delay(1000);
  }
}