#include "http_handler.h"
#include "../char_utils/char_arr_utils.h"

int HttpHandler::recv_data(const char *server_name, const char *endpoint_name)
{
    CharArrUtils::concat_char_arr(HttpHandler::server_endpoint_name,
                                  server_name,
                                  endpoint_name,
                                  HttpHandler::ENDPOINT_NAME_MAX_LEN);

    if (HttpHandler::http.begin(HttpHandler::client,
                                HttpHandler::server_endpoint_name))
    {
        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK || 
                httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
                CharArrUtils::clear_arr(HttpHandler::recv_buff, 
                                        HttpHandler::RECV_BUFF_SIZE);

                int incoming_data_size = http.getSize();
                WiFiClient *stream = http.getStreamPtr();

                while (http.connected() &&
                       (incoming_data_size > 0 || incoming_data_size == -1))
                {
                    size_t available_data_size = stream->available();

                    if (available_data_size)
                    {
                        int c = stream->readBytes(recv_buff, (
                                                                 (available_data_size > sizeof(recv_buff)) ? sizeof(recv_buff) : available_data_size));

                        if (incoming_data_size > 0)
                        {
                            incoming_data_size -= c;
                        }
                    }
                }

                Serial.printf("recv data size: %d\n", strlen(recv_buff));
                JsonDocument doc;
                DeserializationError json_error = deserializeJson(doc, recv_buff);

                if (json_error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(json_error.f_str());
                }
                else
                {
                    for (int i = 0; i < ARR_LEN; i++)
                    {
                        charging_times_arr[i] = doc[CHARGING_TIME][i].as<int>();
                        is_charging_arr[i] = doc[IS_CHARGING][i].as<bool>();
                    }
                }
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
    else
    {
        Serial.println("[HTTP] Unable to connect");
    }
}

int HttpHandler::recv_charging_data(int *charging_times_arr,
                                    bool *is_charging_arr)
{

    if (http.begin(client, server_endpoint_name))
    {
        Serial.print("[HTTP] GET...\n");

        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            static char recv_buff[600] = {0};

            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {

                CharArrUtils::clear_arr(recv_buff, 600);

                int incoming_data_size = http.getSize();
                WiFiClient *stream = http.getStreamPtr();

                while (http.connected() &&
                       (incoming_data_size > 0 || incoming_data_size == -1))
                {
                    size_t available_data_size = stream->available();

                    if (available_data_size)
                    {
                        int c = stream->readBytes(recv_buff, (
                                                                 (available_data_size > sizeof(recv_buff)) ? sizeof(recv_buff) : available_data_size));

                        if (incoming_data_size > 0)
                        {
                            incoming_data_size -= c;
                        }
                    }
                }

                Serial.printf("recv data size: %d\n", strlen(recv_buff));
                JsonDocument doc;
                DeserializationError json_error = deserializeJson(doc, recv_buff);

                if (json_error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(json_error.f_str());
                }
                else
                {
                    for (int i = 0; i < ARR_LEN; i++)
                    {
                        charging_times_arr[i] = doc[CHARGING_TIME][i].as<int>();
                        is_charging_arr[i] = doc[IS_CHARGING][i].as<bool>();
                    }
                }
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
    else
    {
        Serial.println("[HTTP] Unable to connect");
    }
}