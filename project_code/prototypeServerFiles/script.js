var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

const API_KEY = 'API-KEY';

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
  var data = JSON.parse(event.data);

  // Update sensor values
  document.getElementById('sensor1').innerHTML = data.sensor1;
  document.getElementById('sensor2').innerHTML = data.sensor2;
  document.getElementById('sensor3').innerHTML = data.sensor3;
  document.getElementById('sensor4').innerHTML = data.sensor4;

  // Update produce information
  document.getElementById('produce').innerHTML = data.produce;
  document.getElementById('produce-img').src = data.produceImg;

  // Update food health indicator
  document.getElementById('health-rating').innerHTML = data.healthRating;
  document.getElementById('health-status').innerHTML = data.healthStatus;
  document.getElementById('health-img').src = data.healthImg;
}

function onLoad(event) {
  initWebSocket();
  document.getElementById('slider').addEventListener('input', onSliderChange);
  document.getElementById('generateRecipe').addEventListener('click', generateRecipe);
}

function onSliderChange(event) {
  var sliderValue = document.getElementById('slider').value;
  document.getElementById('sliderValue').innerHTML = sliderValue;
  websocket.send(sliderValue);
}

async function generateRecipe() {
  const responseTextarea = document.getElementById('recipe-output')
  try {
    const response = await fetch('https://api.openai.com/v1/chat/completions', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${API_KEY}`,
        },
        body: JSON.stringify({
            model: 'gpt-3.5-turbo',
            messages: [{ role: 'assistant', content: 'Generate a very simple recipe using Bananas' }],
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
    