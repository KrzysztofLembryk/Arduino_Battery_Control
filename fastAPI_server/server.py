from fastapi import FastAPI
from fastapi import Response
import pandas as pd
import numpy as np
import sys
import datetime

app = FastAPI()

@app.get("/chargingData")
async def root():
    # np.random.seed(0)
    json_data = {"chargingTime": [np.random.randint(0, 16) for x in range(96)]}
    byte_size = sys.getsizeof(json_data)
    print(f"Byte size of JSON data: {byte_size}")
    # print(f"json_data {json_data}")

    return  json_data


@app.get("/currInterval")
async def get_curr_time():
    now = datetime.datetime.now()
    # Total minutes since 0:00
    total_minutes = now.hour * 60 + now.minute  
    curr_interval_idx = total_minutes // 15  

    # Minutes until the next interval
    minutes_till_next_interval = 15 - (total_minutes % 15)  
    print(f"#### currIntervalIdx {curr_interval_idx}, minutesTillNext {minutes_till_next_interval}####")
    return {"currIntervalIdx": curr_interval_idx,
            "minutesTillNextInterval": minutes_till_next_interval}