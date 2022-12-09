#include "receiver.h"
#include <WiFi101.h>
#include <SPI.h>

// FSM vars
state CURRENT_STATE;
uint32_t savedClock = 0; // last time saved
int serverMessage = NULL; // the message from the server
WiFiClient client; // the wifi client to connect to the server
char buffer[200]; // a buffer to hold incoming messages from the server

void setup() {
  setup_wifi();
  initializeLCD();
  initializeMotor();
}

void loop() {
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis());
}

state updateFsm(state curState, uint32_t mils) {
  state nextState;
  switch (curState) {
    case sRECEIVING:
      // connect to server
      // read from server

      // if failure to connect || bad message
      // else, assign server message

      // server message error
      if (serverMessage == NULL) {
        nextState = sERROR;
      }

      else if (serverMessage == "OFF") {
        savedClock = mils;
        nextState = sOFF;
      } else {
        // message is an integer
        if (isInteger(serverMessage)) {
          // update motor
          // update LCD screen
          savedClock = mils;
          nextState = sDISPLAY_HEARTBEAT;
        } else {
          savedClock = mils;
          nextState = sDISPLAY_MESSAGE;
        }
      }
      break;
    case sOFF:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 500) {
        nextState = sRECEIVING;
      } else {
        nextState = sOFF;
      }
      break;
    case sDISPLAY_HEARTBEAT:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 500) {
        nextState = sRECEIVING;
      } else {
        nextState = sDISPLAY_HEARTBEAT;
      }
      break;
    case sDISPLAY_MESSAGE:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 500) {
        nextState = sRECEIVING;
      } else {
        nextState = sDISPLAY_HEARTBEAT;
      }
      break;
    case sERROR:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 500) {
        nextState = sRECEIVING;
      } else {
        nextState = sERROR;
      }
      break;
  }

  return nextState;
}