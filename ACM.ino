// Code based on Michael Schoeffler's OLED + HX711 code
// http://www.mschoeffler.de/2017/12/04/arduino-tutorial-hx711-load-cell-amplifier-weight-sensor-module-lcm1602-iic-v1-lcd/

// HX711_ADC HAS A CHANGED config.h (#define SAMPLES 2)
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC

HX711_ADC LoadCell(3, 4); // data pin + serial clock pin
void setup() {
  Serial.begin(9600); // opens serial with 9600 baud
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }
  
  LoadCell.begin(); // start hx711 connection 
  LoadCell.start(2000); // 2000 ms to stabilize but will be set to 0 when the tare offset is found
  LoadCell.setCalFactor(818); // Sets the calibration factor from the calibration sketch
  
  Serial.println(LoadCell.getTareOffset()); // prints the Tare Offset to be saved later, as the device WILL be rebooted randomly when finished
}
void loop() {
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // i= load cell value
  String text = "Weight[g]: " + String(i); // creates String from text + the Load Cell value
  Serial.println(text); // print out that String

  delay(1500); // waits for a second and a half
}
