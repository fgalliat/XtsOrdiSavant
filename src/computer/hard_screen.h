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

int ttyCursX = 0;
int ttyCursY = 0;
#if ILI_4INCH
 int ttyWidth = 80;
 int ttyHeight = 40;
 int ttyFontWidth = 6;
 int ttyFontHeight = 8;
#else
 int ttyWidth = 53;
 int ttyHeight = 30;
 int ttyFontWidth = 6;
 int ttyFontHeight = 8;
#endif

void _applyTTYcursor() {
  tft.setCursor(ttyCursX*ttyFontWidth, ttyCursY*ttyFontHeight);
}

void gotoXY_tft(int x, int y) {
  ttyCursX = x;
  ttyCursY = y;
  _applyTTYcursor();
}

void home_tft() {
  gotoXY_tft(0,0);
}

void cls_tft() {
  tft.fillScreen(TFT_BLACK);
  home_tft();
}

void _br() {
  // FIXME : scroll
  ttyCursX = 0;
  ttyCursY++;
  _applyTTYcursor();
}

void _bcksp() {
  if ( ttyCursX == 0 && ttyCursY == 0 ) { return; }
  if ( ttyCursX == 0 ) {
    ttyCursY--;
    ttyCursX = ttyWidth-1;
  } else {
    ttyCursX--;
  }
  _applyTTYcursor();
  tft.write(' '); // FIXME : when txtBgTransparent
  _applyTTYcursor();
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
  else if ( ch == 7 )    { bell(); }
  else if ( ch == 27 )   { tft.write('^'); } // FIXME : VT100
  else {
    tft.write(ch);
    ttyCursX++;
    if ( ttyCursX >= ttyWidth ) {
      _br();
    }
  }
  lastCh = ch;
}

void print_tft(char* str) {
  int t = strlen(str);
  for(int i=0; i < t; i++) { write_tft(str[i]); }
}

// TODO : refacto in soft_screen.h ?

void cls() { cls_tft(); }
void gotoXY(int x, int y) { gotoXY_tft(x,y); }
void write(char ch) { write_tft(ch); } 
void print(char* str) { print_tft(str); } 

