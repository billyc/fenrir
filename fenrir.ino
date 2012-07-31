#define SWITCHES 6

int pFirstSwitch = 40;
int pFirstRingLED = 41;
int pFirstRelay = 22;

int val;
int val2;

int ringState[SWITCHES];

void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);

  // Loop for each of the six switches:
  for (int i=0; i<SWITCHES; i++) {
    // set pin modes to OUTPUT for all six ring LEDs and Relays
    pinMode(pFirstRingLED + i*2, OUTPUT);
    pinMode(pFirstRelay + i*2, OUTPUT);  

    // ensure everything is off at startup:
    digitalWrite(pFirstRelay + i*2, LOW);
    ringState[i] = LOW;
    digitalWrite(pFirstRingLED + i*2, ringState[i]);
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
  val = digitalRead(pFirstSwitch + s*2);  
  if (val==HIGH) {
    toggleLight(s);
  }
  digitalWrite(pFirstRelay + s*2, val);
}

void toggleLight(int s) {
    ringState[s] = (ringState[s]==LOW ? HIGH : LOW);
    digitalWrite(pFirstRingLED + s*2, ringState[s]);
    Serial.write("Switch!");
    Serial.print(s+1);
}


