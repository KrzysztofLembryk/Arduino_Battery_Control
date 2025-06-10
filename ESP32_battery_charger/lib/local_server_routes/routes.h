#ifndef ROUTES_H
#define ROUTES_H

// #include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void get_server_ip_addr(AsyncWebServerRequest *request);
void recv_charging_times_from_user(AsyncWebServerRequest *request);
void get_user_interface(AsyncWebServerRequest *request);

#endif // ROUTES_H