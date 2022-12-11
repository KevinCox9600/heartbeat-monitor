#include <LiquidCrystal.h>
#include <Servo.h>               //Servo library

Servo servo_test;        //initialize a servo object for the connected servo 

// LCD SCREEN
// numbers are arduino pins (depends on circuit)
const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/**
 * Initializes the LCD.
 */
void initializeLCD() {
  lcd.begin(16,2);
}

/**
 * Clears the old LCD message and writes a new message and heartbeat.
*/
void writeToLCD(String msg, int hb) {
  #ifndef TESTING
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0, 1);
  if(!(msg == "OFF" || msg == "ERROR")){
    lcd.print(hb);
  }
  #endif
}

// MOTOR

int DIAL_OFF = 0;
int DIAL_DEAD = 5;
int DIAL_PROBABLY_DYING = 23;
int DIAL_PRO_ATHLETE = 42;
int DIAL_NORMAL_START = 60;
int DIAL_NORMAL_END = 105;
int DIAL_NERVOUS_WRECK = 120;
int DIAL_PROBABLY_DYING_AGAIN = 150;
int DIAL_ERROR = 170;


/**
 * Initialized the motor.
*/
void initializeMotor(){
  servo_test.attach(9);
  servo_test.write(0);
  delay(1000);
}

/**
 * Calculate the appropriate dial position based on the heartbeat.
 */
int calculateDialPosition(int hb) {
  if (hb == 404) {
    return DIAL_OFF;
  }

  if (hb == 0) {
    return DIAL_ERROR;
  }

  if (hb > 0 && hb < 30) {
    return DIAL_PROBABLY_DYING;
  }

  if (hb >= 30 && hb < 55) {
    return DIAL_PRO_ATHLETE;
  }

  if (hb >= 55 && hb < 105) {
    return map(hb, 55, 105, DIAL_NORMAL_START, DIAL_NORMAL_END);
  }

  if (hb >= 105 && hb < 150) {
    return DIAL_NERVOUS_WRECK;
  }

  if (hb >= 150) {
    return DIAL_PROBABLY_DYING_AGAIN;
  }
}

/**
 * Update the motor to a specific position.
*/
void updateMotor(int hb) {
  #ifndef TESTING

  int dial_pos = calculateDialPosition(hb);
  servo_test.write(dial_pos); 
  delay(1000);
  #endif
}
