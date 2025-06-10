#include "routes.h"
#include <Arduino.h>
#include <WiFi.h>
#include "../global_vars/global_vars.h"
#include "../http_handler/http_handler.h"
#include "../../include/error_constants.h"

// helper functions pre-declarations
void handle_set_data_ret_code(int result, AsyncWebServerRequest *request);

// /serverIP
void get_server_ip_addr(AsyncWebServerRequest *request)
{
    request->send(HTTP_CODE_OK, "text/plain", WiFi.localIP().toString().c_str());
}

// /userData
void recv_charging_times_from_user(AsyncWebServerRequest *request)
{
    if (request->hasArg(PLAIN_KEY))
    {
        JsonDocument json_doc;
        static char recv_buff[1024] = {0};

        request->arg(PLAIN_KEY).toCharArray(recv_buff, 1024);

        if (HttpHandler::handle_json_deserialization(json_doc, recv_buff) != SUCCESS)
        {
            request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Rcvd data json deserialization error");
            return;
        }

        memset(recv_buff, 0, 1024);
        handle_set_data_ret_code(server_data.set_data(json_doc,
                                            CHARGING_DATA_KEY,
                                            CHARGING_MODE_KEY), request);
    }
    else
    {
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "No plain arg in post request");
    }
    Serial.flush();
}


void get_user_interface(AsyncWebServerRequest *request)
{
  digitalWrite(LED_BUILTIN, HIGH);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
  </html>",

           hr, min % 60, sec % 60);
  request->send(200, "text/html", temp);
  digitalWrite(LED_BUILTIN, LOW);
}


// helper functions impl
void handle_set_data_ret_code(int result, AsyncWebServerRequest *request)
{
    switch (result)
    {
    case SUCCESS:
        request->send(HTTP_CODE_OK, "text/plain", "Data successfully set");
        break;

    case ERROR_JSON_WRONG_RECVD_ARR_SIZE:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Received array size is incorrect, should be " + String(NBR_OF_INTERVALS));
        break;

    case ERROR_JSON_UNDER_DATA_KEY_NOT_ARRAY:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Data under:" + String(CHARGING_DATA_KEY) + " key is not an array");
        break;

    case ERROR_JSON_UNSUPPORTED_MODE_RCVD:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Unsupported mode received");
        break;

    case ERROR_JSON_NO_MODE_KEY:
        request->send(HTTP_CODE_BAD_REQUEST, "text/plain", "Error: Mode key is missing in JSON");
        break;

    default:
        request->send(HTTP_CODE_INTERNAL_SERVER_ERROR, "text/plain", "Error: Unknown error occurred");
        break;
    }
}
