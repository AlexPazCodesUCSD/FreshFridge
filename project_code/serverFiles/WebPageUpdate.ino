/*


  this example will show
  1. how to use and ESP 32 for reading pins
  2. building a web page for a client (web browser, smartphone, smartTV) to connect to
  3. sending data from the ESP to the client to update JUST changed data
  4. sending data from the web page (like a slider or button press) to the ESP to tell the ESP to do something

  If you are not familiar with HTML, CSS page styling, and javascript, be patient, these code platforms are
  not intuitive and syntax is very inconsitent between platforms

  I know of 4 ways to update a web page
  1. send the whole page--very slow updates, causes ugly page redraws and is what you see in most examples
  2. send XML data to the web page that will update just the changed data--fast updates but older method
  3. JSON strings which are similar to XML but newer method
  4. web sockets very very fast updates, but not sure all the library support is available for ESP's

  I use XML here...

  compile options
  1. esp32 dev module
  2. upload speed 921600
  3. cpu speed 240 mhz
  flash speed 80 mhz
  flash mode qio
  flash size 4mb
  partition scheme default


  NOTE if your ESP fails to program press the BOOT button during programm when the IDE is "looking for the ESP"

  The MIT License (MIT)

  code writen by Kris Kasprzak
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  On a personal note, if you develop an application or product using this code 
  and make millions of dollars, I'm happy for you!

*/

#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code
#include <Wire.h>

// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
#define USE_INTRANET

// replace this with your homes intranet connect parameters
#define LOCAL_SSID "BryanAndTheBeanStalk"
#define LOCAL_PASS "jollyapple802"

// once  you are read to go live these settings are what you client will connect to
#define AP_SSID "TestFreshFridge"
#define AP_PASS "1234"

// start your defines for pins for sensors, outputs etc.
/*#define PIN_OUTPUT 26 // connected to nothing but an example of a digital write from the web page
#define PIN_FAN 27    // pin 27 and is a PWM signal to control a fan speed
#define PIN_LED 2     //On board LED*/

#define micPin 1
#define MQ4Pin 2

//#define PIN_A0 34     // sensor1
//#define SDA_PIN 21
//#define SCL_PIN 26
//#define PIN_A1 35     // sensor2

// variables to store measure data and sensor states
int AmmoniaValue = 0, micValue = 0, MQ4Value = 0; //BitsA1 = 0;
float ammoniaConcentration = 0; //VoltsA1 = 0;
//bool SomeOutput = false;
uint32_t SensorUpdate = 0;
byte Ammonia_msb, Ammonia_lsb;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[2048];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

void setup() {

  // standard stuff here
  //Wire.begin(SDA_PIN, SCL_PIN);
  USBSerial.begin(115200);
  
  //USBSerial.begin(115200);

  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  //  disableCore1WDT();

  // just an update to progress
  USBSerial.println("starting server");

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USBSerial.print(".");
  }
  USBSerial.print("IP address: "); USBSerial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  USBSerial.print("IP address: "); USBSerial.println(Actual_IP);
#endif

  printWifiStatus();


  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  // upon ESP getting /UPDATE_SLIDER string, ESP will execute the UpdateSlider function
  // same notion for the following .on calls
  // add as many as you need to process incoming strings from your web page
  // as you can imagine you will need to code some javascript in your web page to send such strings
  // this process will be documented in the SuperMon.h web page code
  server.on("/ProcessMic", ProcessMic);
  server.on("/ProcessMQ4", ProcessMQ4);
  /*server.on("/BUTTON_0", ProcessButton_0);
  server.on("/BUTTON_1", ProcessButton_1);*/

  // finally begin the server
  server.begin();

}

