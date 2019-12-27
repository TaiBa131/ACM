/*
  ACM - Adaptive Classroom Manager - Early POC
  
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

void setup() {

  Serial.begin(9600); // Open serial with 9600 baud
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  LoadCell.begin(); // start hx711 connection 
  LoadCell.start(2000); // 2000 ms to stabilize but will be set to 0 when the tare offset is found
  LoadCell.setCalFactor(824); // Sets the calibration factor from the calibration sketch
  
  Serial.println("TareOffset: " + String(LoadCell.getTareOffset())); // prints the Tare Offset to be saved later, as the device WILL be rebooted randomly when finished
  
  // Sends a test message via the webhook over to Discord
  sendMessage("Hello World without TTS AT ALL");
}

void loop() {
  // This is the default DHCP maintain code from the Ethernet library.
  // It allows to reset / lease a new IP address WITHOUT "rebooting" the Arduino.
  dhcpCheck(); // TO BE REPLACED WITH Ethernet.maintain();
  
  weigh();

  delay(2500); // waits for a second and a half
}

void weigh() {
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // i= load cell value
  String text = "Weight[g]: " + String(i); // creates String from text + the Load Cell value
  Serial.println(text); // print out that String
}

void sendMessage(String content) {
  Serial.println("[HTTP] Connecting to Discord...");
  Serial.println("[HTTP] Message: " + content);
  Serial.println("[HTTP] TTS: " + discord_tts);
  http_client.post(discord_webhook, "application/json", "{\"content\":\"" + content + "\"" + "}");
  // read the status code and body of the response
  int statusCode = http_client.responseStatusCode();
  String response = http_client.responseBody();
  
  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  Serial.print("[HTTP] Response: ");
  Serial.println(response);
}

void dhcpCheck() {
  switch (Ethernet.maintain()) {
  case 1:
    //renewed fail
    Serial.println("Error: renewed fail");
    break;

  case 2:
    //renewed success
    Serial.println("Renewed success");
    //print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    break;

  case 3:
    //rebind fail
    Serial.println("Error: rebind fail");
    break;

  case 4:
    //rebind success
    Serial.println("Rebind success");
    //print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    break;

  default:
    //nothing happened
    break;
  }
}
