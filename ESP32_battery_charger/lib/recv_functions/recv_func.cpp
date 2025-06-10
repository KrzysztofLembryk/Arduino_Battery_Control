#include "recv_func.h"
#include "../../include/constants.h"
#include "../../include/error_constants.h"

int handle_error_ret_code(int ret_code);


int recv_curr_interval(int *curr_charging_interval_idx,
                      int *time_till_next_interval,
                      HttpHandler &http_handler)
{
  int ret_code = http_handler.get_curr_interval(curr_charging_interval_idx,
                                                time_till_next_interval,
                                                REMOTE_SERVER_ADDRESS,
                                                ENDPOINT_REMOTE_CURR_INTERVAL);

  if (ret_code == SUCCESS)
  {
    Serial.println("Curr interval success");
    return SUCCESS;
  }
  else
    return handle_error_ret_code(ret_code);
}

int recv_charging_data(int charging_times_arr[],
                        HttpHandler &http_handler)
{
  Serial.println("RECV_CHARGING_DATA");
  int ret_code = http_handler.get_charging_data(charging_times_arr,
                                                NBR_OF_INTERVALS,
                                                CHARGING_TIME_KEY,
                                                REMOTE_SERVER_ADDRESS,
                                                ENDPOINT_REMOTE_CHARGING_DATA);
  if (ret_code == SUCCESS)
  {
    Serial.println("###SUCCESS - data received###\nPrinting data:");
    Serial.printf("nbr\tcharging_time [min]\n");

    for (int i = 0; i < 3; i++)
    {
      Serial.printf("%d\t%d\t\n", i, charging_times_arr[i]);
    }
    Serial.flush();
    return SUCCESS;
  }
  else
    return handle_error_ret_code(ret_code);
}


// -------- HELPER FUNCTIONS --------

/**
 * If we get any error it means we have already retried to fetch data from
 * server RETRY_NBR of times, since we failed we also know that data we gave
 * to fetch functions WAS NOT CHANGED thus here we only print logging info
 * and in main function we continue code execution with old data
 */
int handle_error_ret_code(int ret_code)
{
    switch (ret_code)
    {
    case SUCCESS:
        Serial.println("[main] No error, operation successful");
        return SUCCESS;

    case ERROR:
        Serial.println("[main] General error occurred");
        break;

    case ERROR_MORE_DATA_THAN_BUFF_SIZE:
        Serial.println("[main] Error: More data than buffer size");
        break;

    case ERROR_DEST_SIZE_TO_SMALL:
        Serial.println("[main] Error: Destination size too small");
        break;

    case ERROR_DESERIALIZE_JSON:
        Serial.println("[main] Error: Failed to deserialize JSON");
        break;

    case ERROR_RECV_TIME_TOO_SMALL:
        Serial.println("[main] Error: Received time too small");
        break;

    case ERROR_EXCEEDED_WAIT_TIME_FOR_DATA_FROM_SERVER:
        Serial.println("[main] Error: Exceeded wait time for data from server");
        break;

    case ERROR_GET_FAILED:
        Serial.println("[main] Error: HTTP GET request failed");
        break;

    case ERROR_GET_COULDNT_RECV_DATA:
        Serial.println("[main] Error: Couldn't receive data from server");
        break;

    case ERROR_GET_UNABLE_TO_CONNECT:
        Serial.println("[main] Error: Unable to connect to server");
        break;

    case ERROR_JSON_MISSING_KEY:
        Serial.println("[main] Error: Received JSON from server does not contain mandatory keys");
        break;

    default:
        Serial.printf("[main] Unknown error code: %d\n", ret_code);
        break;
    }

    // Return the error code for further handling if needed
    return ret_code;
}