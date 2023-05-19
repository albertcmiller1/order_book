# aws socket tutorial 
https://docs.aws.amazon.com/apigateway/latest/developerguide/websocket-api-chat-app.html

# connect w/ python 
https://websocket-client.readthedocs.io/en/latest/examples.html

# local shell example 
export URL="wss://4jvmckx4fc.execute-api.us-east-1.amazonaws.com/dev"
wscat -c $URL
{"action": "sendmessage", "message": "hello, everyone!"}

# notes
currenlty, anyone can poll and publish to the socket. Alternativly, we should only allow the order book to publish to the socket. 
This can (probably?) be done by adding a little logic in the connection lambda. 



# kafka as an alternative: 
https://aws.amazon.com/msk/what-is-kafka/