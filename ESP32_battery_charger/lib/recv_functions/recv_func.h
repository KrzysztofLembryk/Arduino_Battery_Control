#ifndef RECV_FUNC_H
#define RECV_FUNC_H

#include "../http_handler/http_handler.h"
#include "../time_handler/time_handler.h"

int recv_charging_data(int charging_times_arr[], HttpHandler &http_handler);
int recv_curr_time(HttpHandler &http_handler, TimeHandler &time_handler);
int recv_curr_interval(HttpHandler &http_handler, TimeHandler &time_handler);

#endif // RECV_FUNC_H