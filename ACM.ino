/*
  ACM - Adaptive Classroom Manager - Early POC
  https://github.com/TaiBa131/ACM
*/

#include <Ethernet.h>
  // HX711_ADC HAS A CHANGED config.h (#define SAMPLES 2)
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <ArduinoMqttClient.h>
#include "secrets.h"

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 // MAC address
};

const char  broker[] = SECRET_SERVER;
int         port = SECRET_PORT;
String classroom = SECRET_CLASSROOM;
String site = SECRET_SITE;

const int lcFactor = 819;
const float lcTare = 8570568;

const int sensorPin = A5;
const int VIN = 5;
const int R = 10000;

int sensorVal=0;
int lux=0;

EthernetClient client;
MqttClient mqtt(client);
HX711_ADC LoadCell(3, 4); // data pin + serial clock pin

///////////////
//   SETUP   //
///////////////

void setup() {
  beginSerial();
  beginEthernet();
  beginMqtt();
  beginLoadCell();
  
  Serial.println("Setup successful.");
}

///////////////
//   LOOP    //
///////////////

void loop() {
  mqtt.poll();
  Ethernet.maintain();
  
  weight();
  light();
  delay(3000);
}

///////////////
//  VARIOUS  //
///////////////

void weight() {
  for(int i=0;i<=10;i++) {
    LoadCell.update();
    delay(20);
  }
  
  int load = LoadCell.getData();
  sendMessage(String(load), "box");
}

void light() {
  sendMessage(String(getLux()), "light");
}

void sendMessage(String content, String reason){
  String topic = (site + "/classroom/" + classroom + "/" + reason);
  Serial.println(topic + ": \"" + content + "\"" );
  mqtt.beginMessage(topic);
  mqtt.print(content);
  mqtt.endMessage();
}

int getLux() {
  sensorVal = analogRead(sensorPin);
  float Vout = float(sensorVal) * (VIN / float(1023));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}

/////////////////////
// SETUP FUNCTIONS //
/////////////////////

void beginSerial() {
  Serial.begin(9600); // Open serial with 9600 baud
  Serial.println("Serial is set up.");
}

void beginEthernet() {
  Serial.println("Getting IP address with DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (true) {
      delay(1);
    }
  }

  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void beginMqtt() {
  Serial.println("Connecting to the MQTT Broker");
  if (!mqtt.connect(broker, port)) {
    while (1);
  }
}

void beginLoadCell() {
  Serial.println("Starting the connection to the Load Cell");
  LoadCell.begin(); // start hx711 connection 
  LoadCell.start(0); // 2000 ms to stabilize but will be set to 0 when the tare offset is found
  LoadCell.setCalFactor(lcFactor); // Calibration factor from Serialthe calibration sketch
  LoadCell.setTareOffset(lcTare);
}
