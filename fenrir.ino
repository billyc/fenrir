#define SWITCHES 6
#define POOFERS  4
// Note: Any switches higher than POOFERS are SHIFT keys.


// Our brand of relays are on when LOW, off when HIGH.
#define R_ON 0
#define R_OFF 1

#define MOUTH 22
#define TAIL 26
#define LEFT 24
#define RIGHT 28

// Pin numbers for each switch: x2 because Arduino Mega has pins
// that are two-wide.
int pSwitch[SWITCHES] =  {40,42,44,46,48,50};
int pRingLED[SWITCHES] = {41,43,45,47,49,51};
int pRelay[SWITCHES] =   {22,24,26,28,30,32};

// ringState stores the on/off status of the ring LEDs.
// switchState stores the pressed/unpressed status of the switches.
int ringState[SWITCHES];
int switchState[SWITCHES];

/** Arduino one-time setup call */
void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);

  // Loop for each of the six switches:
  for (int i=0; i<SWITCHES; i++) {
    // setup ring LEDs: output and ready to be pushed
    pinMode(pRingLED[i], OUTPUT);
    ringState[i] = HIGH;
    switchState[i] = LOW;

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
    updateSwitchStatuses();

    // do special stuff if "SHIFT" keys are held
    if (processShiftKeys()==true) {
        delay(100);

    // otherwise, just pass the switch state to the poofers.
    } else {
        for (int i=0; i<POOFERS; i++) {
            digitalWrite(pRelay[i], switchState[i]==HIGH ? R_ON : R_OFF);
        }
    }
}

/** Test each switch twice, to avoid debounce problems */
void updateSwitchStatuses() {

  int val[SWITCHES], val2[SWITCHES];

  // Get first reading for every switch
  for (int s=0; s<SWITCHES; s++) {
    val[s] = digitalRead(pSwitch[s]);
  }

  // A very brief but non-zero delay:
  delay(3);

  // Get second reading for every switch
  for (int s=0; s<SWITCHES; s++) {
    val2[s] = digitalRead(pSwitch[s]);
  }

  // If the readings are the same, switch is "settled". Save it.
  for (int s=0; s<SWITCHES; s++) {
    if (val[s]==val2[s]) {
      switchState[s] = val[s];
    }
  }
}

/** Test the SHIFT keys and do something if they're being pressed.
 *  @return true if any SHIFT is pressed or false if no SHIFT is pressed. */
boolean processShiftKeys() {
    // just return false if no shift key is pressed:
    if (switchState[4]==LOW && switchState[5]==LOW) {
        return false;
    }

    // Both SHIFTS pressed!
    if (switchState[4]==HIGH && switchState[5]==HIGH) {
        bothShiftsPressed();
        return true;
    }

    // Just one:
    if (switchState[4]==HIGH) {
        shift1Pressed();
    } else {
        shift2Pressed();
    }

    return true;
}

void shift1Pressed() {
  Serial.println("shift 1");
  counterClockwise();
}

void shift2Pressed() {
  Serial.println("shift 2");
  clockwise();
}

void bothShiftsPressed() {
  Serial.println("both shifts");

  performMouthBackSides(1);
}

// ############## Fancy Effects ############
//

void clockwise() {
  digitalWrite(MOUTH,R_ON);
  delay(200);
  digitalWrite(MOUTH,R_OFF);

  digitalWrite(RIGHT,R_ON);
  delay(200);
  digitalWrite(RIGHT,R_OFF);

  digitalWrite(TAIL,R_ON);
  delay(200);
  digitalWrite(TAIL,R_OFF);

  digitalWrite(LEFT,R_ON);
  delay(200);
  digitalWrite(LEFT,R_OFF);  
}

void counterClockwise() {
  digitalWrite(MOUTH,R_ON);
  delay(200);
  digitalWrite(MOUTH,R_OFF);

  digitalWrite(LEFT,R_ON);
  delay(200);
  digitalWrite(LEFT,R_OFF);  

  digitalWrite(TAIL,R_ON);
  delay(200);
  digitalWrite(TAIL,R_OFF);

  digitalWrite(RIGHT,R_ON);
  delay(200);
  digitalWrite(RIGHT,R_OFF);
}


void performMouthBackSides(int reps) {
    for (int z=0; z<reps; z++) {
        
        digitalWrite(MOUTH,R_ON);
        delay(250);
        digitalWrite(MOUTH,R_OFF);

        digitalWrite(TAIL,R_ON);
        delay(250);
        digitalWrite(TAIL,R_OFF);

        digitalWrite(LEFT,R_ON);
        digitalWrite(RIGHT,R_ON);
        delay(400);
        blackout();
        delay(500);

    }
}

void blackout() {
    for (int i=0; i<POOFERS; i++) {
        digitalWrite(pRelay[i],R_OFF);
    }
}
