/*
  ACM - Adaptive Classroom Manager - Early POC
  
  This is currently based on the Ethernet library's DHCP-based IP printer sketch, and @maditnerd implementation of sending Discord messages.
  Slightly modified.
*/

#include <Ethernet.h>

#include <ArduinoHttpClient.h>

#include "secrets.h"
// MAC address
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

// domain of my web server / reverse proxy
const char server[] = SECRET_SERVER;
// Port
const int port = 80; // HTTP + NGINX, because not powerful enough
// link to webhook / reverse proxy in http
//const String discord_webhook = "http://ltam.ftp.sh/api/webhooks/656949865611395113/5XRarsg5pI65gPRfhFzOLpaebYBuW6fyOnJhrrA-UU8eQguknar0wZcXAuFM166oouw6";
// Text To Speech
const String discord_tts = "false";

EthernetClient client;
HttpClient http_client = HttpClient(client, server, port);

void setup() {

  // Open serial communications and wait for port to open.
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
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

  // Sends a test message via the webhook over to Discord
  sendMessage("Hello World");

}

void loop() {
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
  
  void sendMessage(String content) {
    Serial.println("[HTTP] Connecting to Discord...");
    Serial.println("[HTTP] Message: " + content);
    Serial.println("[HTTP] TTS: " + discord_tts);
    http_client.post(discord_webhook, "application/json", "{\"content\":\"" + content + "\", \"tts\":" + discord_tts + "}");
    // read the status code and body of the response
    int statusCode = http_client.responseStatusCode();
    String response = http_client.responseBody();

    Serial.print("[HTTP] Status code: ");
    Serial.println(statusCode);
    Serial.print("[HTTP] Response: ");
    Serial.println(response);
  }
