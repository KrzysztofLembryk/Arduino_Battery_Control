#include "routes.h"
#include <Arduino.h>
#include <WiFi.h>
#include "../global_vars/global_vars.h"

void get_server_ip_addr()
{
    Serial.println("#######################################");
    Serial.println("#######################################");
    Serial.println("########## GETTING CONNECTION #########"); 
    Serial.println("#######################################");
    Serial.println("#######################################");
    server.send(200, "text/plain", WiFi.localIP().toString().c_str());
}

void recv_charging_times_from_user()
{
    Serial.print("Number of args in POST request: ");
    Serial.println(server.args());

    Serial.println("------- host header -------");
    Serial.println(server.hostHeader());

    if (server.hasArg("userData"))
    {
        Serial.println("#######################################");
        Serial.println("request has USER_DATA arg");
        Serial.println(server.arg("userData"));
        Serial.println("#######################################");
    }

    if (server.hasArg("body"))
    {
        Serial.println("#######################################");
        Serial.println("request has BODY arg:");
        Serial.println(server.arg("body"));
        Serial.println("#######################################");
    }

    if (server.hasArg("plain"))
    {
        Serial.println("#######################################");
        Serial.println("request has PLAIN arg:");
        Serial.println(server.arg("plain"));
        Serial.println("#######################################");
    }

    Serial.flush();
}