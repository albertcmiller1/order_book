import subprocess
import websocket
import json 
import pprint

ws = websocket.WebSocket()

def execute_binary(order_id, limit, shares, buy_sell, event_time, entry_time):
    print("executing binary with these parameters: ")
    print("-----------")
    print(order_id)
    print(limit)
    print(shares)
    print(buy_sell)
    print(event_time)
    print(entry_time)
    print("-----------\n")

    command = [
        "../build/a", 
        "--order_id", 
        f"{order_id}",
        "--limit",
        f"{limit}",
        "--shares",
        f"{shares}",
        "--buy_sell",
        f"{buy_sell}",
        "--event_time",
        f"{event_time}",
        "--entry_time",
        f"{entry_time}"
    ]
    log_file = open(f"../logs/{order_id}.log", "w")
    process = subprocess.Popen(command, stdout=log_file)
    process.wait()
    print("finished.\n\n\n\n\n")


def on_message(wsapp, message):
    print(f"got this message: {message}")
    print("parsing message")
    arr = message.split(" ")
    for i, j in enumerate(arr):
        if j == "--order_id":
            order_id=arr[i+1]
        if j == "--limit":
            limit=arr[i+1]
        if j == "--shares":
            shares=arr[i+1]
        if j == "--buy_sell":
            buy_sell=arr[i+1]
        if j == "--event_time":
            event_time=arr[i+1]
        if j == "--entry_time":
            entry_time=arr[i+1]

    print("done parsing.\n")
    execute_binary(order_id, limit, shares, buy_sell, event_time, entry_time)

wsapp = websocket.WebSocketApp("wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev", on_message=on_message)
wsapp.run_forever() 
