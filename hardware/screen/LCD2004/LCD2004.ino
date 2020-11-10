/**
 * 3.3v MCU -- working w/ arduino IDE
 * 
 * Xtase - fgalliat @Sept2019 / @Nov2020
 * 
 * YenoOrdiSavant Additional LCD 20x4 chars
 * 
 * 
 */

//====================================================================================
//                                    Settings
//====================================================================================

//====================================================================================
//                                    LCD 20x4
//====================================================================================
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
// #include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

void setupLCD() {
  lcd.begin(20,4);

  lcd.setBacklight(LOW);
  lcd.clear();
  // lcd.home ();                   // go home
  lcd.setCursor ( 0, 0 );
}

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  setupLCD(); // a bit long

  // aux screen
  lcd.setCursor(0,0);
  lcd.print("ready");
}

//====================================================================================
//                                    Loop
//====================================================================================

// ===========================================
// ============ Serial Port Routines =========

// my initial code was using 2 UARTs
#define bridge Serial

#define PORT_NONE 0
#define PORT_HARD 1
#define PORT_SOFT 2
char line[20+1];

int serReadLine(int port) {
  memset(line, 0x00, 20+1);
  if ( port == PORT_HARD ) { return Serial.readBytesUntil('\n', line, 20); }
  return bridge.readBytesUntil('\n', line, 20);
}

int serRead(int port) {
  if ( port == PORT_HARD ) { return Serial.read(); }
  return bridge.read();
}

int serWrite(int port, char ch) {
  if ( port == PORT_HARD ) { return Serial.write( (int)ch ); }
  return bridge.write( (int)ch );
}

int serWrite(int port, char* chs) {
  if ( port == PORT_HARD ) { Serial.print( chs ); Serial.write( (uint8_t)0x00 ); }
  bridge.print( chs ); bridge.write( (uint8_t)0x00 );
}

int probePort() {
  if ( Serial.available() > 0 ) { return PORT_HARD; }
//   if ( bridge.available() > 0 ) { return PORT_SOFT; }
  return PORT_NONE;
}

// ===========================================

void loop() {
  int port = probePort();

  if ( port != PORT_NONE ) {
    char chr = serRead(port);

    // == LCD Control ==
    if ( chr == 'C' ) {
      // cls
      lcd.clear(); lcd.home();

      return;
    } else if ( chr == 'P' ) {
      // print
      int len = serReadLine(port);
      lcd.print(line);

      return;
    } else if ( chr == 'c' ) {
      // setCursor(x,y)
      int x = serRead(port); // [0..3]
      int y = serRead(port); // [0..19]
      lcd.setCursor(x,y);

      return;
    }

  }

}

//====================================================================================

