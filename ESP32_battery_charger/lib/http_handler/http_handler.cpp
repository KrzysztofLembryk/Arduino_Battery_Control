#include <ArduinoJson.h>
#include "http_handler.h"
#include "../char_utils/char_arr_utils.h"

int HttpHandler::handle_incoming_data()
{
    CharArrUtils::clear_arr(recv_buff, RECV_BUFF_SIZE);

    int incoming_data_size = http.getSize();
    recv_data_size = incoming_data_size;

    WiFiClient *stream = http.getStreamPtr();
    int shift = 0;

    while (http.connected() &&
           (incoming_data_size > 0 || incoming_data_size == -1))
    {
        size_t available_size = stream->available();

        if (available_size)
        {
            int bytes_read = stream->readBytes(recv_buff + shift,
            ((available_size > RECV_BUFF_SIZE - shift) ? 
                RECV_BUFF_SIZE - shift : available_size));

            if (incoming_data_size > 0)
            {
                shift += bytes_read;
                incoming_data_size -= bytes_read;
            }
        }
    }

    // TODO - instead of recv_data_size we could probably use strlen(recv_buff)
    // since we will get null terminated string
    Serial.printf("recv data size: %d\n", recv_data_size);
    return 0;
}

int HttpHandler::recv_data(const char *server_name, const char *endpoint_name)
{
    CharArrUtils::concat_char_arr(server_endpoint_name,
                                  server_name,
                                  endpoint_name,
                                  SERVER_ENDPOINT_MAX_LEN);
    
    int ret_val = 0;

    if (http.begin(client, server_endpoint_name))
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
                handle_incoming_data();
            }
            else 
            {
                Serial.printf("[HTTP] GET... couldn't recieve data\n");
                ret_val = -1;
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            ret_val = -1;
        }

        http.end();
        return ret_val;
    }
    else
    {
        Serial.println("[HTTP] Unable to connect");
        return -1;
    }
}

int HttpHandler::recv_charging_data(int *charging_times_arr,
                                    bool *is_charging_arr,
                                    int arr_len,
                                    const char *charging_time_key, 
                                    const char *is_charging_key,
                                    const char *server_name, 
                                    const char *endpoint_name)
{
    int ret_val = recv_data(server_name, endpoint_name);

    if (ret_val == -1)
    {
        return -1;
    }

    JsonDocument doc;
    DeserializationError json_error = deserializeJson(doc, recv_buff);

    if (json_error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(json_error.f_str());
        return -1;
    }
    else
    {
        for (int i = 0; i < arr_len; i++)
        {
            charging_times_arr[i] = doc[charging_time_key][i].as<int>();
            is_charging_arr[i] = doc[is_charging_key][i].as<bool>();
        }
    }
    return 0;
}