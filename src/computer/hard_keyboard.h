/**
 * Keyboard hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 * 
 * MCP 23017 GPIO/I2C & 8x8 OrdiSavant builtin keyboard (AZERTY)
 */


//====================================================================================
//                                    Keyboard
//====================================================================================
#include "hard_Keypad_MC17.h"
#include <Keypad.h>        // from Arduino's libs
#include <Wire.h>          // from Arduino's libs

#define KEYB_I2CADDR 0x20

const byte K_ROWS = 8; // eight rows
const byte K_COLS = 8; // eight columns

#define ESCP 0x1B
#define SHFT 0xFF
#define CTRL 0xFE
#define BREK 0x03
#define BKSP '\b'
#define RETN '\r'


//define the cymbols on the buttons of the keypads
char hexaKeys[K_ROWS][K_COLS] = {
  // 0    1    2    3    4    5    6    7  
  { '#',BREK, '2', '4', '6', '8', '0',0x00 }, // 0 -- LOCK/BREAK -> Ctrl-C
  {ESCP, '1', '3', '5', '7', '9', '^',0x00 }, // 1 -- 0x1B is Esc 
  {0x00, '~', 'z', 'r', 'y', 'i', 'p',0x00 }, // 2
  {0x00, 'q', 's', 'f', 'h', 'k', 'm', '>' }, // 3
  {0x00, ' ', 'c', 'b', ',', ':',SHFT, '$' }, // 4 -- 0xFF is Shift
  {0x00,CTRL, 'x', 'v', 'n', ';', '=','\b' }, // 5 -- 0xFE is Ctrl (ERASE/AC)
  {0x00, 'w', 'd', 'g', 'j', 'l', '!',RETN }, // 6
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
  {0x00, 'W', 'D', 'G', 'J', 'L', 'u',RETN }, // 6
  {0x00, 'A', 'E', 'T', 'U', 'O', '^', '<' }, // 7
};

byte rowPins[K_ROWS] = {7, 6, 5, 4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[K_COLS] = {15, 14, 13, 12, 11, 10, 9, 8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_MC17 customKeypad( makeKeymap(hexaKeys), rowPins, colPins, K_ROWS, K_COLS, KEYB_I2CADDR ); 

void setupKeyb() {
    customKeypad.begin();
}

boolean shiftKey = false;
boolean ctrlKey = false;

// long t0,t1,tMin=1000,tMax=0, duration;

#define KEYB_BUFF_LEN 15
char _keyBuffer[KEYB_BUFF_LEN+1];
int _keyBufferCursor = -1; // not initialized

void _appendToKBBuffer(char ch) {
    if ( _keyBufferCursor < 0 ) { memset(_keyBuffer, 0x00, KEYB_BUFF_LEN+1); }
    if ( _keyBufferCursor >= KEYB_BUFF_LEN ) {
        // overflow ...
        return;
    }
    _keyBuffer[ _keyBufferCursor++ ] = ch;
}

int _readKBBuffer() {
    if ( _keyBufferCursor <= 0 ) { return -1; }
    return _keyBuffer[ --_keyBufferCursor ];
}

void _flushKBBuffer() {
  if ( _keyBufferCursor <= 0 ) return;
  memset(_keyBuffer, 0x00, _keyBufferCursor);
}


void pollKeyb() {
    // have BETTER RESULT than scanning only one key @ a time
    // consumes between 0 & 57msec to scan keys (on an ESP32)
    bool keyActivity = customKeypad.getKeys();

    if ( keyActivity ) {

        for (int i=0; i<LIST_MAX; i++) {  // Scan the whole key list.
            if ( customKeypad.key[i].stateChanged ) {   // Only find keys that have changed state.
                switch (customKeypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                      if ( customKeypad.key[i].kchar == SHFT ) {
                        shiftKey = true;
                      } else if ( customKeypad.key[i].kchar == CTRL ) {
                        ctrlKey = true;
                      }
                       else if ( customKeypad.key[i].kchar == ESCP ) {
                        Serial.println( "Esc" );
                        _appendToKBBuffer( 27 );
                      } else {
                        char ch = customKeypad.key[i].kchar;
                        if ( shiftKey ) {
                          ch = makeKeymap( hexaShiftedKeys )[ customKeypad.key[i].kcode ];
                        }
                        Serial.print(ch);
                        _appendToKBBuffer( ch );
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
                        // manage key repeat ?
                    break;
                    case IDLE:
                        // manage key repeat ?
                    break;
                }
            }
        }
    }
}

int readKeyb(bool poll=false) {
    if ( poll ) { pollKeyb(); }
    return _readKBBuffer();
}

int getch_kb() {
  int ch = 0;
  while( ch <= 0 ) {
    pollKeyb();
    ch = _readKBBuffer();
  } 
  _flushKBBuffer(); // even if typed 2 keys, keep only 1 char
  return ch;
}

// beware of Esc, BackSpace (& cursor / Vs index > 0), CR Vs LF
void getline_echo(char ch, int index);

#define MAX_KB_LINE_LEN 255
char _kbLine[MAX_KB_LINE_LEN+1];
int _kbLineCursor = 0;

char* getline_kb(bool echo=true, int maxLen=MAX_KB_LINE_LEN) {
  memset(_kbLine, 0x00, MAX_KB_LINE_LEN+1);
  _kbLineCursor = 0;
  int ch = 0;
  while( true ) {
    pollKeyb();
    ch = _readKBBuffer();
    if ( ch <= 0 ) { continue; }
    
    if ( ch == BREK ) { 
      if ( echo ) { getline_echo('^', _kbLineCursor); getline_echo('C', _kbLineCursor+1); }
      _flushKBBuffer(); 
      return NULL; 
    }
    else if ( ch == BKSP ) {
      if ( _kbLineCursor > 0 ) {
        if (echo) getline_echo('\b', _kbLineCursor);
        _kbLine[ --_kbLineCursor ] = 0x00;
      }
    }
    else if ( ch == RETN ) {
      if (echo) getline_echo('\r', _kbLineCursor);
      break;
    }
    else {
      if ( _kbLineCursor < maxLen-1 ) {
        if (echo) getline_echo(ch, _kbLineCursor);
        _kbLine[ _kbLineCursor++ ] = ch;
      } // else : other spe keys must be allowed
    }
  }
  _flushKBBuffer();
  return _kbLine;
}

bool available_kb() {
  return _keyBufferCursor > 0;
}

// TODO : refacto in soft_keyboard.h ?

// TODO : may duplicate these functions for Serial, WiFi, ...
int getch() {
  return getch_kb();
}
/** returns NULL if Ctrl-C */
char* getline(bool echo=true, int maxLen=MAX_KB_LINE_LEN) {
  return getline_kb(echo, maxLen);
}

bool kbHit() {
  return available_kb();
}