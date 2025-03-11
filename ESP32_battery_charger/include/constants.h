#ifndef CONSTANTS_H
#define CONSTANTS_H


constexpr const char *SERVER_ADDRESS = "http://192.168.43.112:8000";
constexpr const char *CHARGING_DATA_ENDPOINT = "/chargingData";
constexpr const char *CURR_TIME_ENDPOINT = "/currTime";

constexpr const char *NETWORK_SSID = "Krzych"; 
constexpr const char *PASSWORD = "12345678";  
constexpr int SERIAL_BAUD_RATE = 115200;

constexpr const char *CHARGING_TIME = "chargingTime";
constexpr const char *IS_CHARGING = "isCharging";
constexpr const char *CURR_TIME = "currTime";

constexpr int INTERVAL_60_S = 60000;
constexpr int INTERVAL_10_S = 10000;
constexpr int INTERVAL_GET_DATA_FROM_SERVER = 1 * INTERVAL_10_S;
constexpr int INTERVAL_GET_TIME_FROM_SERVER = 1 * INTERVAL_10_S;
constexpr int INTERVAL_15_MIN = 15 * INTERVAL_10_S;


constexpr int ARR_LEN = 96;
constexpr int NBR_OF_INTERVALS = 96;
constexpr int REC_DATA_BUFF_SIZE = 1024;


#endif // CONSTANTS_H