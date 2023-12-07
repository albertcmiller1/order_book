'''
this will be a socket that is constantly listening to the output of the order book's socket 
as the orderbook outputs a match of a real user, this socket will post to an AWS db to record transactions 

https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/dynamodb/client/put_item.html
'''

import requests, json, websocket, ast, boto3, uuid

def parse_message(wsapp, message):
    print(f"og message: {message}")
    result = ast.literal_eval(message)
    put_message_in_table(result)


def put_message_in_table(message): 
    message['match_id'] = str(message['match_id'])

    dynamodbTableName = 'order_book_matches'
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table(dynamodbTableName)

    table.put_item(
        TableName=dynamodbTableName,
        Item=message
    )

HOST="ws://0.0.0.0:5001"


wsapp = websocket.WebSocketApp(f"{HOST}/matches", on_message=parse_message)
wsapp.run_forever() 