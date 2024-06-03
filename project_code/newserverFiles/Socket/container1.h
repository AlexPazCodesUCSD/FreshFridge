#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* htmlContent1 = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Food Detail</title>
    <style>
        body {
          font-family: Arial, sans-serif;
          background-color: #f0f0f0;
          color: #333;
          margin: 0;
          padding: 0;
          display: flex;
          flex-direction: column;
          align-items: center;
        }
        h1 {
          background-color: #4CAF50;
          color: white;
          padding: 20px;
          width: 100%;
          text-align: center;
          margin: 0;
          box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .container {
          display: flex;
          justify-content: space-around;
          width: 80%;
          margin-top: 20px;
          background-color: white;
          border-radius: 10px;
          box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
          overflow: hidden;
        }
        .column {
          flex: 1;
          padding: 20px;
          text-align: center;
        }
        .column h2 {
          color: #4CAF50;
        }
        .column img {
          max-width: 100%;
          border-radius: 10px;
        }
        .center {
          display: flex;
          flex-direction: column;
          align-items: center;
          width: 80%;
          margin-top: 20px;
          background-color: white;
          padding: 20px;
          border-radius: 10px;
          box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
          text-align: center;
        }
        .center img {
          max-width: 100px;
        }
        .slider-container {
          display: flex;
          flex-direction: column;
          align-items: center;
          margin-top: 20px;
        }
        .slider-container input[type="range"] {
          width: 100%;
        }
        .footer {
          margin-top: 20px;
          padding: 10px;
          background-color: #4CAF50;
          color: white;
          text-align: center;
          width: 100%;
          box-shadow: 0 -2px 4px rgba(0, 0, 0, 0.1);
        }
        button {
          padding: 10px 20px;
          background-color: #4CAF50;
          color: white;
          border: none;
          border-radius: 5px;
          cursor: pointer;
          margin-top: 20px;
        }
        button:hover {
          background-color: #45a049;
        }
        .healthy {
          color: green;
        }
        .spoiling {
          color: red;
        }
    </style>
</head>
<body>
    <h1>Food Detail</h1>
    <h2>Container #1</h2>

    <div class="container">
        <div class = "column">
            <h2>Health Status</h2>
            <p id="healthStatus" class = "healthy">Healthy</p>

            <h2>Sensor Data</h2>
            <p>Ammonia Sensor: <span id="sensor1">0</span></p>
            <p>CO2 Sensor: <span id="sensor2">0</span></p>
            <p>Ethanol Sensor: <span id="sensor3">0</span></p>
            <p>Humidity Sensor: <span id="sensor4">0</span></p>
            <div class="slider-container">
              <p>Update Interval: <span id="sliderValue">1000</span> ms</p>
              <input type="range" id="slider" min="500" max="5000" value="1000">
            </div>
        </div>

        <div class="column">
          <h2 id="objectDetail">Produce Name</h2>
          <img id="produce-img" src="https://images.immediate.co.uk/production/volatile/sites/30/2017/01/Bunch-of-bananas-67e91d5.jpg?quality=90&resize=440,400" alt="Produce Image">
        </div>
    </div>

    <button id="deleteButton">Delete</button>
    
    <script>
        // Get object ID from URL parameters
        const urlParams = new URLSearchParams(window.location.search);
        const objectId = urlParams.get('id');
        
        // Load object from localStorage
        const objects = JSON.parse(localStorage.getItem('objects')) || [];
        const object = objects.find(obj => obj.id == objectId);
        
        // Display object details
        const objectDetail = document.getElementById('objectDetail');
        objectDetail.innerHTML = `<p>Name: ${object.name}</p>`;
        
        // Handle delete button click
        document.getElementById('deleteButton').addEventListener('click', function() {
            const index = objects.findIndex(obj => obj.id == objectId);
            objects.splice(index, 1);
            localStorage.setItem('objects', JSON.stringify(objects));
            window.location.href = 'webpage.html';
        });

        // Function to update sensor data from localStorage
        function updateSensorData() {
            const sensorData = JSON.parse(localStorage.getItem('sensorData'));
            if (sensorData) {
                document.getElementById('sensor1').innerHTML = sensorData.sensorValue1;
                document.getElementById('sensor2').innerHTML = sensorData.sensorValue2;
                document.getElementById('sensor3').innerHTML = sensorData.sensorValue3;
                document.getElementById('sensor4').innerHTML = sensorData.sensorValue4;
                
                updateHealthStatus(sensorData);
            }
        }

        // Function to update health status based on sensor data
        function updateHealthStatus(sensorData) {
            const healthStatus = document.getElementById('healthStatus');
            const { sensorValue1, sensorValue2, sensorValue3, sensorValue4 } = sensorData;

            if (sensorValue1 > 500 || sensorValue2 > 222 || sensorValue3 > 366 || sensorValue4 > 9000) {
                healthStatus.textContent = 'Spoiling';
                healthStatus.classList.remove('healthy');
                healthStatus.classList.add('spoiling');
            } else {
                healthStatus.textContent = 'Healthy';
                healthStatus.classList.remove('spoiling');
                healthStatus.classList.add('healthy');
            }
        }

        // Update sensor data every second
        setInterval(updateSensorData, 1000);

        // Initial call to display sensor data
        updateSensorData();
    </script>
</body>
</html>
)";

#endif // WEBPAGE_H
