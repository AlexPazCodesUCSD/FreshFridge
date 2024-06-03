var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

const API_KEY = 'api-key';

//
function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
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
  var data = JSON.parse(event.data);

  // Store sensor data in localStorage
  localStorage.setItem('sensorData', JSON.stringify(data));
}

function onLoad(event) {
  initWebSocket();
  document.getElementById('generateRecipe').addEventListener('click', generateRecipe);

  // Load objects from localStorage on page load
  const objects = JSON.parse(localStorage.getItem('objects')) || [];
  objects.forEach(addObjectToList);
}


// Function to add object to the list
function addObjectToList(object) {
  const objectList = document.getElementById('objectList');
  const objectRow = document.createElement('div');
  objectRow.classList.add('object-row');
  objectRow.textContent = `Container ${object.containerNum}: ${object.name}`;

  // Add event listener based on container number
  objectRow.addEventListener('click', function() {
    window.location.href = `container${object.containerNum}_html.html?id=${object.id}`;
  });

  objectList.appendChild(objectRow);
}

// Handle form submission
document.getElementById('objectForm').addEventListener('submit', function(event) {
  event.preventDefault();

  // Get form data
  const name = document.getElementById('objectName').value;
  const containerNum = document.getElementById('containerNum').value;
  const errorMessage = document.getElementById('error-message');

  // Check if containerNum is between 1 and 4
  if (containerNum < 1 || containerNum > 4) {
    errorMessage.textContent = 'Container number must be between 1 and 4';
    errorMessage.style.display = 'block';
    return;
  }

  // Hide error message if validation passes
  errorMessage.style.display = 'none';

  // Create new object
  const newObject = {
    id: Date.now(), // Unique ID based on timestamp
    name: name,
    containerNum: containerNum,
  };

  // Store object in localStorage (optional, for persistence)
  let objects = JSON.parse(localStorage.getItem('objects')) || [];

  // Check for existing object in the same container
  const existingIndex = objects.findIndex(obj => obj.containerNum == containerNum);
  if (existingIndex !== -1) {
    // Replace the existing object with the new object
    objects[existingIndex] = newObject;
  } else {
    // Add new object to the array
    objects.push(newObject);
  }
  
  // Sort objects by containerNum in ascending order
  objects.sort((a, b) => a.containerNum - b.containerNum);

  localStorage.setItem('objects', JSON.stringify(objects));

  // Clear and re-render the object list
  objectList.innerHTML = '';
  objects.forEach(addObjectToList);

  // Clear form
  document.getElementById('objectForm').reset();
});

async function generateRecipe() {
  const responseTextarea = document.getElementById('recipe-output');
  try {
    // Retrieve objects from localStorage
    const objects = JSON.parse(localStorage.getItem('objects')) || [];

    // Extract names of the foods
    const foodNames = objects.map(obj => obj.name).join(', ');

    // Construct the message for the API request
    const messageContent = `Generate a very simple recipe using ${foodNames}`;

    const response = await fetch('https://api.openai.com/v1/chat/completions', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${API_KEY}`,
      },
      body: JSON.stringify({
        model: 'gpt-3.5-turbo',
        messages: [{ role: 'user', content: messageContent }],
        temperature: 1.0,
        top_p: 0.7,
        n: 1,
        stream: false,
        presence_penalty: 0,
        frequency_penalty: 0,
      }),
    });

    if (response.ok) {
      const data = await response.json();
      responseTextarea.value = data.choices[0].message.content;
      responseTextarea.classList.add('show');
    } else {
      responseTextarea.value = 'Try Again';
    }
  } catch (error) {
    console.error(error);
    responseTextarea.value = 'Try Again';
  }
}

