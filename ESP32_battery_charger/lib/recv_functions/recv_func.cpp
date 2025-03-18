#include "recv_func.h"
#include "../../include/constants.h"
#include "../../include/error_constants.h"

int handle_error_ret_code(int ret_code);

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


// HELPER FUNCTIONS
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