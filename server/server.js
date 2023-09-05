const express = require('express');
const app = express();
const port = 3000
app.use(express.static('public'));

app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});


const WebSocket = require("ws");
const server = new WebSocket.Server({ port: 8080 });


const connectedSockets = new Map(); //socket:socket, user/client

server.on("connection", (socket) => {
    console.log("A socket has initialized a connection!");

    socket.on("message", (str_message) => {
        console.log("Received message:", str_message.toString());
        //connectedSockets.forEach((name,socket) => {
        //    console.log(name)
        //});
        try {
            message = JSON.parse(str_message);
            //str_message2=JSON.stringify(message)
            //console.log("str_message2"==="str_message2")
            if (message.from == "client" || message.password == "123") {
                if (message.command == "register-user") {
                    if (!connectedSockets.has(socket)) {
                        connectedSockets.set(socket, "user");
                        socket.send('{"from":"server","to":"user","command":"log","text":"Confirmed login!"}')
                    }
                } else if (message.command == "register-client") {
                    connectedSockets.set(socket, "client");
                } else if (message.command == "log" || message.command == "photo") {
                    broadcast(JSON.stringify(message), "user");
                } else {
                    if (message.to == "server") {
                        console.log(
                            "Recieved message pointed towards server but it got through guards: " +
                                str_message
                        );
                    } else {
                        console.log("broadcasting command to " + message.to)
                        broadcast(JSON.stringify(message), message.to);
                    }
                }
            } else {
                console.log("wrong pass");
                socket.send('{"from":"server","to":"user","command":"log","text":"wrong password!"}');
            }
        } catch (exception) {
            console.log("something went wrong in parsing: " + str_message);
            console.log(exception);
        }
    });

    socket.on("close", () => {
        console.log("A socket disconnected");

        // Remove the disconnected socket from the array
        if (connectedSockets.has(socket)) {
            connectedSockets.delete(socket);
        }
    });
});

function broadcast(str_message, str_recipient) {
    connectedSockets.forEach((type, socket) => {
        try {
            if (type == str_recipient) {
                socket.send(str_message.toString());
            }
        } catch {
            console.log("ERROR: couldn't send message to a socket!");
        }
    });
}
