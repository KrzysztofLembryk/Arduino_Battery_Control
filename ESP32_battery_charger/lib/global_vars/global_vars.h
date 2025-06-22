#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "../../include/constants.h"

class ServerData;

// ----------GLOBAL VARIABLES----------
extern AsyncWebServer server;
extern ServerData server_data;

class ServerData
{
public:
    ServerData() : m_recvd_data_size(0), m_new_data_recvd(false), m_charging_mode(CHARGING_MODE_DEFAULT), m_mutex(xSemaphoreCreateMutex()) {}
    ~ServerData() = default;

    bool is_new_data_received() const;
    // int set_data(char buff[], int buff_size);
    int set_data(JsonObject &json_obj, const char *key, const char *mode_key);
    int get_data(int dest[], int dest_size);
    int get_charging_mode() const;
    void print() const;

private:
    static constexpr const int RECV_BUFF_SIZE = NBR_OF_INTERVALS;
    int m_charging_times_arr[RECV_BUFF_SIZE];
    int m_recvd_data_size;
    bool m_new_data_recvd;
    int m_charging_mode;
    SemaphoreHandle_t m_mutex;
};

#endif // GLOBAL_VARS_H