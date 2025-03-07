#include <Arduino.h>
// WiFi.h has WiFiClient.h, WiFiServer.h  included
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"
#include "char_arr_utils.h"
#include "http_handler.h"

void wait_for_few_seconds();
void init_WiFi();

HttpHandler http_handler;

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

  // TODO Need to add getting time from server
  static int charging_times_arr[ARR_LEN];
  static bool is_charging_arr[ARR_LEN];

  current_millis = millis();

  if (current_millis - prev_millis >= INTERVAL_GET_DATA_FROM_SERVER)
  {
    prev_millis = current_millis;
    int ret_val = http_handler.recv_charging_data(charging_times_arr,
                                    is_charging_arr,
                                    ARR_LEN,
                                    CHARGING_TIME,
                                    IS_CHARGING,
                                    SERVER_ADDRESS,
                                    CHARGING_DATA_ENDPOINT);
    if (ret_val == -1)
    {
      Serial.println("AAAEASEASEASEE xddddd");
    }
    else 
    {
      Serial.println(is_charging_arr[0]);
      Serial.println(charging_times_arr[0]);
    }
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