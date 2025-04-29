#include "http_handler.h"
#include "../char_utils/char_arr_utils.h"
#include "../../include/error_constants.h"
#include "../../include/constants.h"

// --------------PUBLIC METHODS IMPL--------------

// int HttpHandler::get_curr_time(TimeHandler &time_handler,
//                                 const char *server_name, 
//                                 const char *endpoint_name)
// {
//     JsonDocument json_doc;
//     int ret_code = get_data_to_json(json_doc, server_name, endpoint_name);

//     if (ret_code != SUCCESS)
//         return ret_code;
    
//     time_handler.extract_HM_time_from_json(json_doc);
//     time_handler.print_time();

//     return SUCCESS;
// }

constexpr const char *CURR_INTERVAL_IDX = "currIntervalIdx";
constexpr const char *MINUTES_TILL_NEXT_INTERVAL = "minutesTillNextInterval";

/**
 * If function encounters error variables:
 * - curr_charging_interval_idx 
 * - time_till_next_interval 
 * ARE NOT MODIFIED
 */
int HttpHandler::get_curr_interval(int *curr_charging_interval_idx,
                                    int *time_till_next_interval,
                                    const char *server_name, 
                                    const char *endpoint_name)
{
    Serial.println("get curr interval");
    JsonDocument json_doc;
    int ret_code = get_data_to_json(json_doc, server_name, endpoint_name);

    if (ret_code != SUCCESS)
    {
        Serial.println("get curr interval getting data to json ERROR");
        return ret_code;
    }
    
    *curr_charging_interval_idx = json_doc[CURR_INTERVAL_IDX].as<int>();
    *time_till_next_interval = json_doc[MINUTES_TILL_NEXT_INTERVAL].as<int>();

    return SUCCESS;
}

/**
 * If function encounters error variable:
 * - charging_times_arr
 * IS NOT MODIFIED
 */
int HttpHandler::get_charging_data(int charging_times_arr[],
                                    int arr_len,
                                    const char *charging_time_key, 
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
    Serial.println("get data to json");
    int recv_data_error = get_data(server_name, endpoint_name);

    if (recv_data_error != SUCCESS)
        return recv_data_error;

    recv_data_error = handle_json_deserialization(json_doc);

    return recv_data_error;
}

/**
 * Function when encountering any error tries RETRY_NBR times to fetch data from server, if still unsuccessful it propagates error further 
 * --> Returns:
 * - ERROR_GET_COULDNT_RECV_DATA - when httpCode > 0 but not OK or MOVED_PERMANENTLY
 * - ERROR_GET_FAILED - when httpCode < 0
 * - ERROR_GET_UNABLE_TO_CONNECT - when http.begin returned error
 * - ERROR_DEST_SIZE_TO_SMALL - when concat_char_arr fails
 */
int HttpHandler::get_data(const char *server_name, const char *endpoint_name)
{
    /**
     * CharArrUtils::concat_char_arr creates a string (URL) from 
     * server_name, endpoint_name and stores it in server_endpoint_name
     */
    int ret_val = CharArrUtils::concat_char_arr(server_endpoint_name,
                                                server_name,
                                                endpoint_name,
                                                SERVER_ENDPOINT_MAX_LEN);
    
    if (ret_val != SUCCESS)
        return ret_val;

    int loop_nbr = 0;

    // If we encounter error we try again for RETRY_NBR times
    while (loop_nbr < RETRY_NBR)
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

                if (httpCode == HTTP_CODE_OK ||
                    httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    Serial.println("HANDLING INCOMING DATA STREAM");
                    ret_val = handle_incoming_data_stream();
                }
                else 
                {
                    Serial.printf("[HTTP] GET... couldn't receive data\n");
                    ret_val = ERROR_GET_COULDNT_RECV_DATA;
                }
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                ret_val = ERROR_GET_FAILED;
            }

            Serial.println("[HTTP] END");
            http.end();
        }
        else
        {
            Serial.println("[HTTP] Unable to connect");
            ret_val =  ERROR_GET_UNABLE_TO_CONNECT;
        }

        if (ret_val != SUCCESS)
        {
            Serial.println("[HTTP] RETRYING...");
            loop_nbr++;
            continue;
        }
        return ret_val;
    }
    return ret_val;
}

/**
 * Before receiveing incoming data, function clears recv_buff
 * Returns:
 * - ERROR_MORE_DATA_THAN_BUFF_SIZE - when read/incoming data > than RECV_BUFF
 * - ERROR_EXCEEDED_WAIT_TIME_FOR_DATA_FROM_SERVER - when we waited 10x100ms
 * for data from server and didnt get any
 * - SUCCESS - otherwise
 */
int HttpHandler::handle_incoming_data_stream()
{
    CharArrUtils::clear_arr(recv_buff, RECV_BUFF_SIZE);

    recv_data_size = 0;
    int incoming_data_size = http.getSize();
    Serial.printf("INCOMING DATA SIZE: %d\n", incoming_data_size);
    if (incoming_data_size > RECV_BUFF_SIZE)
    {
        Serial.println("incoming data size > recv_buff_size");
        return ERROR_MORE_DATA_THAN_BUFF_SIZE;
    }

    WiFiClient *stream = http.getStreamPtr();
    int shift = 0;
    int delay_counter = 0;
    unsigned long prev_time = 0;
    unsigned long curr_time = 0;
    // incoming_data_size == -1 --> means no info
    while (http.connected() &&
           (incoming_data_size > 0 || incoming_data_size == -1))
    {
        curr_time = millis();
        size_t available_size = stream->available();

        if (shift > RECV_BUFF_SIZE)
        {
            Serial.println("shift > recv_buff_size --> Server sends more data than recv_buff_size");
            return ERROR_MORE_DATA_THAN_BUFF_SIZE;
        }

        if (available_size)
        {
            // we zero delay counter since we got some data
            delay_counter = 0;

            int bytes_read = stream->readBytes(recv_buff + shift,
                                    available_size > RECV_BUFF_SIZE - shift ? 
                                    RECV_BUFF_SIZE - shift : available_size);

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
        else if (curr_time - prev_time >= 100) 
        {
            /**
             * WiFi connection is slow and might take more than 1s to get data
             * from server. ESP32 has a watchdog which detects loops that take
             * over 1s and  RESETS whole arduino, to prevent that we call yield
             * which resets watchdog timer
             */
            prev_time = curr_time;

            yield();

            delay_counter++;
            if (delay_counter > 10)
            {
                Serial.println("HANDLE INCOMING DATA STREAM, delay counter > 10, we waited too long for data");
                return ERROR_EXCEEDED_WAIT_TIME_FOR_DATA_FROM_SERVER;
            }
        }
    }

    // TODO - instead of recv_data_size we could probably use strlen(recv_buff)
    // since we will get null terminated string
    Serial.printf("recv data size: %d\n", recv_data_size);
    return SUCCESS;
}

/**
 * Function stores data from recv_buffer to json_doc
 * Returns:
 * - ERROR_DESERIALIZE_JSON if deserialization was unsuccessful,
 * - SUCCESS otherwise
 */
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