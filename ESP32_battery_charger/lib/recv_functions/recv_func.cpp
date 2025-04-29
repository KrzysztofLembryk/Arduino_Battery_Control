#include "recv_func.h"
#include "../../include/constants.h"
#include "../../include/error_constants.h"

int handle_error_ret_code(int ret_code);

// int recv_curr_time(HttpHandler &http_handler, TimeHandler &time_handler)
// {
//     int ret_code = http_handler.get_curr_time(time_handler,
//                                               SERVER_ADDRESS,
//                                               CURR_TIME_ENDPOINT);

//     if (ret_code == SUCCESS)
//     {
//       Serial.println("Curr time success");
//       return SUCCESS;
//     }
//     else
//       return handle_error_ret_code(ret_code);
// }

int recv_curr_interval(int *curr_charging_interval_idx,
                      int *time_till_next_interval,
                      HttpHandler &http_handler)
{
  int ret_code = http_handler.get_curr_interval(curr_charging_interval_idx,
                                                time_till_next_interval,
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
                        HttpHandler &http_handler)
{
  Serial.println("RECV_CHARGING_DATA");
  int ret_code = http_handler.get_charging_data(charging_times_arr,
                                                NBR_OF_INTERVALS,
                                                CHARGING_TIME_KEY,
                                                SERVER_ADDRESS,
                                                CHARGING_DATA_ENDPOINT);
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

// int handle_error_ret_code(int ret_code)
// {
//   // TODO
//   // We probably need to set some flags here so that in main we can handle them
//   switch (ret_code)
//   {
//   // ######### TODO #########
//   // Errors need some handlers, e.g. like while we dont get success we send
//   // like 10 consecutive gets to server in 10s intervals, and if we still
//   // dont get success, we send info about it to server and maybe wait for
//   // its response etc.
//   case ERROR:
//     Serial.println("[main] Normal error");
//     break;
//   case ERROR_MORE_DATA_THAN_BUFF_SIZE:
//     Serial.println("[main] More data than buff size error");
//     break;
//   case ERROR_DEST_SIZE_TO_SMALL:
//     Serial.println("[main] Dest size to small");
//     break;
//   case ERROR_DESERIALIZE_JSON:
//     Serial.println("[main] Deserialize json");
//     break;
//   case ERROR_RECV_TIME_TOO_SMALL:
//     Serial.println("[main] REcv time too small");
//     break;
//   default:
//     Serial.println("[main] Unknown error");
//     break;
//   }
//   return ERROR;
// }

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

    default:
        Serial.printf("[main] Unknown error code: %d\n", ret_code);
        break;
    }

    // Return the error code for further handling if needed
    return ret_code;
}