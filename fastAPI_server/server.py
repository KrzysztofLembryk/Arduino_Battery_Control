from fastapi import FastAPI
from fastapi import Response
import pandas as pd
import numpy as np
import sys
import datetime
import requests

app = FastAPI()

@app.get("/arduinoIP")
async def arduino_ip():
    res = requests.get("http://192.168.43.147/serverIP")
    print(f"##### /arduinoIP res type {type(res)}, value: {res.text} #####")
    return res.text

@app.get("/sendUserData")
async def user_data():
    json_data = {"chargingData": [np.random.randint(6, 9) for x in range(96)],
                 "chargingMode": 0}

    res = requests.api.post("http://192.168.43.147/userData", json=json_data)
    print(f"/sendUserData res: {res.text}")
    return res.text


@app.get("/chargingData")
async def root():
    # np.random.seed(0)
    json_data = {"chargingTime": [np.random.randint(0, 6) for x in range(96)]}
    byte_size = sys.getsizeof(json_data)
    print(f"Byte size of JSON data: {byte_size}")
    # print(f"json_data {json_data}")

     # Pretty print the charging data
    print("Charging Data (idx: value):")
    for idx, value in enumerate(json_data["chargingTime"]):
        print(f"{idx}: {value}")

    return  json_data



@app.get("/currInterval")
async def get_curr_time():
    """
    Returns the current 15-minute interval index and milliseconds until the next interval.
    Using milliseconds provides much more precise timing for the microcontroller.
    """
    now = datetime.datetime.now()
    
    # Total minutes since 0:00
    total_minutes = now.hour * 60 + now.minute  
    curr_interval_idx = total_minutes // 15  
    
    # Calculate the exact time of the next interval
    minutes_in_current_interval = total_minutes % 15
    seconds_in_minute = now.second
    microseconds = now.microsecond
    
    # Total milliseconds until next interval
    # +1000 buffer to accomodate for the time it takes to process the request
    # or some network delay etx
    milliseconds_till_next = ((14 - minutes_in_current_interval) * 60 + (59 - seconds_in_minute)) * 1000 + (1000000 - microseconds) // 1000 + 1000
    
    print(f"#### currIntervalIdx {curr_interval_idx}, msTillNext {milliseconds_till_next}, curr_time: {now}####")
    
    return {"currIntervalIdx": curr_interval_idx,
            "millisTillNextInterval": milliseconds_till_next}