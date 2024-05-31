#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* htmlContent = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Fresh Fridge - Food Health Monitor</title>
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
    #recipe-output {
      width: 400%;
      margin: 0 auto;
      border: none;
      font-size: 16px;
      font-family: Arial, sans-serif;
      text-align: left;
      resize: none;
      margin-right: -50%;
      opacity: 0; /* Start with opacity set to 0 to make it invisible */
      transition: opacity 1s ease-in-out; /* Apply transition effect on opacity */
    }
    #recipe-output.show {
    opacity: 1; /* Change opacity to 1 to make it visible */
    }
  </style>
</head>
<body>
  <h1>Fresh Fridge - Food Health Monitor</h1>

  <h2>Your Food</h2>
  <div id="objectList"></div>

  <h2>Add Food</h2>
<form id="objectForm">
    <label for="objectName">Food Name:</label>
    <input type="text" id="objectName" name="objectName" required>
    <br>
    <label for="containerNum">Container:</label>
    <input type="number" id="containerNum" name="containerNum" required>
    <br>
    <button type="submit">Add food</button>
</form>
<div id="error-message" style="color: red; display: none;"></div>


  <div class="center">
    <div class="info">
      <h2>Food Health Indicator</h2>
      <p id="health-status">Health Status</p>
      <img id="health-img" src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSQWN-SLzk5eeEuA9zBJKzsM0qbvtLsKDfJ-w&s" alt="Health Status Image">
    </div>
    <button id="generateRecipe">Generate Recipe</button>

    <div style="text-align: center;">
      <h2>Recipe:</h2>
      <textarea id="recipe-output" rows="20" readonly></textarea>
  </div>
  
  </div>
  <div class="footer">
    ECE 196 Spring 2024 - Team 9: Food Health Monitor
  </div>
  
  <script src="script.js"></script>
</body>
</html>
)";

#endif // WEBPAGE_H
