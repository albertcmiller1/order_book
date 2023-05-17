import websocket
import json 

ws = websocket.WebSocket()
ws.connect("wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev")

ws.send(json.dumps({"action": "sendmessage", "message": "hello socket world"}))

ws.close()

