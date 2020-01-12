/*
  ACM - Adaptive Classroom Manager - Early POC - Without Electronics
*/

unsigned long prevTime=0;
unsigned long interval = 500;
int check=0;

void setup() {

  Serial.begin(9600); // Open serial with 9600 baud
  
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }

  Serial.print("My IP address");
  
  Serial.println("TareOffset: " + String(80)); // prints the Tare Offset to be saved later, as the device WILL be rebooted randomly when finished
  
  sendMessage("Hello World without TTS AT ALL"); // Sends a test message via the webhook over to Discord




  randomSeed(analogRead(0)); // Random seed for Random number
}



void loop() {
  //weigh();
  Serial.println(LoadCellData());
  delay(500); // waits for a second and a half
}



void weigh() {
  float i = LoadCellData();
  String text = "Weight[g]: " + String(i);
  Serial.println(text);

  checkWeight();
}


// Has not been tested
// This should increase check, if the time interval was right and the "load" under 10g
// If the check amount is over 5, it restes it, and prints something
void checkWeight() {
  if((millis() - prevTime >= interval) && (LoadCellData()<=10)){
    check++;
  } else if(check>5) {
    check=0;
    Serial.println("This Works");
  }
}



void sendMessage(String content) {
  Serial.println("[HTTP] Connecting to Discord...");
  Serial.println("[HTTP] Message: " + content);
  Serial.println("https://discordapp.com/webhook " "application/json " "{\"content\":\"" + content + "\"" + "}");
  
  int statusCode = 204;
  
  Serial.print("[HTTP] Status code: ");
  Serial.println(statusCode);
  
  if(statusCode != 204) {
    Serial.print("[HTTP] Response: ");
    Serial.println("Error");
  }
}



int LoadCellData() {
  return random(0, 11);
}
