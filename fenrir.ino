// FENRiR -- the giant flaming wolf.
// Contact: http://sheetmetalalchemist.com
// --------------------------------------------------------------------

// Note: Any switches higher than POOFERS are SHIFT keys.
#define SWITCHES 6
#define POOFERS  4

// Which relay corresponds to which poofer position:
#define MOUTH 0
#define TAIL  2
#define LEFT  1
#define RIGHT 3

// Joystick directions
#define J_FIRE 0
#define J_UP 1
#define J_DOWN 2
#define J_LEFT 3
#define J_RIGHT 4

// Pin numbers for each switch: count by two because Arduino Mega
// has headers that are two-wide.
int pSwitch[SWITCHES] =  {40,42,44,46,48,50};
int pRingLED[SWITCHES] = {41,43,45,47,49,51};
int pRelay[SWITCHES] =   {22,24,26,28,30,32};
int pJoystick[5] =       {8,9,10,11,12};

// ---- code starts here ----------------------------------------------

// Our brand of relays are on when LOW, off when HIGH.
#define R_ON 0
#define R_OFF 1

// ringState stores the on/off status of the ring LEDs.
// switchState stores the pressed/unpressed status of the switches.
int ringState[SWITCHES];
int switchState[SWITCHES];

int poofSpinner = 0;

// timed effects blackout the LED rings; this variable is used to signal that
// they need to be re-lit after the effect.
boolean ringsNeedResetting = true;

/** Arduino one-time setup call */
void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);

  // Setup each switch and LED:
  for (int i=0; i<SWITCHES; i++) {
    // setup variables for each switch & LED
    pinMode(pRingLED[i], OUTPUT);
    ringState[i] = HIGH;
    switchState[i] = LOW;

    // ensure relays are off at startup: (IMPORTANT! DO NOT CHANGE!)
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

  for (int j=0; j<2; j++) {
    for (int i=SWITCHES-1; i>=0; i--) {
      digitalWrite(pRingLED[i], HIGH);
      delay(25); // - j*5);
      digitalWrite(pRingLED[i], LOW);
      delay(75); // 150 - j*50);
    }
  }
  delay(1000);

  resetRings(HIGH);
}


// Main Arduino Event Loop
void loop() {
    updateSwitchStatuses();

    // do special stuff if "SHIFT" keys are held
    if (processShiftKeys()==true) {
        delay(100);

    } else if (handleJoystick()==true) {
      // don't do anything -- especially don't reset rings!
    } else {

        // otherwise, just pass the switch state to the poofers.
        resetRings(HIGH);
        for (int i=0; i<POOFERS; i++) {
            digitalWrite(pRelay[i], switchState[i]==HIGH ? R_ON : R_OFF);
            digitalWrite(pRingLED[i], 1-switchState[i]);
        }
    }
}

/** Atari Joysticks require pull-up HIGH value, and are LOW when activated. */
void setupJoystick() {
    for (int i=0; i<5; i++) {
        pinMode(pJoystick[i],INPUT);
        digitalWrite(pJoystick[i],HIGH);
    }
}

boolean handleJoystick() {
    for (int j=0;j<5;j++) {
       digitalWrite(pJoystick[j],HIGH);
    }

    // only do stuff if fire button is held down
    if (digitalRead(pJoystick[J_FIRE])==HIGH) {
       resetRings(HIGH);
       return false;
    }

    if (digitalRead(pJoystick[J_LEFT])==LOW) {

        ringsNeedResetting = true;
        blackout();

        poofSpinner++;
        poofSpinner %= 4;

        setPoofer(poofSpinner,R_ON);
        delay(100);
        setPoofer(poofSpinner,R_OFF);
        delay(50);

        return true;

    } else if (digitalRead(pJoystick[J_RIGHT])==LOW) {

        ringsNeedResetting = true;
        blackout();

        poofSpinner--;
        if (poofSpinner<0) poofSpinner += POOFERS;

        setPoofer(poofSpinner,R_ON);
        delay(100);
        setPoofer(poofSpinner,R_OFF);
        delay(50);
        return true;

    } else if (digitalRead(pJoystick[J_UP])==LOW) {

        ringsNeedResetting = true;
        blackout();

        setPoofer(MOUTH,R_ON);
        delay(100);
        setPoofer(MOUTH,R_OFF);
        delay(50);
        return true;

    } else if (digitalRead(pJoystick[J_DOWN])==LOW) {

        ringsNeedResetting = true;
        blackout();

        setPoofer(TAIL,R_OFF);
        setPoofer(LEFT,R_ON);
        setPoofer(RIGHT,R_ON);
        delay(100);
        setPoofer(TAIL,R_ON);
        setPoofer(LEFT,R_OFF);
        setPoofer(RIGHT,R_OFF);
        delay(100);
        setPoofer(TAIL,R_OFF);
        return true;
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

/** Set poofer *and* ring LED to specified state */
void setPoofer(int which, int state) {
    digitalWrite(pRelay[which], state);
    digitalWrite(pRingLED[which], state==R_ON ? HIGH : LOW);
}

/** Dim or light up all the rings, if necessary */
void resetRings(int state) {
  if (state==LOW) {
    for (int i=0; i<SWITCHES; i++) {
      digitalWrite(pRingLED[i], LOW);
    }
    ringsNeedResetting = true;

  } else if (ringsNeedResetting) {
    delay(400);
    for (int i=0; i<SWITCHES; i++) {
      digitalWrite(pRingLED[i], HIGH);
      delay(80);
    }
    ringsNeedResetting = false;
  }
}

// ############## Fancy Effects ############

void clockwise() {
  resetRings(LOW);

  setPoofer(MOUTH,R_ON);
  delay(200);
  setPoofer(MOUTH,R_OFF);

  setPoofer(RIGHT,R_ON);
  delay(200);
  setPoofer(RIGHT,R_OFF);

  setPoofer(TAIL,R_ON);
  delay(200);
  setPoofer(TAIL,R_OFF);

  setPoofer(LEFT,R_ON);
  delay(200);
  setPoofer(LEFT,R_OFF);
}

void counterClockwise() {
  resetRings(LOW);

  setPoofer(MOUTH,R_ON);
  delay(200);
  setPoofer(MOUTH,R_OFF);

  setPoofer(LEFT,R_ON);
  delay(200);
  setPoofer(LEFT,R_OFF);

  setPoofer(TAIL,R_ON);
  delay(200);
  setPoofer(TAIL,R_OFF);

  setPoofer(RIGHT,R_ON);
  delay(200);
  setPoofer(RIGHT,R_OFF);
}


void performMouthBackSides(int reps) {
    resetRings(LOW);
    for (int z=0; z<reps; z++) {

        setPoofer(MOUTH,R_ON);
        delay(250);
        setPoofer(MOUTH,R_OFF);

        setPoofer(TAIL,R_ON);
        delay(250);
        setPoofer(TAIL,R_OFF);

        setPoofer(LEFT,R_ON);
        setPoofer(RIGHT,R_ON);
        delay(400);
        setPoofer(LEFT,R_OFF);
        setPoofer(RIGHT,R_OFF);

        delay(500);
    }
}

void blackout() {
    for (int i=0; i<POOFERS; i++) {
        setPoofer(i, R_OFF);
    }
}

