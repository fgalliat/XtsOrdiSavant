/**
 * Screen hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 * 
 * TFT_eSPI w/ ILI9486 4inch 480x320 pixels
 */

#include <SPI.h>
#include <TFT_eSPImoa.h>

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

void cls_tft();

void setupScreen() {
  tft.init();
  #if ILI_4INCH
    tft.setRotation(3);
  #else
    tft.setRotation(1);
  #endif
  tft.setTextSize(1);
  tft.setTextColor( TFT_WHITE ); // WHITE - transparent
  cls_tft();
}

void cls_tft() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
}

void _br() {
  // FIXME : cursor
  // FIXME : scroll
  tft.write('\n');
}

void _bcksp() {
  // FIXME : cursor
  tft.write('\b');
}

// forward symbols
void bell();

char lastCh = 0x00;
void write_tft(char ch) {
  if ( ch == '\r' ) {
    _br();
  }
  else if ( ch == '\n' ) {
    if ( lastCh != '\r' ) {
      _br();
    }
  }
  else if ( ch == '\b' ) { _bcksp(); }
  else if ( ch == 27 )   { tft.write('^'); } // FIXME : VT100
  else if ( ch == 7 )    { bell(); }
  else tft.write(ch);
  lastCh = ch;
}

void print_tft(char* str) {
  //tft.print(str);
  int t = strlen(str);
  for(int i=0; i < t; i++) { write_tft(str[i]); }
}

// TODO : refacto in soft_screen.h ?

void cls() {
  cls_tft();
}

void write(char ch) { write_tft(ch); } 
void print(char* str) { print_tft(str); } 

