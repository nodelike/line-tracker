#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

const char* AP_SSID = "ESP8266";     
const char* AP_PWD = "123456789"; 

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);


const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robot Controller</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }

        .container {
            padding: 20px;
        }

        .input-group {
            margin-bottom: 10px;
        }

        label {
            display: block;
            margin-bottom: 5px;
        }

        input[type="text"], button {
            padding: 10px;
            width: 100%;
            margin-bottom: 10px;
        }

        button {
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Robot PID Controller</h1>
        
        <div class="input-group">
            <label for="kp">Kp:</label>
            <input type="text" id="kp" placeholder="Enter Kp value">
        </div>

        <div class="input-group">
            <label for="ki">Ki:</label>
            <input type="text" id="ki" placeholder="Enter Ki value">
        </div>

        <div class="input-group">
            <label for="kd">Kd:</label>
            <input type="text" id="kd" placeholder="Enter Kd value">
        </div>

        <button onclick="sendPID()">Set PID Values</button>
        <button onclick="sendCommand('START')">Start Tracking</button>
        <button onclick="sendCommand('STOP')">Stop Tracking</button>
        <button onclick="sendCommand('CALIBRATE')">Calibrate</button>
    </div>

    <script>
        var connection = new WebSocket('ws://' + window.location.hostname + ':81/');

        connection.onopen = function () {
            console.log('Connection open!');
        }

        connection.onerror = function (error) {
            console.log('WebSocket Error ', error);
        };

        connection.onmessage = function (e) {
            console.log('Server: ', e.data);
        };

        function sendPID() {
            var kp = document.getElementById('kp').value;
            var ki = document.getElementById('ki').value;
            var kd = document.getElementById('kd').value;

            connection.send('Kp:' + kp);
            connection.send('Ki:' + ki);
            connection.send('Kd:' + kd);
        }

        function sendCommand(command) {
            connection.send(command);
        }
    </script>
</body>
</html>


)rawliteral";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT) {
    String text = String((char *) &payload[0]);

    String message = text;
    webSocket.sendTXT(num, message);
    Serial.println(text);
  }
}


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PWD);


  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}


void loop() {
  server.handleClient();
  webSocket.loop();

  delay(500);
}