#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* htmlContent = R"(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 WebSocket</title>
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    
    window.addEventListener('load', onLoad);

    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage;
    }

    function onOpen(event) {
      console.log('Connection opened');
    }

    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      console.log(event.data);
      document.getElementById('sensor').innerHTML = event.data;
    }

    function onLoad(event) {
      initWebSocket();
    }
  </script>
</head>
<body>
  <h1>ESP32 WebSocket</h1>
  <p>Sensor Value: <span id="sensor">0</span></p>
</body>
</html>
)";

#endif // WEBPAGE_H
