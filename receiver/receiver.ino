#include "receiver.h"
#include <WiFi101.h>
#include <SPI.h>

// constants
int SERVER_OFF = 404;

// FSM vars
state CURRENT_STATE;
uint32_t savedClock = 0; // last time saved
int serverMessage = NULL; // the message from the server


void setup() {
  Serial.begin(9600);
  setup_wifi();
  initializeLCD();
  initializeMotor();
  delay(1000);
}

void loop() {
  CURRENT_STATE = updateFsm(CURRENT_STATE, millis());
}

state updateFsm(state curState, uint32_t mils) {
  state nextState;
  int count = 0;
  switch (curState) {
    case sRECEIVING:
      if (mils - savedClock < 100) {
        nextState = sRECEIVING;
        break;
      }

      serverMessage = read_from_get();

      // try 20 times to read the server message
      while (serverMessage == 0 && count < 100) {
        if (!client_connected()) {
          delay(1000);
          if (!connect_to_get()) {
            delay(1000);
          }
        }
        
        // read the server message
        serverMessage = read_from_get();
        // increment the count
        count = count + 1;
      }

      if (count >= 100) {
        nextState = sERROR;
        Serial.println("error: couldn't get value in 100+ tries");
        writeToLCD("ERROR", 0);
        break; // break out of case
      }
       
       
      // server message error
      if (serverMessage == 0) {
        savedClock = mils;
        nextState = sERROR;
      }
      else if (serverMessage == SERVER_OFF) {
        savedClock = mils;
        nextState = sOFF;

        writeToLCD("OFF", serverMessage);
        updateMotor(0);

      } else {
        // message is an integer
          savedClock = mils;
          nextState = sDISPLAY_HEARTBEAT;

          writeToLCD("Heartbeat:", serverMessage);
          updateMotor(serverMessage);
      }
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
      if (mils - savedClock >= 2000) {
        nextState = sRECEIVING;
      } else {
        nextState = sERROR;
      }
      break;
  }

  return nextState;
}
