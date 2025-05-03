#ifndef ERROR_CONSTANTS_H
#define ERROR_CONSTANTS_H

constexpr const int SUCCESS = 0;
constexpr const int ERROR = 1;
constexpr const int ERROR_MORE_DATA_THAN_BUFF_SIZE = 2;
constexpr const int ERROR_DEST_SIZE_TO_SMALL = 3;
constexpr const int ERROR_DESERIALIZE_JSON = 4;
constexpr const int ERROR_RECV_TIME_TOO_SMALL = 5;
constexpr const int ERROR_EXCEEDED_WAIT_TIME_FOR_DATA_FROM_SERVER = 6;
constexpr const int ERROR_GET_FAILED = 7;
constexpr const int ERROR_GET_COULDNT_RECV_DATA = 8;
constexpr const int ERROR_GET_UNABLE_TO_CONNECT = 9;
constexpr const int ERROR_NULLPTR = 10;
constexpr const int ERROR_JSON_KEY_NOT_PRESENT = 11;
constexpr const int ERROR_JSON_UNDER_DATA_KEY_NOT_ARRAY = 12;
constexpr const int ERROR_JSON_WRONG_RECVD_ARR_SIZE = 13;
constexpr const int ERROR_JSON_NO_MODE_KEY = 14;
constexpr const int ERROR_JSON_UNSUPPORTED_MODE_RCVD = 15;

#endif // ERROR_CONSTANTS_H