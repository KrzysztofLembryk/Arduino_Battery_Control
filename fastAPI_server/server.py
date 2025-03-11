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
    json_data = {"isCharging": [np.random.randint(0, 2) for x in range(96)],
                 "chargingTime": [np.random.randint(1, 16) for x in range(96)]}
    byte_size = sys.getsizeof(json_data)
    print(f"Byte size of JSON data: {byte_size}")
    # print(f"json_data {json_data}")

    return  json_data


@app.get("/currTime")
async def get_curr_time():
    now = datetime.datetime.now()
    return {"currTime": now.time()}