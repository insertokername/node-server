import pyautogui
import json

import base64
from io import BytesIO

def respond(str_message:str)->str:
    try:
        message= json.loads(str_message)
        command="log"
        if(message["command"]=="screenshot"):
            command="photo"

        return (json.dumps( {"from":"client","to":"user","command":f"{command}","text":f"{parse_string(message)}"}))
    except Exception as e:
        print(f"ERROR PARSING MESSAGE \"{message} \": {e}")
        return(f"ERROR PARSING MESSAGE \"{message} \": {e}")

def parse_string(message ) -> str:
    if(message["command"] == "move-to"):
        try:
            x,y=int(message["x"]),int(message["y"])
            pyautogui.moveTo(x,y)
            print(x,y)
            return(f"Moved mouse to {x},{y}!")
        except Exception as e:
            print(f"pyautogui move-to error:{e}")
            return(f"pyautogui move-to error:{e}")
    elif(message["command"]=="screenshot"):
        photo = pyautogui.screenshot()
        photo=photo.resize((640,360))
        photo.save("ss.jpeg")
        output = BytesIO()
        photo.save(output, format='JPEG')
        im_data = output.getvalue()

        image_data = base64.b64encode(im_data).decode('utf-8')
        return(image_data)
    else:
        return(f"User passed invalid command!")

if __name__=="__main__":
    print(respond(r'{"from":"user","to":"client","password":"123","command":"screenshot"} '))