#include <Arduino.h>
// WiFi.h has WiFiClient.h, WiFiServer.h  included
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.h"
#include "error_constants.h"
#include "char_arr_utils.h"
#include "http_handler.h"

// ----------FUNCTIONS PRE-DECLARATION----------
void wait_for_few_seconds();
void init_WiFi();
int recv_charging_data(int charging_times_arr[],
                       bool is_charging_arr[],
                        HttpHandler &http_handler);
int recv_curr_time(HttpHandler &http_handler, TimeHandler &time_handler);
int recv_curr_interval(HttpHandler &http_handler, TimeHandler &time_handler);
int handle_error_ret_code(int ret_code);

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
  static unsigned long time_to_next_interval = 0;

  // ----------CONTROL VARIABLES----------
  static int curr_interval_idx = 0;
  static int time_till_next_interval = 0;
  static bool is_charging = false;
  static bool first_time = true;

  // ----------FUNCTIONS CALLED ONCE DURING FIRST LOOP----------
  // These could be in setup, but I would need to make two *_arr global 
  // variables, I don't want to make global variables if not really necessary
  if (first_time)
  {
    recv_curr_interval(http_handler, time_handler);

    curr_interval_idx = time_handler.get_curr_interval();
    time_till_next_interval = time_handler.get_time_till_next_interval();

    recv_charging_data(charging_times_arr,
                      is_charging_arr,
                      http_handler);
  }

  // ----------MAIN PROGRAM LOOP----------
  current_millis = millis();

  if (current_millis - prev_millis >= time_to_next_interval)
  {
    prev_millis = current_millis;
    if (is_charging)
    {
      time_to_next_interval = INTERVAL_15_MIN - time_to_next_interval;
      is_charging = false;
      Serial.println("*************SWITCHING CHARGING OFF*************");
    }
    else 
    {
      if (!first_time)
      {
        curr_interval_idx = (curr_interval_idx + 1) % NBR_OF_INTERVALS;

        if (curr_interval_idx == DATA_FOR_TODAY_IDX || 
            curr_interval_idx == DATA_UPDATE_IDX)
        {

          recv_charging_data(charging_times_arr,
                              is_charging_arr,
                              http_handler);
        }

        if (is_charging_arr[curr_interval_idx])
        {
          is_charging = true;
          Serial.println("*************SWITCHING CHARGING ON*************");
          time_to_next_interval = 
                          charging_times_arr[curr_interval_idx] * INTERVAL_10_S;
        }
        else
        {
          time_to_next_interval = INTERVAL_15_MIN;
        }
      }
      else
      {

      }
    }
  }
}

int recv_curr_time(HttpHandler &http_handler, TimeHandler &time_handler)
{
    int ret_code = http_handler.get_curr_time(time_handler,
                                              SERVER_ADDRESS,
                                              CURR_TIME_ENDPOINT);

    if (ret_code == SUCCESS)
    {
      Serial.println("Curr time success");
      return SUCCESS;
    }
    else
      return handle_error_ret_code(ret_code);
}

int recv_curr_interval(HttpHandler &http_handler, TimeHandler &time_handler)
{
  int ret_code = http_handler.get_curr_interval(time_handler,
                                                SERVER_ADDRESS,
                                                CURR_INTERVAL_ENDPOINT);

  if (ret_code == SUCCESS)
  {
    Serial.println("Curr interval success");
    return SUCCESS;
  }
  else
    return handle_error_ret_code(ret_code);
}

int recv_charging_data(int charging_times_arr[],
                       bool is_charging_arr[],
                        HttpHandler &http_handler)
{
  int ret_code = http_handler.get_charging_data(charging_times_arr,
                                                is_charging_arr,
                                                ARR_LEN,
                                                CHARGING_TIME,
                                                IS_CHARGING,
                                                SERVER_ADDRESS,
                                                CHARGING_DATA_ENDPOINT);
  if (ret_code == SUCCESS)
  {
    Serial.println("###SUCCESS - data received###\nPrinting data:");
    Serial.printf("nbr\tis_charging\tcharging_time [min]\n");

    for (int i = 0; i < 3; i++)
    {
      Serial.printf("%d\t%d\t\t%d\n", i,
                    is_charging_arr[i],
                    charging_times_arr[i]);
    }
    Serial.flush();
    return SUCCESS;
  }
  else
    return handle_error_ret_code(ret_code);
}

int handle_error_ret_code(int ret_code)
{
  // TODO
  // We probably need to set some flags here so that in main we can handle them
  switch (ret_code)
  {
  // ######### TODO #########
  // Errors need some handlers, e.g. like while we dont get success we send
  // like 10 consecutive gets to server in 10s intervals, and if we still
  // dont get success, we send info about it to server and maybe wait for
  // its response etc.
  case ERROR:
    Serial.println("[main] Normal error");
    break;
  case ERROR_MORE_DATA_THAN_BUFF_SIZE:
    Serial.println("[main] More data error");
    break;
  case ERROR_DEST_SIZE_TO_SMALL:
    Serial.println("[main] Dest size to small");
    break;
  case ERROR_DESERIALIZE_JSON:
    Serial.println("[main] Deserialize json");
    break;
  case ERROR_RECV_TIME_TOO_SMALL:
    Serial.println("[main] REcv time too small");
    break;
  default:
    Serial.println("[main] Unknown error");
    break;
  }
  return ERROR;
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