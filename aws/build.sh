# aws socket tutorial 
https://docs.aws.amazon.com/apigateway/latest/developerguide/websocket-api-chat-app.html

# connect w/ python 
https://websocket-client.readthedocs.io/en/latest/examples.html

# local shell example 
export URL="wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev"
wscat -c $URL
{"action": "sendmessage", "message": "hello, everyone!"}