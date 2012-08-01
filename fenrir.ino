#define SWITCHES 6

// Our brand of relays are on when LOW, off when HIGH.
#define R_ON 0
#define R_OFF 1

// Pin numbers for switch 1; subsequent switches MUST be sequential!
int pSwitch = 40;
int pRingLED = 41;
int pRelay = 22;

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
    pinMode(pRingLED + i*2, OUTPUT);
    ringState[i] = HIGH;

    // ensure relays are off at startup:
    digitalWrite(pRelay + i*2, R_OFF);
    pinMode(pRelay + i*2, OUTPUT);
  }

  // do a fancy ringlight dance, to welcome the user
  for (int j=0; j<4;j++) {
    for (int i=SWITCHES-1; i>=0; i--) {
      digitalWrite(pRingLED + i*2, HIGH);
      delay(30 - j*5);
      digitalWrite(pRingLED + i*2, LOW);
      delay(80 - j*20);
    }
  }
  // give user time to verify all is well after setup
  delay(500);

  // light 'em up!
  for (int i=0; i<SWITCHES; i++) {
    digitalWrite(pRingLED + i*2, HIGH);
    delay(100);
  }
}

// Infinite Arduino event loop!
void loop() {
  for (int s=0; s<SWITCHES; s++) {
    checkSwitch(s);
  }
  delay(250);
}

void checkSwitch(int s) {
  val = digitalRead(pSwitch + s*2);

  if (val==HIGH) {
    toggleRingLED(s);
    digitalWrite(pRelay + s*2, R_ON);
  } else {
    digitalWrite(pRelay + s*2, R_OFF);
  }
}

void toggleRingLED(int s) {
    ringState[s] = (ringState[s]==LOW ? HIGH : LOW);
    digitalWrite(pRingLED + s*2, ringState[s]);

    Serial.write("Switch! ");
    Serial.println(s+1);
}

