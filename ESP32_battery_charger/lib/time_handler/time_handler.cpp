#include "time_handler.h"
#include "../../include/constants.h"
#include "../../include/error_constants.h"

int TimeHandler::extract_curr_interval_from_json(JsonDocument &json_doc)
{
    curr_charging_interval_idx = json_doc["currIntervalIdx"].as<int>();
    time_till_next_interval = json_doc["minutesTillNextInterval"].as<int>();
    return SUCCESS;
}

/**
 * Function extracts HM (hours and minutes) from time given from server
 */
int TimeHandler::extract_HM_time_from_json(JsonDocument &json_doc)
{
    const char *buff = json_doc[CURR_TIME].as<const char*>();

    if (strlen(buff) < MIN_SIZE_OF_RECV_TIME)
    {
        Serial.println("[extract_HM_time_from_json] ERROR recv time in json_doc is < 5 (time we receive needs to be at least HH:MM)");
        return ERROR_RECV_TIME_TOO_SMALL;
    }

    for(int i = 0, j = 3; i < 2; i++, j++)
    {
        hour_arr[i] = buff[i];
        minute_arr[i] = buff[j];
    }

    hour_arr[LAST_ARR_IDX] = '\0';
    minute_arr[LAST_ARR_IDX] = '\0';

    return SUCCESS;
}

void TimeHandler::print_time()
{
    Serial.println("[TimeHandler] print time:");
    Serial.println(String(hour_arr) + ":" + String(minute_arr));
}

int TimeHandler::get_curr_interval() const
{
    return curr_charging_interval_idx;
}

int TimeHandler::get_time_till_next_interval() const
{
    return time_till_next_interval;
}