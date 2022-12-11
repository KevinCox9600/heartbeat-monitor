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
  // OFF
  if (hb == 404) {
    return 0;
  }

  // ERROR
  else if (hb == 0) {
    return 170;
  }

  // PROBABLY DYING
  else if (hb > 0 && hb < 30) {
    return 23;
  }

  // PRO ATHLETE
  else if (hb >= 30 && hb < 55) {
    return 42;
  }

  // NORMAL
  else if (hb >= 55 && hb < 105) {
    // return 90;
   return map(hb, 55, 105, 60, 105);
  }

  // NERVOUS WRECK
  else if (hb >= 105 && hb < 150) {
    return 120;
  }

  // PROBABLY DYING AGAIN
  else if (hb >= 150) {
    return 150;
  }

  return 170;
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
