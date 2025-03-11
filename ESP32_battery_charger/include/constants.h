#ifndef CONSTANTS_H
#define CONSTANTS_H

// ----------SERVER ENDPOINTS----------
constexpr const char *SERVER_ADDRESS = "http://192.168.43.112:8000";
constexpr const char *CHARGING_DATA_ENDPOINT = "/chargingData";
constexpr const char *CURR_TIME_ENDPOINT = "/currTime";
constexpr const char *CURR_INTERVAL_ENDPOINT = "/currInterval";

// ----------WIFI CONSTANTS----------
constexpr const char *NETWORK_SSID = "Krzych"; 
constexpr const char *PASSWORD = "12345678";  
constexpr int SERIAL_BAUD_RATE = 115200;

// ----------JSON KEY STRINGS----------
constexpr const char *CHARGING_TIME = "chargingTime";
constexpr const char *IS_CHARGING = "isCharging";
constexpr const char *CURR_TIME = "currTime";

// ----------TIME CONSTANTS----------
constexpr int INTERVAL_60_S = 60000;
constexpr int INTERVAL_10_S = 10000;
constexpr int INTERVAL_GET_DATA_FROM_SERVER = 1 * INTERVAL_10_S;
constexpr int INTERVAL_GET_TIME_FROM_SERVER = 1 * INTERVAL_10_S;
constexpr int INTERVAL_15_MIN = 15 * INTERVAL_10_S;

// idx = 0, means time 00:00, we count in 15min intervals, thus (67 + 1) * 15
// 67 since we count from 0, not from 1
constexpr int DATA_FOR_TODAY_IDX = 0;
constexpr int DATA_UPDATE_IDX = 67;

// ----------DATA SIZE CONSTANTS----------
constexpr int ARR_LEN = 96;
constexpr int NBR_OF_INTERVALS = 96;
constexpr int REC_DATA_BUFF_SIZE = 1024;


#endif // CONSTANTS_H