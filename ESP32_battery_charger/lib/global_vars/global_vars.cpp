#include "global_vars.h"
#include "../../include/error_constants.h"

// Here we define global variables so that there are no multiple definitions
// of them
AsyncWebServer server(LOCAL_SERVER_PORT);
ServerData server_data;

bool ServerData::is_new_data_received() const
{
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) 
    {
        volatile bool ret_val = m_new_data_recvd;
        xSemaphoreGive(m_mutex);
        return ret_val;
    }
    else
    {
        Serial.println("[ERROR] - is_new_data_received - Semaphore TIMEOUT");
        return false; 
    }
}

// int ServerData::set_data(char buff[], int buff_size)
// {
//     if (buff == nullptr)
//         return ERROR_NULLPTR;
//     if (buff_size > this->RECV_BUFF_SIZE)
//         return ERROR_MORE_DATA_THAN_BUFF_SIZE;

//     memcpy(m_charging_times_arr, buff, buff_size);
//     m_recvd_data_size = buff_size;
//     m_new_data_recvd = true;

//     return SUCCESS;
// }

int ServerData::set_data(JsonObject &json_obj,
                         const char *data_key,
                         const char *mode_key)
{
    /**
     * We use mutex since we have Async server thus there might be race 
     * conditions. When trying to take semaphore thread will wait indefinitely 
     * if needed.
     */
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) 
    {
        int ret_val = SUCCESS;
        /**
         * If under data_key there is an array we check its size and iterate 
         * through it and update charging_times_arr with user defined values, 
         * if there is no array or it has wrong size we return error 
         */
        if (json_obj[data_key].is<JsonArray>())
        {
            JsonArray array = json_obj[data_key].as<JsonArray>();

            // arr_len in our case should always be 96, if it's not we don't update
            // our charging_times_arr and return error
            int arr_len = array.size();

            if (arr_len == this->RECV_BUFF_SIZE)
            {
                for (int i = 0; i < arr_len; i++)
                {
                    int new_val = json_obj[data_key][i].as<int>();

                    // We want to make sure that charging times are always in 
                    // interval: [MIN_ARR_VAL, MAX_ARR_VAL], so if we got wrong value
                    // from server we just make it either max or min
                    m_charging_times_arr[i] = new_val > MAX_ARR_VAL 
                                                        ? MAX_ARR_VAL
                                                        : (new_val < MIN_ARR_VAL 
                                                            ? MIN_ARR_VAL 
                                                            : new_val);
                }

                m_recvd_data_size = arr_len;

                Serial.println("m_recv_data_size from json: ");
                Serial.print(m_recvd_data_size);
                Serial.println();

                m_new_data_recvd = true;
            }
            else 
                ret_val = ERROR_JSON_WRONG_RECVD_ARR_SIZE;
        } 
        else 
            ret_val = ERROR_JSON_UNDER_DATA_KEY_NOT_ARRAY;

        if (ret_val != SUCCESS) 
        {
            xSemaphoreGive(m_mutex);
            return ret_val;
        }

        /**
         * If there is no mode_key or mode_key value is not within the scope we 
         * change flag new_data_recvd to false and return error, since we dont 
         * want our main to see that there was a request with wrong data 
         */
        if (json_obj[mode_key].is<int>())
        {
            int mode = json_obj[mode_key].as<int>();
            if (mode == CHARGING_MODE_DEFAULT 
                || mode == CHARGING_MODE_USER
                || mode == CHARGING_MODE_USER_WITH_TIMEOUT) 
            {
                m_charging_mode = mode;
            }
            else 
            {
                m_new_data_recvd = false;
                ret_val = ERROR_JSON_UNSUPPORTED_MODE_RCVD;
            }
        }
        else 
        {
            m_new_data_recvd = false;
            ret_val = ERROR_JSON_NO_MODE_KEY;
        }

        xSemaphoreGive(m_mutex);

        return ret_val;
    }
    else 
    {
        Serial.println("[ERROR] - set_data - Semaphore TIMEOUT");
        return ERROR_SEMAPHORE_TIMEOUT;
    }
}

int ServerData::get_data(int dest[], int dest_size)
{

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) 
    {
        m_new_data_recvd = false;

        if (dest == nullptr)
        {
            xSemaphoreGive(m_mutex);
            return ERROR_NULLPTR;
        }
        if (dest_size < m_recvd_data_size)
        {
            Serial.println("[ERROR] get_data dest_size < m_recvd_data_size");
            xSemaphoreGive(m_mutex);
            return ERROR_DEST_SIZE_TO_SMALL;
        }

        for (int i = 0; i < m_recvd_data_size; i++)
        {
            dest[i] = m_charging_times_arr[i];
        }
        // memcpy(dest, m_charging_times_arr, m_recvd_data_size);

        // after getting data from server_data we do not clear charging_times_arr
        // and recvd_data_size so that we remember last user's choice
        xSemaphoreGive(m_mutex);
        return SUCCESS;
    }
    else 
    {
        Serial.println("[ERROR] - get_data - Semaphore TIMEOUT");
        return ERROR_SEMAPHORE_TIMEOUT;
    }
}

int ServerData::get_charging_mode() const
{
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) 
    {
        volatile int ret_val = m_charging_mode;
        xSemaphoreGive(m_mutex);
        return ret_val;
    }
    else 
    {
        Serial.println("[ERROR] - get_charging_mode - Semaphore TIMEOUT");
        return ERROR_SEMAPHORE_TIMEOUT;
    }
}

void ServerData::print() const
{
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) 
    {
        Serial.println("----- ServerData State -----");
        Serial.print("Received Data Size: ");
        Serial.println(m_recvd_data_size);

        Serial.print("New Data Received: ");
        Serial.println(m_new_data_recvd ? "true" : "false");

        Serial.print("Charging Mode: ");
        Serial.println(m_charging_mode);

        Serial.println("Charging Times Array:");
        for (int i = 0; i < m_recvd_data_size; i++)
        {
            Serial.print(m_charging_times_arr[i]);
            Serial.print(", ");
        }
        Serial.println("\n----------------------------");
        Serial.flush();

        xSemaphoreGive(m_mutex);
    }
    else 
    {
        Serial.println("[ERROR] - print - Semaphore TIMEOUT");
    } 
}