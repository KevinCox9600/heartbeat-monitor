//#include "buffer.ino"
#include "heartbeat.h"
//#include "watchdog.ino"
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
int buttonPin = 5;

// fsm vars
uint32_t savedClock = 0;
int sensorSignal = 0;
uint32_t mostRecentHeartbeat = 0;
volatile bool off = true;
state CURRENT_STATE;
bool previouslyBelowThreshold = true;

void setup() {
  // put your setup code here, to run once:
  Serial.println("starting");
  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(inPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), togglePower, RISING);

  CURRENT_STATE = sOFF;

  //  setup_wifi();
  setupFlatlineTimer();
  startFlatlineTimer();
  configWatchdog();
}

void loop() {
  updateInputs();
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis(), sensorSignal);
  Serial.println(sensorSignal);
  delay(10);
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
      savedClock = mils;
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
      mostRecentHeartbeat = millis();
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
    Serial.println(mostRecentHeartbeat);
    if (bufferFull()) {
      bufPush(mostRecentHeartbeat);
      nextState = sSENDING_HEARTBEAT;
    } else {
      bufPush(mostRecentHeartbeat);
      nextState = sRECEIVING_HEARTBEAT;
    }
    break;
  case sSENDING_HEARTBEAT:
    if (off) {
      clearBuf();
      nextState = sOFF;
    } else {
      Serial.print("avg: ");
      printBuf();
      Serial.println(bufAvg());
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
