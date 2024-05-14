#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 26

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  USBSerial.begin(9600);
}

void loop() {
  Wire.beginTransmission(0x30);
  Wire.endTransmission();
  
  delay(100);
  Wire.requestFrom(0x30, 2);
  if (Wire.available() == 2) {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    int sensorValue = ((msb << 8) | lsb);
    USBSerial.print("Sensor Value: ");
    USBSerial.println(sensorValue);

    
    float ammoniaConcentration = sensorValue * 0.1; // Example conversion factor
    
    USBSerial.print("Ammonia Concentration (ppm): ");
    USBSerial.println(ammoniaConcentration);
  }
  delay(1000);
}
