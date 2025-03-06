#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <WiFi.h>
#include <HTTPClient.h>

class HttpHandler
{

public:
    static int recv_charging_data(int *charging_times_arr,
                                    bool *is_charging_arr);

private:
    static WiFiClient client;
    static HTTPClient http;
    static constexpr const int ENDPOINT_NAME_MAX_LEN = 100;
    static constexpr const int RECV_BUFF_SIZE = 1024;
    static char server_endpoint_name[ENDPOINT_NAME_MAX_LEN];
    static char recv_buff[RECV_BUFF_SIZE];

    static int recv_data(const char *server_name, const char *endpoint_name);
};


#endif // HTTP_HANDLER_H