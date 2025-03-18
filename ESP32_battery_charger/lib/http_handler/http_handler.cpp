#include "http_handler.h"
#include "../char_utils/char_arr_utils.h"
#include "../../include/error_constants.h"

// --------------PUBLIC METHODS IMPL--------------

int HttpHandler::get_curr_time(TimeHandler &time_handler,
                                const char *server_name, 
                                const char *endpoint_name)
{
    JsonDocument json_doc;
    int ret_code = get_data_to_json(json_doc, server_name, endpoint_name);


    if (ret_code != SUCCESS)
        return ret_code;
    
    time_handler.extract_HM_time_from_json(json_doc);
    time_handler.print_time();

    return SUCCESS;
}

int HttpHandler::get_curr_interval(TimeHandler &time_handler,
                                    const char *server_name, 
                                    const char *endpoint_name)
{
    JsonDocument json_doc;
    int ret_code = get_data_to_json(json_doc, server_name, endpoint_name);

    if (ret_code != SUCCESS)
        return ret_code;
    
    time_handler.extract_curr_interval_from_json(json_doc);

    return SUCCESS;

}

int HttpHandler::get_charging_data(int charging_times_arr[],
                                    int arr_len,
                                    const char *charging_time_key, 
                                    const char *is_charging_key,
                                    const char *server_name, 
                                    const char *endpoint_name)
{
    JsonDocument json_doc;
    int ret_code = get_data_to_json(json_doc, server_name, endpoint_name);

    if (ret_code != SUCCESS)
        return ret_code;

    for (int i = 0; i < arr_len; i++)
        charging_times_arr[i] = json_doc[charging_time_key][i].as<int>();
    
    return SUCCESS;
}

// --------------PRIVATE METHODS IMPL--------------
int HttpHandler::get_data_to_json(JsonDocument &json_doc,
                                    const char *server_name, 
                                    const char *endpoint_name)
{
    int recv_data_error = get_data(server_name, endpoint_name);

    if (recv_data_error != SUCCESS)
        return recv_data_error;

    recv_data_error = handle_json_deserialization(json_doc);

    if (recv_data_error != SUCCESS)
        return recv_data_error;
    
    return SUCCESS;
}

int HttpHandler::get_data(const char *server_name, const char *endpoint_name)
{
    int ret_val = CharArrUtils::concat_char_arr(server_endpoint_name,
                                                server_name,
                                                endpoint_name,
                                                SERVER_ENDPOINT_MAX_LEN);
    
    if (ret_val != SUCCESS)
        return ret_val;
    
    ret_val = ERROR;

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
                ret_val = handle_incoming_data_stream();
            }
            else 
            {
                Serial.printf("[HTTP] GET... couldn't recieve data\n");
            }
        }
        else
        {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        return ret_val;
    }
    else
    {
        Serial.println("[HTTP] Unable to connect");
        return ret_val;
    }
}

/**
 * Before receiveing incoming data, function clears recv_buff
 */
int HttpHandler::handle_incoming_data_stream()
{
    CharArrUtils::clear_arr(recv_buff, RECV_BUFF_SIZE);

    recv_data_size = 0;
    int incoming_data_size = http.getSize();

    if (incoming_data_size > RECV_BUFF_SIZE)
    {
        Serial.println("incoming data size > recv_buff_size");
        return ERROR_MORE_DATA_THAN_BUFF_SIZE;
    }

    WiFiClient *stream = http.getStreamPtr();
    int shift = 0;

    // incoming_data_size = -1 --> means no info
    while (http.connected() &&
           (incoming_data_size > 0 || incoming_data_size == -1))
    {
        size_t available_size = stream->available();

        if (shift > RECV_BUFF_SIZE)
        {
            Serial.println("shift > recv_buff_size --> Server sends more data than recv_buff_size");
            return ERROR_MORE_DATA_THAN_BUFF_SIZE;
        }

        if (available_size)
        {
            int bytes_read = stream->readBytes(recv_buff + shift,
            ((available_size > RECV_BUFF_SIZE - shift) ? 
                RECV_BUFF_SIZE - shift : available_size));

            if (incoming_data_size > 0)
            {
                incoming_data_size -= bytes_read;
            }
            
            // we always shift by available_size since in bytes_read we have 
            // if statement thus we can maximally read RECV_BUFF_SIZE bytes to 
            // recv_buff, thus if (shift > RECV_BUFF_SIZE) would never be true
            // if we added bytes_read to shift
            shift += available_size;
            recv_data_size = shift;
        }
    }

    // TODO - instead of recv_data_size we could probably use strlen(recv_buff)
    // since we will get null terminated string
    Serial.printf("recv data size: %d\n", recv_data_size);
    return SUCCESS;
}

int HttpHandler::handle_json_deserialization(JsonDocument &json_doc)
{
    DeserializationError json_error = deserializeJson(json_doc, recv_buff);

    if (json_error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(json_error.f_str());
        return ERROR_DESERIALIZE_JSON;
    }
    return SUCCESS;
}