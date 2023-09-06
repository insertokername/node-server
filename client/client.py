import websocket
import message_parser
import json

def on_message(ws, message):

    print(f"Received message: {message}")
    return_msg = message_parser.respond(message) 
    ws.send(return_msg )
    

def on_close(ws, close_status_code, close_msg):
    print("Connection closed")

def on_open(ws):
    print("Client started!")
    ws.send(json.dumps({"from":"client","to":"server","command":"register-client"}))

ws = websocket.WebSocketApp("ws://localhost:8080/",
                            on_message=on_message,
                            on_close=on_close,
                            on_open=on_open)

ws.run_forever()