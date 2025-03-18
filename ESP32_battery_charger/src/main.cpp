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

void handle_battery_turn_on_off(unsigned long *time_to_next_event_millis,
                               int *time_left_to_next_interval,
                               int *time_to_charge);

int sync_with_server(int *curr_interval_idx,
                     int *time_left_to_next_interval,
                     int *time_to_charge,
                     int charging_times_arr[],
                     HttpHandler &http_handler,
                     TimeHandler &time_handler);

int do_battery_turn_on_off(int *curr_interval_idx,
                           unsigned long *time_to_next_event_millis,
                           int *time_left_to_next_interval,
                           int *time_to_charge,
                           int charging_times_arr[],
                           HttpHandler &http_handler,
                           TimeHandler &time_handler);

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
  // ----------MEASURING TIME VARIABLES----------
  static unsigned long current_millis = 0;
  static unsigned long prev_millis = 0;
  static unsigned long time_to_next_event_millis = 0;
  static int time_to_charge = 0;
  static int time_left_to_next_interval = 0;

  // ----------MAIN PROGRAM LOOP----------
  current_millis = millis();

  if (current_millis - prev_millis >= time_to_next_event_millis)
  {
    prev_millis = current_millis;

    handle_battery_turn_on_off(&time_to_next_event_millis,
                               &time_left_to_next_interval,
                               &time_to_charge);
  }
}

void handle_battery_turn_on_off(unsigned long *time_to_next_event_millis,
                               int *time_left_to_next_interval,
                               int *time_to_charge)
                               
{
  // ----------HANDLER CLASSES OBJECTS-----------
  static HttpHandler http_handler;
  static TimeHandler time_handler;

  // ----------ARRAYS----------
  static int charging_times_arr[NBR_OF_INTERVALS];

  // ----------FLOW CONTROL VARIABLES----------
  static bool first_loop = true;
  static int curr_interval_idx = -1;

  if (!first_loop)
  {
    do_battery_turn_on_off(&curr_interval_idx,
                           time_to_next_event_millis, time_left_to_next_interval,
                           time_to_charge, charging_times_arr, http_handler,
                           time_handler);
  }
  else
  {
    /**
     * During first loop we need to fetch data from the server to synchronise
     * our time with real time
     */
    first_loop = false;

    sync_with_server(&curr_interval_idx,
                     time_left_to_next_interval,
                     time_to_charge,
                     charging_times_arr,
                     http_handler,
                     time_handler);
    do_battery_turn_on_off(&curr_interval_idx,
                           time_to_next_event_millis,
                           time_left_to_next_interval,
                           time_to_charge,
                           charging_times_arr,
                           http_handler,
                           time_handler);
  }
}

int do_battery_turn_on_off(int *curr_interval_idx,
                           unsigned long *time_to_next_event_millis,
                           int *time_left_to_next_interval,
                           int *time_to_charge,
                           int charging_times_arr[],
                           HttpHandler &http_handler,
                           TimeHandler &time_handler)
{
  if (*time_left_to_next_interval == 0)
  {
    *curr_interval_idx = (*curr_interval_idx + 1) % ARR_LEN;

    if (*curr_interval_idx == SYNC_SERVER_0000_IDX ||
        *curr_interval_idx == SYNC_SERVER_0600_IDX ||
        *curr_interval_idx == SYNC_SERVER_1200_IDX ||
        *curr_interval_idx == SYNC_SERVER_1700_IDX ||
        *curr_interval_idx == SYNC_SERVER_2200_IDX)
    {
      sync_with_server(curr_interval_idx,
                       time_left_to_next_interval,
                       time_to_charge,
                       charging_times_arr,
                       http_handler,
                       time_handler);
    }
    else
    {
      // !!! INTERVAL_15MIN_MILLIS - is in miliseconds, whereas time_to_charge is in
      // minutes -- NEED TO CHANGE IT for whole alg to work
      *time_left_to_next_interval = 15;
      *time_to_charge = charging_times_arr[*curr_interval_idx];
    }
  }

  if (*time_left_to_next_interval < *time_to_charge)
  {
    *time_to_next_event_millis = *time_left_to_next_interval * INTERVAL_10S_MILLIS;
    *time_left_to_next_interval = 0;
    *time_to_charge = 0;
    Serial.println("###############CHARGING START###############");
  }
  else
  {
    if (*time_to_charge == 0)
    {
      Serial.println("###############CHARGING STOP###############");
      *time_to_next_event_millis = *time_left_to_next_interval * INTERVAL_10S_MILLIS;
      *time_left_to_next_interval = 0;
    }
    else
    {
      Serial.println("###############CHARGING START###############");
      *time_left_to_next_interval -= *time_to_charge;
      *time_to_next_event_millis = *time_to_charge * INTERVAL_10S_MILLIS;
      *time_to_charge = 0;
    }
  }
}

int sync_with_server(int *curr_interval_idx,
                     int *time_left_to_next_interval,
                     int *time_to_charge,
                     int charging_times_arr[],
                     HttpHandler &http_handler,
                     TimeHandler &time_handler)
{
  recv_charging_data(charging_times_arr, http_handler);
  recv_curr_interval(http_handler, time_handler);

  *curr_interval_idx = time_handler.get_curr_interval();
  *time_left_to_next_interval = time_handler.get_time_till_next_interval();
  *time_to_charge = charging_times_arr[*curr_interval_idx];
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