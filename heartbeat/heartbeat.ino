#include "buffer.ino"
#include "heartbeat.h"
#include "watchdog.ino"

int inPin = A0;
int threshold = 1000;
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
  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(inPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), togglePower, RISING);

  CURRENT_STATE = sOFF;
}

void loop() {
  updateInputs();
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis(), sensorSignal);
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
