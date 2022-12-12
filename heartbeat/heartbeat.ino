#include "heartbeat.h"
// #include "test_sender.h"

#include <SPI.h>
#include <WiFi101.h>

WiFiClient client;
char buffer[200]; // wifi buffer

int threshold = 800;

// pins
int inPin = A0;
int buttonPin = 6;

// fsm vars
int sensorSignal = 0;
volatile bool off = true;
bool previouslyBelowThreshold = true;

state CURRENT_STATE;

/**
 * Setup code
 */
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
#ifndef TESTING
  pinMode(inPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), togglePower, FALLING);

  setup_wifi();
  setupFlatlineTimer();
  startFlatlineTimer();
  configWatchdog();
#else
  test_all_tests();
#endif

  CURRENT_STATE = sOFF;
}

/**
 * Loop code
 */
void loop() {
#ifndef TESTING
  updateInputs();
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis(), sensorSignal);
  delay(10);
#endif
}

/** Toggles the button. */
void togglePower() {
  off = !off;
}

/**
 * Update the FSM and undergo state transitions if applicable.
 * Params:
 *    curState (state) - the current state of the FSM
 *    mils (uint32_t) - the current time in milliseconds
 *    sensorSignal (int) - the analog signal from the heartbeat monitor, from 0 to 1024
 */
state updateFsm(state curState, uint32_t mils, int sensorSignal) {
  state nextState;
  switch (curState) {
  case sOFF:
    // pet watchdog if off
    petWatchdog();
    Serial.println("off");
    if (off) { // transition 1-1
      nextState = sOFF; 
    } else { // transition 1-2
      Serial.println("on");
      nextState = sRECEIVING_HEARTBEAT; 
    }
    break;
  case sRECEIVING_HEARTBEAT:
    // pet watchdog if there is a sensor signal
    if (sensorSignal >= 5) {
      petWatchdog();
    }

    if (off) { // transition 2-1
      nextState = sSENDING_HEARTBEAT;
    } else if (sensorSignal > threshold && previouslyBelowThreshold) { // transition 2-3
      previouslyBelowThreshold = false;
      nextState = sSTORING_HEARTBEAT;
    } else if (sensorSignal <= threshold) { // transition 2-2a
      previouslyBelowThreshold = true;
      nextState = sRECEIVING_HEARTBEAT;
    } else { // transition 2-2b
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sSTORING_HEARTBEAT:
    // reset the flatline timer every time a heartbeat is stored
    restartFlatlineTimer();

    Serial.print("most recent heartbeat");
    Serial.println(mils);
    if (off) { // transition 3-1
      nextState = sSENDING_HEARTBEAT;
    } else if (bufferFull()) { // transition 3-4
      bufPush(mils);
      nextState = sSENDING_HEARTBEAT;
    } else { // transition 3-2
      bufPush(mils);
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sSENDING_HEARTBEAT:
    if (off) { // transition 4-1
      clearBuf();
      post_heartrate_to_website(404);
      nextState = sOFF;
    } else { // transition 4-2
      float avg = bufAvg();
      if (avg > 0 && avg < 190) {
        post_heartrate_to_website((int)avg);
        Serial.print("avg: ");
        printBuf();
        Serial.println(avg);
      }
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  }

  return nextState;
}

/** Update the sensor signal based on the sensor reading. */
void updateInputs() {
  sensorSignal = analogRead(inPin);
}

// Interrupt Service Routine
/** Clear buffer if no heartbeat for 5 seconds (based on TC implementation). */
void TC3_Handler() {
  TC3->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;
  clearBuf();
  Serial.println("Resetting the buffer");
}
