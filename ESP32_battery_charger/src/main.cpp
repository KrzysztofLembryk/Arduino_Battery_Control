#include <Arduino.h>
// WiFi.h has WiFiClient.h, WiFiServer.h  included
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"
#include "error_constants.h"
#include "char_arr_utils.h"
#include "http_handler.h"
#include "recv_func.h"

// ----------FUNCTIONS PRE-DECLARATION----------
void wait_for_few_seconds();
void init_WiFi();

// ----------GLOBAL VARIABLES----------
// HttpHandler http_handler;
// TimeHandler time_handler;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  // we give time to open the serial monitor - for DEVELOPMENT purposes
  wait_for_few_seconds();
  init_WiFi();
}

void loop()
{
  // ----------HANDLER CLASSES OBJECTS-----------
  static HttpHandler http_handler;
  static TimeHandler time_handler;

  // ----------ARRAYS----------
  static int charging_times_arr[ARR_LEN];
  static bool is_charging_arr[ARR_LEN];

  // ----------MEASURING TIME VARIABLES----------
  static unsigned long current_millis = 0;
  static unsigned long prev_millis = 0;
  static unsigned long time_to_next_event = 0;
  static unsigned long time_to_charge = 0;
  static unsigned long time_left_to_next_interval = 0;

  static constexpr int STATE_CHARGING = 1;
  static constexpr int STATE_WAITING = 2;
  static constexpr int STATE_CHARGING = 3;
  static int curr_state;

  // ----------CONTROL VARIABLES----------
  static int curr_interval_idx = -1;
  static bool is_charging = false;

  // ----------FUNCTIONS CALLED ONCE DURING FIRST LOOP----------
  // These could be in setup, but I would need to make two *_arr global 
  // variables, I don't want to make global variables if not really necessary
  // if (first_time)
  // {
  //   recv_curr_interval(http_handler, time_handler);

  //   curr_interval_idx = time_handler.get_curr_interval();
  //   time_till_next_interval = time_handler.get_time_till_next_interval();

  //   recv_charging_data(charging_times_arr,
  //                     is_charging_arr,
  //                     http_handler);
  // }

  // ----------MAIN PROGRAM LOOP----------
  current_millis = millis();

  if (current_millis - prev_millis >= time_to_next_event)
  {
    prev_millis = current_millis;

    if (curr_interval_idx == -1)
    {
      recv_charging_data(charging_times_arr,
                        is_charging_arr,
                        http_handler);
      recv_curr_interval(http_handler, time_handler);

      curr_interval_idx = time_handler.get_curr_interval();
      time_left_to_next_interval = time_handler.get_time_till_next_interval();
      time_to_charge = charging_times_arr[curr_interval_idx];

      // TO DO FUNKCJI
      if (time_left_to_next_interval < time_to_charge)
      {
        time_to_next_event = time_left_to_next_interval * INTERVAL_10_S;
        time_left_to_next_interval = 0;
        time_to_charge = 0;
        Serial.println("###############CHARGING START###############");
      }
      else 
      {
        if (time_to_charge == 0)
        {
          Serial.println("###############CHARGING STOP###############");
          time_to_next_event = time_left_to_next_interval * INTERVAL_10_S;
          time_left_to_next_interval = 0;
        }
        else
        {
          Serial.println("###############CHARGING START###############");
          time_left_to_next_interval -= time_to_charge;
          time_to_next_event = time_to_charge * INTERVAL_10_S;
          time_to_charge = 0;
        }
      }
    }
    else 
    {
      if (time_left_to_next_interval == 0)
      {
        curr_interval_idx = (curr_interval_idx + 1) % ARR_LEN;
        time_left_to_next_interval = INTERVAL_15_MIN;
        time_to_charge = charging_times_arr[curr_interval_idx];
      }
      else 
      {
        
      }
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