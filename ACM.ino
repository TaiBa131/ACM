/*
  ACM - Adaptive Classroom Manager - Early POC - Without Electronics
*/

unsigned long prevTime= 0;
unsigned long interval = 700;
int check=0;

void setup() {

  Serial.begin(9600); // Open serial with 9600 baud
  
  while (!Serial) {
    ; // wait for serial port to connect, needed for testing, otherwise its useless
  }
  randomSeed(analogRead(0)); // Random seed for Random number

  Serial.println("Setup successful.");
}



void loop() {
  weigh();
  delay(500); // waits for a second and a half
}



void weigh() {
  float i = LoadCellData();
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
void checkWeight(int i, String text) {
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

int LoadCellData() {
  return random(0, 11);
}
