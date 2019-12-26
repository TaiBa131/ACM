#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
HX711_ADC LoadCell(3, 4); // data pin + serial clock pin
void setup() {
  Serial.begin(9600); // opens serial with 9600 baud
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }
  
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(0); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(818); // Sets the calibration factor from the calibration sketch
  
  Serial.println(LoadCell.getTareOffset()); // prints the Tare Offset to be saved later, as the device WILL be rebooted randomly
}
void loop() {
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  String text = "Weight[g]: " + String(i); // creates String from text + the Load Cell value
  Serial.println(text); // print out that String

  delay(1500); // waits for a second and a half
}
