#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <ArduinoJson.h>

class TimeHandler
{
public:
    int extract_HM_time_from_json(JsonDocument &json_doc);
    void print_time();

    int extract_curr_interval_from_json(JsonDocument &json_doc);

    int get_curr_interval() const;
    int get_time_till_next_interval() const;

private:
    // CLASS CONSTANTS
    // value is 3 since we need to append '\0' at the end of array
    static constexpr int HOURS_ARR_SIZE = 3;
    static constexpr int MINUTES_ARR_SIZE = 3;
    static constexpr int LAST_ARR_IDX = 2;
    static constexpr int MIN_SIZE_OF_RECV_TIME = 5;

    // CLASS MEMBER VARIABLES
    char hour_arr[HOURS_ARR_SIZE];
    char minute_arr[MINUTES_ARR_SIZE];
    int curr_charging_interval_idx;
    // in minutes
    int time_till_next_interval;
};

#endif // TIME_HANDLER_H