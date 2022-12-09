/**
 * Connects to the server.
 * Returns true for successful connection, false otherwise.
*/
bool connectToServer() {
  return true;
}

/**
 * Reads the current heartbeat value from the server.
 * Returns the value as a string.
*/
String readFromServer() {
  return "100";
}

//////// MAYBE include the above two in a wifi_utils file

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
void writeToLCD(String msg) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg);
}

// MOTOR
void updateMotor(int rotation) {
  return;
}


// OTHER
/**
 * Figures out if integer 
*/
bool isInteger(String msg) {
  return true;
}