void loop() {

  // you main loop that measures, processes, runs code, etc.
  // note that handling the "on" strings from the web page are NOT in the loop
  // that processing is in individual functions all managed by the wifi lib

  // in my example here every 50 ms, i measure some analog sensor data (my finger dragging over the pins
  // and process accordingly
  // analog input can be from temperature sensors, light sensors, digital pin sensors, etc.
  /*Wire.beginTransmission(0x30);
  Wire.endTransmission();
  
  delay(100);
  Wire.requestFrom(0x30, 2);
  if (Wire.available() == 2) {
    ammonia_msb = Wire.read();
    ammonia_lsb = Wire.read();

    int ammonia_value = ((ammonia_msb << 8) | ammonia_lsb);
    USBSerial.print("Sensor Value: ");
    USBSerial.println(ammonia_value); 

    
    float ammonia_concentration = ammonia_value * 0.1; // Example conversion factor
    
    USBSerial.print("Ammonia Concentration (ppm): ");
    USBSerial.println(ammonia_concentration);
  }
  delay(1000);*/

  if ((millis() - SensorUpdate) >= 50) {
    USBSerial.println("Reading Sensors");
    SensorUpdate = millis();
    micValue = analogRead(micPin);
    MQ4Value = analogRead(MQPin);
    //BitsA1 = analogRead(PIN_A1);

    // standard converion to go from 12 bit resolution reads to volts on an ESP
    //VoltsA1 = BitsA1 * 3.3 / 4096;

  }

  // no matter what you must call this handleClient repeatidly--otherwise the web page
  // will not get instructions to do something
  server.handleClient();

}


// function managed by an .on method to handle slider actions on the web page
// this example will get the passed string called VALUE and conver to a pwm value
// and control the fan speed
//void UpdateSlider() {

 /* // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("sensor0_value");

  // conver the string sent from the web page to an int
  sensor0 = t_state.toInt();
  Serial.print("UpdateSlider"); Serial.println(FanSpeed);
  // now set the PWM duty cycle
  ledcWrite(0, FanSpeed);*/


  // YOU MUST SEND SOMETHING BACK TO THE WEB PAGE--BASICALLY TO KEEP IT LIVE

  // option 1: send no information back, but at least keep the page live
  // just send nothing back
  // server.send(200, "text/plain", ""); //Send web page

  // option 2: send something back immediately, maybe a pass/fail indication, maybe a measured value
  // here is how you send data back immediately and NOT through the general XML page update code
  // my simple example guesses at fan speed--ideally measure it and send back real data
  // i avoid strings at all caost, hence all the code to start with "" in the buffer and build a
  // simple piece of data

  /*strcpy(buf, "");
  sprintf(buf, "%d", FanRPM);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page

}*/

// now process button_0 press from the web site. Typical applications are the used on the web client can
// turn on / off a light, a fan, disable something etc
void ProcessMic() {

  // regardless if you want to send stuff back to client or not
  // you must have the send line--as it keeps the page running
  // if you don't want feedback from the MCU--or let the XML manage
  // sending feeback

  // option 1 -- keep page live but dont send any thing
  // here i don't need to send and immediate status, any status
  // like the illumination status will be send in the main XML page update
  // code
  strcpy(buf, "");
  sprintf(buf, "%d", micValue);
  sprintf(buf, buf);
  server.send(200, "text/plain", "buf"); //Send web page

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  /*
    if (LED0) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
    */
}

void ProcessMQ4() {

  // regardless if you want to send stuff back to client or not
  // you must have the send line--as it keeps the page running
  // if you don't want feedback from the MCU--or let the XML manage
  // sending feeback

  // option 1 -- keep page live but dont send any thing
  // here i don't need to send and immediate status, any status
  // like the illumination status will be send in the main XML page update
  // code
  strcpy(buf, "");
  sprintf(buf, "%d", MQ4Value);
  sprintf(buf, buf);
  server.send(200, "text/plain", "buf"); //Send web page

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  /*
    if (LED0) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
    */
}



// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {

  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);

}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML() {

  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  // send mic value
  sprintf(buf, "<B1>%d</B1>\n", micValue);
  strcat(XML, buf);

  
  sprintf(bug,"<B2>%d</B2>\n", MQ4Value);
  strcat(XML,buf);

  /*if (SomeOutput) {
    strcat(XML, "<SWITCH>1</SWITCH>\n");
  }
  else {
    strcat(XML, "<SWITCH>0</SWITCH>\n");
  }*/

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  USBSerial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);


}

// I think I got this code from the wifi example
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  USBSerial.print("SSID: ");
  USBSerial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  USBSerial.print("IP Address: ");
  USBSerial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  USBSerial.print("signal strength (RSSI):");
  USBSerial.print(rssi);
  USBSerial.println(" dBm");
  // print where to go in a browser:
  USBSerial.print("Open http://");
  USBSerial.println(ip);
}

// end of code
