//#include "test_receiver.h" // uncomment this line to run receiver tests
#include "receiver.h"
#include <WiFi101.h>
#include <SPI.h>

// constants
int SERVER_OFF = 404;

// FSM vars
state CURRENT_STATE;
uint32_t savedClock = 0; // last time saved
int serverMessage = NULL; // the message from the server

#ifdef TESTING
int serverValue = NULL;
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial);
  #ifdef TESTING
  test_all_tests();
  #else
  setup_wifi();
  initializeLCD();
  initializeMotor();
  #endif
  delay(1000);
}

void loop() {
  #ifndef TESTING
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis());
  #endif
}

state updateFsm(state curState, uint32_t mils) {
  state nextState;
  switch (curState) {
    case sRECEIVING:
      if (mils - savedClock < 100) {
        nextState = sRECEIVING;
        break;
      }

      // get server message
      serverMessage = get_server_message();

      if (serverMessage == 0) {
        savedClock = mils;
        nextState = sERROR;
        writeToLCD("ERROR", 0);
        break; // break out of case
      }
       
      if (serverMessage == SERVER_OFF) {
        savedClock = mils;
        nextState = sOFF;

        writeToLCD("OFF", serverMessage);
        updateMotor(0);
        break;
      }
      // message is a valid heartbeat
      savedClock = mils;
      nextState = sDISPLAY_HEARTBEAT;

      writeToLCD("Heartbeat:", serverMessage);
      updateMotor(serverMessage);
      break;
    case sOFF:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 2000) {
        nextState = sRECEIVING;
      } else {
        nextState = sOFF;
      }
      break;
    case sDISPLAY_HEARTBEAT:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 2000) {
        nextState = sRECEIVING;
      } else {
        nextState = sDISPLAY_HEARTBEAT;
      }
      break;
    case sERROR:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 6000) {
        nextState = sRECEIVING;
      } else {
        nextState = sERROR;
      }
      break;
  }

  return nextState;
}
