const express = require("express");
const fs = require("fs");
const app = express();
const config = JSON.parse(fs.readFileSync("config.json", "utf-8"));

const html_port = config.html_port;

app.get("/", (req, res) => {
    // Read your HTML file and replace placeholders with the WebSocket server IP
    let html = fs.readFileSync("public/index.html", "utf-8");
    html = html.replace("{{ip}}", config.ip);
    html = html.replace("{{websocket_port}}", config.websocket_port);


    // Serve the modified HTML file
    res.send(html);
});

app.listen(html_port, () => {
    console.log(`Server is running on http://localhost:${html_port}`);
});

const WebSocket = require("ws");
const server = new WebSocket.Server({ port: config.websocket_port });

const connectedSockets = new Map(); //socket:socket, user/client

server.on("connection", (socket) => {
    console.log("A socket has initialized a connection!");

    socket.on("message", (str_message) => {
        console.log("Received message:");
        //connectedSockets.forEach((name,socket) => {
        //    console.log(name)
        //});
        try {
            message = JSON.parse(str_message);
            if (message.command != "photo") {
                console.log(str_message.toString())
            }
            else {
                console.log("photo from client to all users")
            }
            //str_message2=JSON.stringify(message)
            //console.log("str_message2"==="str_message2")
            if (message.from == "client" || message.password == "123") {
                if (message.command == "register-user") {
                    if (!connectedSockets.has(socket)) {
                        connectedSockets.set(socket, "user");
                        socket.send(
                            '{"from":"server","to":"user","command":"log","text":"Confirmed login!"}'
                        );
                    }
                } else if (message.command == "register-client") {
                    connectedSockets.set(socket, "client");
                } else if (
                    message.command == "log" ||
                    message.command == "photo"
                ) {
                    broadcast(JSON.stringify(message), "user");
                } else {
                    if (message.to == "server") {
                        console.log(
                            "Recieved message pointed towards server but it got through guards: " +
                                str_message
                        );
                    } else {
                        console.log("broadcasting command to " + message.to);
                        broadcast(JSON.stringify(message), message.to);
                    }
                }
            } else {
                console.log("wrong pass");
                socket.send(
                    '{"from":"server","to":"user","command":"log","text":"wrong password!"}'
                );
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
