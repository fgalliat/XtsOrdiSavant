/**
 * ESP32 code for MCP23017 GPIO 
 * 
 * Xtase - fgalliat @Sept2019 / @Nov2020
 * 
 * YenoOrdiSavant 8x8 keyboard
 * 
 */

//====================================================================================
//                                    Settings
//====================================================================================
#define LED 27

//====================================================================================
//                                    Keyboard
//====================================================================================
#include "Keypad_MC17.h"
#include <Keypad.h>        // from Arduino's libs
#include <Wire.h>          // from Arduino's libs

#define KEYB_I2CADDR 0x20

const byte K_ROWS = 8; // eight rows
const byte K_COLS = 8; // eight columns

#define ESCP 0x1B
#define SHFT 0xFF
#define CTRL 0xFE
#define BREK 0x03


//define the cymbols on the buttons of the keypads
char hexaKeys[K_ROWS][K_COLS] = {
  // 0    1    2    3    4    5    6    7  
  { '#',BREK, '2', '4', '6', '8', '0',0x00 }, // 0 -- LOCK/BREAK -> Ctrl-C
  {ESCP, '1', '3', '5', '7', '9', '^',0x00 }, // 1 -- 0x1B is Esc 
  {0x00, '~', 'z', 'r', 'y', 'i', 'p',0x00 }, // 2
  {0x00, 'q', 's', 'f', 'h', 'k', 'm', '>' }, // 3
  {0x00, ' ', 'c', 'b', ',', ':',SHFT, '$' }, // 4 -- 0xFF is Shift
  {0x00,CTRL, 'x', 'v', 'n', ';', '=','\b' }, // 5 -- 0xFE is Ctrl (ERASE/AC)
  {0x00, 'w', 'd', 'g', 'j', 'l', '!','\n' }, // 6
  {0x00, 'a', 'e', 't', 'u', 'o', '^', '<' }, // 7
};

char hexaShiftedKeys[K_ROWS][K_COLS] = {
  // 0    1    2    3    4    5    6    7  
  { '#',BREK, 'e','\'', '-', '_', '0',0x00 }, // 0
  {ESCP, '(', '"', ')', 'e', 'c', '^',0x00 }, // 1
  {0x00, '~', 'Z', 'R', 'Y', 'I', 'P',0x00 }, // 2
  {0x00, 'Q', 'S', 'F', 'H', 'K', 'M', '>' }, // 3
  {0x00, ' ', 'C', 'B', '?', '/',SHFT, '$' }, // 4
  {0x00,CTRL, 'X', 'V', 'N', '.', '+','\b' }, // 5
  {0x00, 'W', 'D', 'G', 'J', 'L', 'u','\n' }, // 6
  {0x00, 'A', 'E', 'T', 'U', 'O', '^', '<' }, // 7
};

byte rowPins[K_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[K_COLS] = {15, 14, 13, 12, 11, 10, 9, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad( makeKeymap(hexaKeys), rowPins, colPins, K_ROWS, K_COLS, KEYB_I2CADDR ); 

void setupKeyb() {
    customKeypad.begin();
}

//====================================================================================
//                                    I2C scan
//====================================================================================
void scanI2C()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknow error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}

//====================================================================================
//                                    Setup
//====================================================================================
void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  digitalWrite(LED, HIGH);
  setupKeyb();
  Serial.println("Keyboard test !");
  digitalWrite(LED, LOW);

}

//====================================================================================
//                                    Loop
//====================================================================================

int loopCpt = -1;

// ===========================================

char* msg;

boolean shiftKey = false;
boolean ctrlKey = false;

long t0,t1,tMin=1000,tMax=0, duration;

void loop()
{
//{ scanI2C(); return; }


  if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
  } else {
    loopCpt++;
  }

  digitalWrite(LED, LOW);

// have BETTER RESULT than scanning only one key @ a time
t0 = millis();
// consumes between 0 & 57msec to scan keys (on an ESP32)
bool keyActivity = customKeypad.getKeys();
t1 = millis();
duration = t1 - t0;

if ( duration > tMax ) {
  tMax = duration;
} else if (duration < tMin) {
  tMin = duration;
}

// if ( loopCpt % 100 == 0 ) {
//   Serial.printf("min=%d max=%d \r\n", tMin, tMax);
// }

if ( keyActivity )
    {
        digitalWrite(LED, HIGH);

        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( customKeypad.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (customKeypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:

                      if ( customKeypad.key[i].kchar == SHFT ) {
                        shiftKey = true;
                      } else if ( customKeypad.key[i].kchar == CTRL ) {
                        ctrlKey = true;
                      }
                       else if ( customKeypad.key[i].kchar == ESCP ) {
                        Serial.println( "Esc" );
                      } else {
                        char ch = customKeypad.key[i].kchar;
                        if ( shiftKey ) {
                          ch = makeKeymap( hexaShiftedKeys )[ customKeypad.key[i].kcode ];
                        }
                        Serial.print(ch);
                      }
                    break;
                    case RELEASED:
                      if ( customKeypad.key[i].kchar == SHFT ) {
                        shiftKey = false;
                      } else if ( customKeypad.key[i].kchar == CTRL ) {
                        ctrlKey = false;
                      }
                    break;
                    case HOLD:
                      // if ( customKeypad.key[i].kchar == SHFT ) {
                      //   Serial.print( "(*)Shift " );
                      // }
                      // else if ( customKeypad.key[i].kchar == CTRL ) {
                      //   Serial.print( "(*)Ctrl " );
                      // }
                    break;
                    case IDLE:
                    break;
                }
            }
        }
    }

}

//====================================================================================

