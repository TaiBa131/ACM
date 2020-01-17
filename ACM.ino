/*
  ACM - Adaptive Classroom Manager - Early POC$
  This is currently "based" on the Ethernet library's DHCP-based IP printer sketch, @maditnerd implementation of sending Discord messages, aswell as Michael Schoeffler's OLED + HX711 code.
  https://github.com/arduino-libraries/Ethernet/blob/master/examples/DhcpAddressPrinter/DhcpAddressPrinter.ino
  https://github.com/maditnerd/discord_test/blob/master/discord_test_mkr1010/discord.h
  http://www.mschoeffler.de/2017/12/04/arduino-tutorial-hx711-load-cell-amplifier-weight-sensor-module-lcm1602-iic-v1-lcd/
  
  Knowing pretty much nothing in electronics, arduino, the libraries i used, or even C++ (We ONLY cover JAVA at school), i looked up a lot of things online, and adapted the examples to fit my goal.
*/

#include <Ethernet.h>
#include <ArduinoHttpClient.h>
  // HX711_ADC HAS A CHANGED config.h (#define SAMPLES 2)
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include "secrets.h"

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 // MAC address
};

const char server[] = SECRET_SERVER; // domain of my web server / reverse proxy
const int port = 80; // HTTP + NGINX, because not powerful enough

EthernetClient client;
HttpClient http_client = HttpClient(client, server, port);

HX711_ADC LoadCell(3, 4); // data pin + serial clock pin

unsigned long prevTime= 0;
unsigned long interval = 700;
int check=0;

void setup() {

  Serial.begin(9600); // Open serial with 9600 baud
  
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }

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

  LoadCell.begin(); // start hx711 connection 
  LoadCell.start(2000); // 2000 ms to stabilize but will be set to 0 when the tare offset is found
  LoadCell.setCalFactor(4110.26); // Calibration factor from the calibration sketch

  randomSeed(analogRead(0)); // Random seed for Random number

  Serial.println("Setup successful.");
}



void loop() {
  Ethernet.maintain();
  weigh();
  // delay(500); // waits for a second and a half
}

void updateData() {
  for (int tmp = 0; tmp < 16; tmp++) {
    LoadCell.update();
    delay(33);
  }
}

void weigh() {
  updateData();
  int i = LoadCell.getData();
  String text = "Weight[g]: " + String(i);
  Serial.print(text); // !!! Change to println !!!

  checkWeight(i, text);
}

// If the load is under 10g, its in the interval and if has successfully checked 4 times,
// the Weight has been under 10 for 5 checks, so it sends a message and resets the check amount
//
// Otherwise, if its not under 10, or its not the interval YET,
// it check if its over 10, and if its the case,
// it resets it.
void checkWeight(float i, String text) {
  if( (int(i) < 10) && ((unsigned long) (millis() - prevTime) >= interval)) {
    if(check==4) {
      check=0;
      Serial.println("            Sending Message here");         //Not Needed
    } else {
      check++;
      Serial.println(" (check++)  " + String(check) + "   / 5");  //Not Needed
      prevTime = millis();
    }
  } else if (int(i)>=10) {
    check=0;
    Serial.println("           0     0");                         //Not Needed
  } else {                                                        //Not Needed
    Serial.println("");                                           //Not Needed
  }
}
