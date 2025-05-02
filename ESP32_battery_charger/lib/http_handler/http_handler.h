#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

class HttpHandler
{
public:
    HttpHandler() : recv_data_size(0) {}

    int get_charging_data(int charging_times_arr[],
                            int arr_len,
                            const char *charging_time_key, 
                            const char *server_name, 
                            const char *endpoint_name);

    int get_curr_interval(int *curr_charging_interval_idx,
                        int *time_till_next_interval,
                        const char *server_name, 
                        const char *endpoint_name);

    // int get_curr_time(TimeHandler &time_handler,
    //                     const char *server_name, 
    //                     const char *endpoint_name);
    static int handle_json_deserialization(JsonDocument &doc, char *src_buff);

private:
    // CLASS CONSTANTS
    static constexpr const int SERVER_ENDPOINT_MAX_LEN = 1024;
    static constexpr const int RECV_BUFF_SIZE = 1024;

    // CLASS MEMBER VARIABLES
    WiFiClient client;
    HTTPClient http;
    char server_endpoint_name[SERVER_ENDPOINT_MAX_LEN];
    char recv_buff[RECV_BUFF_SIZE];
    int recv_data_size;

    // PRIVATE METHODS
    int get_data(const char *server_name, const char *endpoint_name);
    int handle_incoming_data_stream();
    int get_data_to_json(JsonDocument &doc,
                        const char *server_name, 
                        const char *endpoint_name);
};

#endif // HTTP_HANDLER_H