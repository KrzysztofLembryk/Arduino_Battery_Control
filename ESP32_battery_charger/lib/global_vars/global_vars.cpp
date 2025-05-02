#include "global_vars.h"
#include "../../include/error_constants.h"

// Here we define global variables so that there are no multiple definitions
// of them
WebServer server(LOCAL_SERVER_PORT);
ServerData server_data;

bool ServerData::is_new_data_received() const
{
    return this->m_new_data_recvd;
}

int ServerData::set_data(char buff[], int buff_size)
{
    if (buff == nullptr)
        return ERROR_NULLPTR;
    if (buff_size > this->RECV_BUFF_SIZE)
        return ERROR_MORE_DATA_THAN_BUFF_SIZE;

    memcpy(m_recv_buff, buff, buff_size);
    m_recvd_data_size = buff_size;
    m_new_data_recvd = true;

    return SUCCESS;
}

int ServerData::get_data(int dest[], int dest_size)
{
    m_new_data_recvd = false;

    if (dest == nullptr)
        return ERROR_NULLPTR;
    if (dest_size < m_recvd_data_size)
        return ERROR_DEST_SIZE_TO_SMALL;
    
    // for (int i = 0; i < m_recvd_data_size; i++)
    // {
    //     dest[i] = 
    // }
    memcpy(dest, m_recv_buff, m_recvd_data_size);

    m_recvd_data_size = 0;

    memset(m_recv_buff, 0, this->RECV_BUFF_SIZE);
    
    return SUCCESS;
}

int ServerData::get_charging_mode() const
{
    return m_charging_mode;
}