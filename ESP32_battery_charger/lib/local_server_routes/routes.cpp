#include "routes.h"
#include <Arduino.h>
#include <WiFi.h>
#include "../global_vars/global_vars.h"
#include "../http_handler/http_handler.h"
#include "../../include/error_constants.h"

// helper functions pre-declarations
void handle_set_data_ret_code(int result);

// /serverIP
void get_server_ip_addr()
{
    server.send(HTTP_CODE_OK, "text/plain", WiFi.localIP().toString().c_str());
}

// /userData
void recv_charging_times_from_user()
{
    if (server.hasArg(PLAIN_KEY))
    {
        JsonDocument json_doc;
        static char recv_buff[1024] = {0};

        server.arg(PLAIN_KEY).toCharArray(recv_buff, 1024);

        if (HttpHandler::handle_json_deserialization(json_doc, recv_buff) != SUCCESS)
        {
            server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "Rcvd data json deserialization error");
            return;
        }

        memset(recv_buff, 0, 1024);
        handle_set_data_ret_code(server_data.set_data(json_doc,
                                            CHARGING_DATA_KEY,
                                            CHARGING_MODE_KEY));
    }
    else
    {
        server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "No plain arg in post request");
    }
    Serial.flush();
}


// helper functions impl
void handle_set_data_ret_code(int result)
{
    switch (result)
    {
    case SUCCESS:
        server.send(HTTP_CODE_OK, "text/plain", "Data successfully set");
        break;

    case ERROR_JSON_WRONG_RECVD_ARR_SIZE:
        server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Received array size is incorrect, should be " + String(NBR_OF_INTERVALS));
        break;

    case ERROR_JSON_UNDER_DATA_KEY_NOT_ARRAY:
        server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Data under:" + String(CHARGING_DATA_KEY) + " key is not an array");
        break;

    case ERROR_JSON_UNSUPPORTED_MODE_RCVD:
        server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Unsupported mode received");
        break;

    case ERROR_JSON_NO_MODE_KEY:
        server.send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Mode key is missing in JSON");
        break;

    default:
        server.send(HTTP_CODE_INTERNAL_SERVER_ERROR, "text/plain", "Error: Unknown error occurred");
        break;
    }
}