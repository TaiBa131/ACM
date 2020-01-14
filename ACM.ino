/*
  ACM - Adaptive Classroom Manager - Early POC - Without Electronics
*/

unsigned long prevTime=0;
unsigned long interval = 100;
int check=0;

void setup() {

  Serial.begin(9600); // Open serial with 9600 baud
  
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }

  Serial.println("My IP address");
  
  Serial.println("TareOffset: " + String(80)); // prints the Tare Offset to be saved later, as the device WILL be rebooted randomly when finished
  
  Serial.println("Hello World without TTS AT ALL"); // Sends a test message via the webhook over to Discord



  randomSeed(analogRead(0)); // Random seed for Random number
}



void loop() {
  weigh();
  delay(500); // waits for a second and a half
}



void weigh() {
  float i = LoadCellData();
  String text = "Weight[g]: " + String(i);
  Serial.println(text);

  checkWeight(i);
}

// This should increase check, if the time interval was right and the "load" under 10g
// If the check amount is equal to 5, it restes it, and prints something
void checkWeight(float i) {
  if(check==5) {
    check=0;
    Serial.println("This Works");
  } else if (int(i) >= 10) {
    check=0;
  } else if ( ((unsigned long) (millis() - prevTime) >= interval) && (int(i) < 10) ) {
    check++;
    prevTime = millis();
  }
}

int LoadCellData() {
  return random(0, 11);
}
