#include "receiver.h"
#include <WiFi101.h>
#include <SPI.h>


// FSM vars
state CURRENT_STATE;
uint32_t savedClock = 0; // last time saved
int serverMessage = NULL; // the message from the server
WiFiClient client;
char buffer[200];


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
  switch (curState) {
    case sRECEIVING:
      // connect to server
      // read from server
      // if failure to connect || bad message
      // else, assign server message
      if (mils - savedClock < 100) {
        nextState = sRECEIVING;
        break;
      }
      
      serverMessage = read_from_get();
      Serial.println(serverMessage);
      if(!client.connected()){
           delay(1000);
           if (connect_to_get() == false) {
            nextState = sERROR;
            savedClock = mils;
            break;
           }
       }
       
      // server message error
      if (serverMessage == 0) {
        savedClock = mils;
        nextState = sERROR;
      }
      else if (serverMessage == 404) {
        savedClock = mils;
        nextState = sOFF;
      } else {
        // message is an integer
          savedClock = mils;
          nextState = sDISPLAY_HEARTBEAT;
      }
      break;
    case sOFF:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 6000) {
        nextState = sRECEIVING;
      } else {
        nextState = sOFF;
        writeToLCD("OFF", serverMessage);
        updateMotor(0);
      }
      break;
    case sDISPLAY_HEARTBEAT:
      // if enough time has passed, try to receive data again
      if (mils - savedClock >= 6000) {
        nextState = sRECEIVING;
      } else {
        writeToLCD("Heartbeat:", serverMessage);
        updateMotor(serverMessage);
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
