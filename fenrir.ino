#define SWITCHES 6

// Our brand of relays are on when LOW, off when HIGH.
#define R_ON 0
#define R_OFF 1

// Pin numbers for each switch: x2 because Arduino Mega has pins
// that are two-wide.
int pSwitch[SWITCHES] =  {40,42,44,46,48,50};
int pRingLED[SWITCHES] = {41,43,45,47,49,51};
int pRelay[SWITCHES] =   {22,24,26,28,30,32};

int val;
int val2;

// ringState stores the on/off status of the ring LEDs.
int ringState[SWITCHES];

void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);

  // Loop for each of the six switches:
  for (int i=0; i<SWITCHES; i++) {
    // setup ring LEDs: output and ready to be pushed
    pinMode(pRingLED[i], OUTPUT);
    ringState[i] = HIGH;

    // ensure relays are off at startup:
    digitalWrite(pRelay[i], R_OFF);
    pinMode(pRelay[i], OUTPUT);
  }

  initializeRingLEDs();
}

void initializeRingLEDs() {

  for (int j=0; j<2; j++) {
    for (int i=0; i<SWITCHES; i++) {
      digitalWrite(pRingLED[i], HIGH);
    }
    delay(100);
    for (int i=0; i<SWITCHES; i++) {
      digitalWrite(pRingLED[i], LOW);
    }
    delay(100);
  }
  delay(500);

  for (int j=0; j<3; j++) {
    for (int i=SWITCHES-1; i>=0; i--) {
      digitalWrite(pRingLED[i], HIGH);
      delay(25); // - j*5);
      digitalWrite(pRingLED[i], LOW);
      delay(75); // 150 - j*50);
    }
  }
  delay(1500);

  for (int i=0; i<SWITCHES; i++) {
    digitalWrite(pRingLED[i], HIGH);
    delay(100);
  }
}


// Main Arduino Event Loop
void loop() {
  for (int s=0; s<SWITCHES; s++) {
    checkSwitch(s);
  }
  delay(10);
}


void checkSwitch(int s) {
  val = digitalRead(pSwitch[s]);

  if (val==HIGH) {
    toggleRingLED(s);
    digitalWrite(pRelay[s], R_ON);
  } else {
    digitalWrite(pRelay[s], R_OFF);
  }
}


void toggleRingLED(int s) {
    ringState[s] = (ringState[s]==LOW ? HIGH : LOW);
    digitalWrite(pRingLED[s], ringState[s]);

    Serial.write("Switch! ");
    Serial.println(s+1);
}


