#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "../../include/constants.h"

class ServerData;

// ----------GLOBAL VARIABLES----------
extern WebServer server;
extern ServerData server_data;

class ServerData
{
public:
    ServerData() : m_recvd_data_size(0), m_new_data_recvd(false), m_charging_mode(CHARGING_MODE_DEFAULT) {}
    ~ServerData() = default;

    bool is_new_data_received() const;
    int set_data(char buff[], int buff_size);
    int set_data(JsonDocument &json_doc, const char *key);
    int get_data(int dest[], int dest_size);
    int get_charging_mode() const;
    void print() const;

private:
    static constexpr const int RECV_BUFF_SIZE = 96;
    int m_charging_times_arr[RECV_BUFF_SIZE];
    int m_recvd_data_size;
    bool m_new_data_recvd;
    int m_charging_mode;
};

#endif // GLOBAL_VARS_H