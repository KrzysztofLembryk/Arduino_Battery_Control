from fastapi import FastAPI
from fastapi import Response
import pandas as pd
import numpy as np
import sys

app = FastAPI()

@app.get("/")
async def root():
    np.random.seed(0)
    json_data = {"isCharging": [np.random.randint(0, 2) for x in range(96)],
                 "chargingTime": [np.random.randint(1, 16) for x in range(96)]}
    # isCharging = np.random.randint(0, 2, 96)
    # chargingTime = [np.random.randint(1, 16) if x == 1 else 0 for x in isCharging]
    # df = pd.DataFrame({"isCharging": isCharging, 
    #                    "chargingTime":chargingTime, 
    #                    })

    # json_data = df.to_json()
    byte_size = sys.getsizeof(json_data)
    print(f"Byte size of JSON data: {byte_size}")
    print(f"json_data {json_data}")

    return  json_data