//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
//-------------------------------------------------------------------------------------
// Calibration Sketch ( https://github.com/olkal/HX711_ADC/blob/master/examples/Calibration/Calibration.ino )
// !!Adapted to my Setup!!
//
//MIT License
//
//Copyright (c) 2017 Olav Kallhovd
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//
// Calibrated using a swiss army knife that weighs around 100-101g, which is CURRENTLY precise enough for me
// Then used that number in my code, as i prefer NOT saving in on the EEPROM
// This is added to the repo to show what i used 

#include <HX711_ADC.h>
#include <EEPROM.h>

//HX711 constructor (dout pin, sck pin):
HX711_ADC LoadCell(3, 4);

int eepromAdress = 0;

long t;

void calibrate() {
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("It is assumed that the mcu was started with no load applied to the load cell.");
  Serial.println("Now, place your known mass on the loadcell,");
  Serial.println("then send the weight of this mass (i.e. 100.0) from serial monitor.");
  float m = 0;
  boolean f = 0;
  while (f == 0) {
    LoadCell.update();
    if (Serial.available() > 0) {
      m = Serial.parseFloat();
      if (m != 0) {
        Serial.print("Known mass is: ");
        Serial.println(m);
        f = 1;
      }
      else {
        Serial.println("Invalid value");
      }
    }
  }
  float c = LoadCell.getData() / m;
  LoadCell.setCalFactor(c);
  Serial.print("Calculated calibration value is: ");
  Serial.print(c);
  Serial.println(", use this in your project sketch");
  f = 0;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(eepromAdress);
  Serial.println("? y/n");
  while (f == 0) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        #if defined(ESP8266) 
        EEPROM.begin(512);
        #endif
        EEPROM.put(eepromAdress, c);
        #if defined(ESP8266)
        EEPROM.commit();
        #endif
        EEPROM.get(eepromAdress, c);
        Serial.print("Value ");
        Serial.print(c);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(eepromAdress);
        f = 1;

      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        f = 1;
      }
    }
  }
  Serial.println("End calibration");
  Serial.println("For manual edit, send 'c' from serial monitor");
  Serial.println("***");
}

void changeSavedCalFactor() {
  float c = LoadCell.getCalFactor();
  boolean f = 0;
  Serial.println("***");
  Serial.print("Current value is: ");
  Serial.println(c);
  Serial.println("Now, send the new value from serial monitor, i.e. 696.0");
  while (f == 0) {
    if (Serial.available() > 0) {
      c = Serial.parseFloat();
      if (c != 0) {
        Serial.print("New calibration value is: ");
        Serial.println(c);
        LoadCell.setCalFactor(c);
        f = 1;
      }
      else {
        Serial.println("Invalid value, exit");
        return;
      }
    }
  }
  f = 0;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(eepromAdress);
  Serial.println("? y/n");
  while (f == 0) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        #if defined(ESP8266)
        EEPROM.begin(512);
        #endif
        EEPROM.put(eepromAdress, c);
        #if defined(ESP8266)
        EEPROM.commit();
        #endif
        EEPROM.get(eepromAdress, c);
        Serial.print("Value ");
        Serial.print(c);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(eepromAdress);
        f = 1;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        f = 1;
      }
    }
  }
  Serial.println("End change calibration value");
  Serial.println("***");
}

void setup() {
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  LoadCell.begin();
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(1.0); // user set calibration value (float)
    Serial.println("Startup + tare is complete");
  }
  while (!LoadCell.update());
  calibrate();
}
void loop() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in sketch will reduce effective sample rate (be carefull with delay() in the loop)
  LoadCell.update();

  //get smoothed value from the data set
  if (millis() > t + 250) {
    float i = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(i);
    t = millis();
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
    else if (inByte == 'c') changeSavedCalFactor();
  }

  //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

}
