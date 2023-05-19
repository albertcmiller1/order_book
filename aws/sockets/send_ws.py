import websocket
import json 

url = "wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev"
ws = websocket.WebSocket()
ws.connect(url)

ws.send(json.dumps({
        "action": "sendmessage",
        "message": "--order_id 123456 --limit 23.42 --shares 1 --buy_sell buy --event_time 000000 --entry_time 983485"
    })
)

ws.close()

