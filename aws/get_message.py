import subprocess
import websocket
import json 

# ws = websocket.WebSocket()

# def on_message(wsapp, message):
#     print(message)

# wsapp = websocket.WebSocketApp("wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev", on_message=on_message)
# wsapp.run_forever() 

order_id="123456"
limit="23.42"
shares="1"
buy_sell="true"
event_time="000000"
entry_time="983485"

# ./a --order_id 123456 --limit 23.42 --shares 1 --buy_sell true --event_time 983485 --entry_time 983485

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

log_file = open(f"../logs/output.log", "w")

process = subprocess.Popen(command, stdout=log_file)
process.wait()
