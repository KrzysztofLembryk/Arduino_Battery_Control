#ifndef CONSTANTS_H
#define CONSTANTS_H

// ----------REMOTE SERVER ENDPOINTS----------
constexpr const char *SERVER_ADDRESS = "http://192.168.43.112:8000";
constexpr const char *CHARGING_DATA_ENDPOINT = "/chargingData";
// constexpr const char *CURR_TIME_ENDPOINT = "/currTime";
constexpr const char *CURR_INTERVAL_ENDPOINT = "/currInterval";

// ----------LOCAL SERVER CONSTANTS----------
constexpr int LOCAL_SERVER_PORT = 80;

// We use only charging times from global server
constexpr int CHARGING_MODE_DEFAULT = 0; 
// We use only charging times defined by user without fetching any data from global server 
constexpr int CHARGING_MODE_USER = 1;
// For user defined period we use only user's charging times, then we switch to using charging times from global server only
constexpr int CHARGING_MODE_USER_WITH_TIMEOUT = 2;

// ----------WIFI CONSTANTS----------
constexpr const char *NETWORK_SSID = "Krzych"; 
constexpr const char *PASSWORD = "12345678";  
constexpr int SERIAL_BAUD_RATE = 115200;

// ----------JSON KEY STRINGS----------
constexpr const char *CHARGING_TIME_KEY = "chargingTime";
constexpr const char *IS_CHARGING = "isCharging";
constexpr const char *CURR_TIME = "currTime";

// ----------TIME CONSTANTS----------
constexpr int INTERVAL_60S_MILLIS = 60000;
constexpr int INTERVAL_10S_MILLIS = 10000;
constexpr int INTERVAL_5S_MILLIS = 5000;
constexpr int INTERVAL_GET_DATA_FROM_SERVER = 1 * INTERVAL_10S_MILLIS;
constexpr int INTERVAL_GET_TIME_FROM_SERVER = 1 * INTERVAL_10S_MILLIS;
constexpr int INTERVAL_15MIN_MILLIS = 15 * INTERVAL_10S_MILLIS;
constexpr int FULL_INTERVAL_DURATION_MINS = 15;

// ----------SYNC WITH SERVER CONSTANTS---------- 
/**
 * In 24 hours we have 96 15min intervals, 00:00 has idx = 0, 23:45 idx = 95.
 * We define five times when we sync with our server so that our timer does not
 * deviate much from real time
 */
constexpr int SYNC_SERVER_0000_IDX = 0;
constexpr int SYNC_SERVER_0600_IDX = 24;
constexpr int SYNC_SERVER_1200_IDX = 48;
constexpr int SYNC_SERVER_1715_IDX = 69;
constexpr int SYNC_SERVER_2200_IDX = 88;

// ----------ERROR CONTROL FLOW CONSTANTS----------
constexpr int RETRY_NBR = 5;

// ----------DATA SIZE CONSTANTS----------
constexpr int ARR_LEN = 96;
constexpr int NBR_OF_INTERVALS = 96;
constexpr int REC_DATA_BUFF_SIZE = 1024;


#endif // CONSTANTS_H