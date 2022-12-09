#include <LiquidCrystal.h>
#include <Servo.h>               //Servo library
 
Servo servo_test;        //initialize a servo object for the connected servo 

// LCD SCREEN
// numbers are arduino pins (depends on circuit)
const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void initializeLCD() {
  lcd.begin(16,2);
}

/**
 * Clears the old LCD message and writes a new message.
*/
void writeToLCD(String msg, int hb) {
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0, 1);
  if(hb != 404){
    lcd.print(hb);
  }
}

void initializeMotor(){
  servo_test.attach(9);
  servo_test.write(0);
  delay(1000);
}

// MOTOR
void updateMotor(int rotation) {
  servo_test.write(rotation); 
  delay(1000);
}
