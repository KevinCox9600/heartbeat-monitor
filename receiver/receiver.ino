//#include "test_receiver.h" // uncomment this line to run receiver tests
#include "receiver.h"
#include <WiFi101.h>
#include <SPI.h>

// constants
int SERVER_OFF = 404; // the server message value that indicates system off

// FSM vars
state CURRENT_STATE; // the current state for the FSM
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
  // set up functions for working with wifi, LCD, and motor
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

/**
 * Updates the FSM to its new state.
 * Params:
 *   curState (state) - the current state
 *   mils (uint32_t) - the current millis() value
 * 
*/
state updateFsm(state curState, uint32_t mils) {
  state nextState;
  switch (curState) {
    case sRECEIVING:
      if (mils - savedClock < 100) { // transition 1-1
        nextState = sRECEIVING;
        break;
      }

      // get server message
      serverMessage = get_server_message();

      if (serverMessage == 0) { // transition 1-4
        savedClock = mils;
        nextState = sERROR;
        writeToLCD("ERROR", 0);
        updateMotor(serverMessage);
        break; // break out of case
      }
       
      if (serverMessage == SERVER_OFF) { // transition 1-2
        savedClock = mils;
        nextState = sOFF;

        writeToLCD("OFF", serverMessage);
        updateMotor(serverMessage);
        break;
      }
      // message is a valid heartbeat
      savedClock = mils;
      nextState = sDISPLAY_HEARTBEAT; // transition 1-3

      writeToLCD("Heartbeat:", serverMessage);
      updateMotor(serverMessage);
      break;
    case sOFF:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 2000) { // transition 2-1
        nextState = sRECEIVING;
      } else {
        nextState = sOFF; // transition 2-2
      }
      break;
    case sDISPLAY_HEARTBEAT:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 2000) { // transition 3-1
        nextState = sRECEIVING;
      } else {
        nextState = sDISPLAY_HEARTBEAT; // transition 3-3
      }
      break;
    case sERROR:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 6000) { // transition 4-1
        nextState = sRECEIVING;
      } else {
        nextState = sERROR; // transition 1-4
      }
      break;
  }

  return nextState;
}
