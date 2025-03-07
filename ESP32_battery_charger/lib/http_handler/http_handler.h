#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <WiFi.h>
#include <HTTPClient.h>

class HttpHandler
{

public:
    static int recv_charging_data(int *charging_times_arr,
                                    bool *is_charging_arr,
                                    int arr_len,
                                    const char *charging_time_key, 
                                    const char *is_charging_key,
                                    const char *server_name, 
                                    const char *endpoint_name);

private:
    // CONSTANTS
    static constexpr const int SERVER_ENDPOINT_MAX_LEN = 100;
    static constexpr const int RECV_BUFF_SIZE = 1024;

    // CLASS MEMBER VARIABLES
    static WiFiClient client;
    static HTTPClient http;
    static char server_endpoint_name[SERVER_ENDPOINT_MAX_LEN];
    static char recv_buff[RECV_BUFF_SIZE];
    static int recv_data_size;

    static int handle_incoming_data();
    static int recv_data(const char *server_name, const char *endpoint_name);
};


#endif // HTTP_HANDLER_H