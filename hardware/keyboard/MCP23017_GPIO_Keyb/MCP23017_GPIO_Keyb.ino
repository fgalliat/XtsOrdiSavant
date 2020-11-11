/**
 * 3.3v MCU -- working w/ arduino IDE
 * 
 * Xtase - fgalliat @Sept2019 / @Nov2020
 * 
 * YenoOrdiSavant 8x8 keyboard
 * 
 * TODO : 
 *  - see $/libraries/Keypad/examples/MultiKey/MultiKey.ino
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
//define the cymbols on the buttons of the keypads
char hexaKeys[K_ROWS][K_COLS] = {
  // 0    1    2    3    4    5    6    7  
  { '#',0x03, '2', '4', '6', '8', '0',0x00 }, // 0
  {0x1B, '1', '3', '5', '7', '9', '^',0x00 }, // 1 -- 0x1B is Esc 
  {0x00, '~', 'z', 'r', 'y', 'i', 'p',0x00 }, // 2
  {0x00, 'q', 's', 'f', 'h', 'k', 'm', '>' }, // 3
  {0x00, ' ', 'c', 'b', ',', ':',0xFF, '$' }, // 4 -- 0xFF is Shift
  {0x00,0xFE, 'x', 'v', 'n', ',', '=','\b' }, // 5 -- 0xFE is Ctrl
  {0x00, 'w', 'd', 'g', 'j', 'l', '!','\n' }, // 6
  {0x00, 'a', 'e', 't', 'u', 'o', '^', '<' }, // 7
};
byte rowPins[K_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[K_COLS] = {15, 14, 13, 12, 11, 10, 9, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad( makeKeymap(hexaKeys), rowPins, colPins, K_ROWS, K_COLS, KEYB_I2CADDR ); 

void setupKeyb() {
    customKeypad.begin();
}

int pollKeyb() {
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY){
    return customKey;
  }

  return -1;
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

  while( Serial.available() == 0 ) { delay(100); }

  setupKeyb();
  //Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22
  Serial.println("Go !");
  digitalWrite(LED, LOW);

}

//====================================================================================
//                                    Loop
//====================================================================================

int loopCpt = -1;

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
  // if ( port == PORT_HARD ) { Serial.print( chs ); Serial.write( (uint8_t)0x00 ); }
  // bridge.print( chs ); bridge.write( (uint8_t)0x00 );
  Serial.print( chs ); Serial.write( (uint8_t)0x00 );
}

int probePort() {
  if ( Serial.available() > 0 ) { return PORT_HARD; }
  // if ( bridge.available() > 0 ) { return PORT_SOFT; }
  return PORT_NONE;
}

// ===========================================
// ==== Keyboard buffer routines =============
#define KEYB_BUFF_LEN 16

char kBuff[KEYB_BUFF_LEN+1] = {
  // 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16/0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ===========================================

void loop()
{
//Serial.println("coucou");
//{ scanI2C(); return; }


  if ( loopCpt == -1 ) {
    // first time
    loopCpt = 0;
  }


  int port = probePort();


  if ( port != PORT_NONE ) {
    char chr = serRead(port);

    // == KEYB Control ==
    if ( chr == 'K' ) {
      // clear Key buffer
      memset( kBuff, 0x00, KEYB_BUFF_LEN+1 );

      return; // skip keyboard reading
    } else if ( chr == 'k' ) {
      // return kBuff content
      serWrite( port, kBuff );
      memset( kBuff, 0x00, KEYB_BUFF_LEN+1 );

      return; // ...
    }
    

  }

  int currentBufferLen = strlen( kBuff );
  if ( currentBufferLen >= KEYB_BUFF_LEN ) {
    // Keyboard buffer overflow
    // stop here ...
    return;
  }

  digitalWrite(LED, LOW);
  // ~56msec w/ Arduino ProMini 3.3 8MHz
  int k = pollKeyb();
  if ( k == -1 ) { return; }

  digitalWrite(LED, HIGH);
  kBuff[ currentBufferLen ] = (char)k;
  Serial.write( (char)k );

}

//====================================================================================

