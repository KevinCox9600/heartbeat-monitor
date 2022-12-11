
#include "heartbeat.h"

#include <SPI.h>
#include <WiFi101.h>

WiFiClient client;
char buffer[200];

int inPin = A0;
int threshold = 800;
bool high;
int oldTime;
int newTime;
int bufferLen;
float prev[5];
int ind = 0;

// pins
int buttonPin = 6;

// fsm vars
// uint32_t savedClock = 0;
int sensorSignal = 0;
// uint32_t mostRecentHeartbeat = 0;
volatile bool off = true;
bool previouslyBelowThreshold = true;

state CURRENT_STATE;

void setup() {
  // put your setup code here, to run once:
  // Serial.println("starting");
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

void loop() {
#ifndef TESTING
  updateInputs();
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis(), sensorSignal);
  delay(10);
#endif
}

void togglePower() {
  off = !off;
}

state updateFsm(state curState, uint32_t mils, int sensorSignal) {
  //  Serial.println(curState);
  state nextState;
  switch (curState) {
  case sOFF:
    // pet watchdog if off
    petWatchdog();
    Serial.println("off");
    if (off) {
      nextState = sOFF;
    } else {
      // savedClock = mils;
      Serial.println("on");
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sRECEIVING_HEARTBEAT:
    // pet watchdog if there is a sensor signal
    if (sensorSignal >= 5) {
      petWatchdog();
    }

    if (off) {
      nextState = sSENDING_HEARTBEAT;
    } else if (sensorSignal > threshold && previouslyBelowThreshold) {
      // mostRecentHeartbeat = millis();
      previouslyBelowThreshold = false;
      nextState = sSTORING_HEARTBEAT;
    } else if (sensorSignal <= threshold) {
      previouslyBelowThreshold = true;
      nextState = sRECEIVING_HEARTBEAT;
    } else {
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sSTORING_HEARTBEAT:
    // reset the flatline timer
    restartFlatlineTimer();

    Serial.print("most recent heartbeat");
    // Serial.println(mostRecentHeartbeat);
    if (off) {
      nextState = sSENDING_HEARTBEAT;
    } else if (bufferFull()) {
      bufPush(mils);
      nextState = sSENDING_HEARTBEAT;
    } else {
      bufPush(mils);
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sSENDING_HEARTBEAT:
    if (off) {
      clearBuf();
      post_heartrate_to_website(404);
      nextState = sOFF;
    } else {
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

void updateInputs() {
  sensorSignal = analogRead(inPin);
}

// Interrupt Service Routines
/** Clear buffer if no heartbeat for 5 seconds (based on TC implementation). */
void TC3_Handler() {
  TC3->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;
  clearBuf();
  Serial.println("Resetting the buffer");
}
