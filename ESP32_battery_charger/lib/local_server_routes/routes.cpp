#include "routes.h"
#include <Arduino.h>
#include <WiFi.h>
#include "../global_vars/global_vars.h"
#include "../http_handler/http_handler.h"
#include "../../include/error_constants.h"

// /serverIP
void get_server_ip_addr()
{
    server.send(200, "text/plain", WiFi.localIP().toString().c_str());
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
            server.send(400, "text/plain", "Rcvd data json deserialization error");
            Serial.println("[recv_chargin_times_from_user] json deserialization error");
            return;
        }
        server_data.set_data(json_doc, CHARGING_DATA_KEY, CHARGING_MODE_KEY); 
        server.send(200, "text/plain", "Data received");
    }
    else 
    {
        server.send(400, "text/plain", "no plain arg in post request");
    }
    Serial.flush();
}