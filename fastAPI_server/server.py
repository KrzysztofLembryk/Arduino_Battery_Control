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
    now = datetime.datetime.now()
    # Total minutes since 0:00
    total_minutes = now.hour * 60 + now.minute  
    curr_interval_idx = total_minutes // 15  

    # Minutes until the next interval
    minutes_till_next_interval = 15 - (total_minutes % 15)  
    print(f"#### currIntervalIdx {curr_interval_idx}, minutesTillNext {minutes_till_next_interval}, curr_time: {now}####")
    return {"currIntervalIdx": curr_interval_idx,
            "minutesTillNextInterval": minutes_till_next_interval}