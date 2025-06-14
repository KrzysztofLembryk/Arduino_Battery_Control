#include <Arduino.h>
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
                     HttpHandler &http_handler);

int do_battery_turn_on_off(int *curr_interval_idx,
                           unsigned long *time_to_next_event_millis,
                           int *time_left_to_next_interval,
                           int *time_to_charge,
                           int charging_times_arr[],
                           HttpHandler &http_handler);

// ----------GLOBAL VARIABLES----------

// ----------HANDLER CLASSES OBJECTS-----------
// needed for interrupts handling, like waiting for info from server
HttpHandler http_handler_global;
// TimeHandler time_handler_global;

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
  // ----------ARRAYS----------
  static int charging_times_arr[NBR_OF_INTERVALS];

  // ----------FLOW CONTROL VARIABLES----------
  static bool first_loop = true;
  static int curr_interval_idx = -1;

  if (!first_loop)
  {
    do_battery_turn_on_off(&curr_interval_idx,
                           time_to_next_event_millis, time_left_to_next_interval,
                           time_to_charge, 
                           charging_times_arr, 
                           http_handler_global);
  }
  else
  {
    /**
     * During first loop we need to fetch data from the server to synchronise
     * our time with real time. 
     * If sunc_with_server or do_battery_turn_on_off fails we set 
     * time_to_next_event to 0 and dont change first_loop to false, because we
     * want to ensure that we fetch data SUCCESSFULLY during first loop
     */
    Serial.println("FIRST LOOOP");

    if (sync_with_server(&curr_interval_idx,
                     time_left_to_next_interval,
                     time_to_charge,
                     charging_times_arr,
                     http_handler_global) != SUCCESS ||
        do_battery_turn_on_off(&curr_interval_idx,
                              time_to_next_event_millis,
                              time_left_to_next_interval,
                              time_to_charge,
                              charging_times_arr,
                              http_handler_global) != SUCCESS)
    {
      Serial.println("[handle_battery_turn_on_off] FIRST LOOP FAILED");
      *time_to_next_event_millis = 0;
      return;
    }

    first_loop = false;
  }
}

int do_battery_turn_on_off(int *curr_interval_idx,
                           unsigned long *time_to_next_event_millis,
                           int *time_left_to_next_interval,
                           int *time_to_charge,
                           int charging_times_arr[],
                           HttpHandler &http_handler)
{
  if (*time_left_to_next_interval == 0)
  {
    /**
     * if time_left_to_next_interval is 0 it means we go to the next interval
     * therefore we increase curr_interval_idx AND check whether we should 
     * sync with the server
     */
    Serial.println("Time left to next interval == 0");
    *curr_interval_idx = (*curr_interval_idx + 1) % NBR_OF_INTERVALS;

    if (*curr_interval_idx == SYNC_SERVER_0000_IDX ||
        *curr_interval_idx == SYNC_SERVER_0600_IDX ||
        *curr_interval_idx == SYNC_SERVER_1200_IDX ||
        *curr_interval_idx == SYNC_SERVER_1715_IDX ||
        *curr_interval_idx == SYNC_SERVER_2200_IDX)
    {
      // in battery_turn_on_off if sync_with_server fails, we don't care we just
      // continue with data we already have till the next sync with server 
      // interval
      if (sync_with_server(curr_interval_idx,
                       time_left_to_next_interval,
                       time_to_charge,
                       charging_times_arr,
                       http_handler) != SUCCESS)
      {
        Serial.println("[do_battery_turn_on_off] FAILED to sync with server");
      }
    }
    else
    {
      /**
       * If we don't sync with server we just set time_left_to_next_interval
       * to 15 mintues, and time_to_charge we get from charging_times_array
       * !!! time_left_to_next_interval is in MINUTES, but our time counting
       * happens in milliseconds,
       * --> thus when updating time_left_to_next_event <---
       * #################################################################
       * !!! WE SHOULD ALWAYS MULTIPLY BY CONSTANT INTERVAL_60S_MILLIS !!!
       * #################################################################
       */
      *time_left_to_next_interval = 15; 
      *time_to_charge = charging_times_arr[*curr_interval_idx];
    }
  }

  if (*time_left_to_next_interval < *time_to_charge)
  {
    /**
     * This case means we have more time_to_charge in current interval than 
     * time to the end of current interval, thus we charge for whole 
     * time_left_to_next_interval
     */
    Serial.println("###############CHARGING START###############");
    Serial.println("time_to_charge: " + String(*time_to_charge) + 
      ", time_left_to_next_interval: " + String(*time_left_to_next_interval));
    *time_to_next_event_millis = *time_left_to_next_interval * INTERVAL_5S_MILLIS;
    *time_left_to_next_interval = 0;
    *time_to_charge = 0;
  }
  else
  {
    if (*time_to_charge == 0)
    {
      /**
       * If time_to_charge is 0 it means either in current interval we have 
       * charged the amount of minutes we wanted to OR we don't charge at all
       * thus time_to_next_event should be equal to time_left_to_next_interval
       */
      Serial.println("###############CHARGING STOP###############");
      Serial.println("time_left_to_next_interval: " + String(*time_left_to_next_interval));

      *time_to_next_event_millis = *time_left_to_next_interval * INTERVAL_5S_MILLIS;
      *time_left_to_next_interval = 0;
    }
    else
    {
      /**
       * We want to charge in curr interval, but time left to next interval
       * is more than time we want to charge, thus time_to_next_event is
       * time_to_charge BUT we also substract time_to_charge from 
       * time_left_to_next_interval
       */
      Serial.println("###############CHARGING START###############");
    Serial.println("time_to_charge: " + String(*time_to_charge) + 
      ", time_left_to_next_interval: " + String(*time_left_to_next_interval));
      *time_left_to_next_interval -= *time_to_charge;
      *time_to_next_event_millis = *time_to_charge * INTERVAL_5S_MILLIS;
      *time_to_charge = 0;
    }
  }

  return SUCCESS;
}

int sync_with_server(int *curr_interval_idx,
                     int *time_left_to_next_interval,
                     int *time_to_charge,
                     int charging_times_arr[],
                     HttpHandler &http_handler)
{
  Serial.println("SYNCING WITH SERVER");

  if (recv_charging_data(charging_times_arr, http_handler) != SUCCESS ||
      recv_curr_interval(curr_interval_idx, time_left_to_next_interval,http_handler) != SUCCESS)
  {
    return ERROR;
  }

  *time_to_charge = charging_times_arr[*curr_interval_idx];

  Serial.printf("###############\ntime_to_charge: %d, curr interval idx: %d, time to next interval: %d\n###############\n", *time_to_charge, *curr_interval_idx, *time_left_to_next_interval);

  return SUCCESS;
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
  for (uint8_t t = 9; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    // flush() ensures that all data is sent to the hardware before proceeding
    Serial.flush();
    delay(1000);
  }
}