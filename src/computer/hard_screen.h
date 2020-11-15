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

int ttyCursX = 0;
int ttyCursY = 0;
#if ILI_4INCH
 const int ttyWidth = 80;
 const int ttyHeight = 40;
 const int ttyFontWidth = 6;
 const int ttyFontHeight = 8;
#else
 const int ttyWidth = 53;
 const int ttyHeight = 30;
 const int ttyFontWidth = 6;
 const int ttyFontHeight = 8;
#endif

uint16_t bgColor = TFT_BLACK;
uint16_t fgColor = TFT_WHITE;
bool transparentText = true;


void cls_tft();
void initTtyMem();

void setupScreen() {
  tft.init();
  #if ILI_4INCH
    tft.setRotation(3);
  #else
    tft.setRotation(1);
  #endif
  tft.setTextSize(1);
  tft.setTextColor( fgColor ); // WHITE - transparent
  cls_tft();
  initTtyMem();
}

void _applyTTYcursor() {
  tft.setCursor(ttyCursX*ttyFontWidth, ttyCursY*ttyFontHeight);
}

void drawBlankChar() {
  if ( !transparentText ) { 
    _applyTTYcursor();
    tft.write(' ');
    _applyTTYcursor();
  } else {
    tft.fillRect(ttyCursX*ttyFontWidth, ttyCursY*ttyFontHeight, ttyFontWidth, ttyFontHeight, bgColor);
  }
}

// =======================================
// || Scrolling Section                 ||
// =======================================
const int ttyMemSize = ttyWidth*ttyHeight;
uint8_t ttyMem[ttyWidth*ttyHeight];
uint8_t ttyMemLines[ttyHeight];
bool ttyMemInited = false;

void initTtyMem() {
  ttyMemInited = false;
  memset(ttyMem, 0x00, ttyMemSize);
  memset(ttyMemLines, 0x00, ttyHeight);
  ttyMemInited = true;
}

void setTtyMemChar(int x, int y, char ch) {
  ttyMem[ (ttyWidth*y)+x ] = ch;
  ttyMemLines[y] = 1; // @least : lines is 'dirty'
}

void renderTtyMemLine(int y) {
  if ( ttyMemLines[y] == 0 ) {
    // FIXME : coords
    tft.fillRect(0, y*ttyFontHeight, 480, ttyFontHeight, bgColor);
    return;
  }
  if ( transparentText ) {
    // FIXME : coords - do eraseUntilEOL()
    tft.fillRect(0, y*ttyFontHeight, 480, ttyFontHeight, bgColor);
  }
  uint8_t* line = &ttyMem[ (ttyWidth*y) ];
  tft.setCursor(0, ttyCursY*ttyFontHeight);
  for(int x=0; x < ttyWidth; x++) {
    uint8_t ch = line[x];
    if ( ch == 0x00 ) { tft.write(' '); } // FIXME : do better but inc tft cursor
    else {
      tft.write(ch);
    } 
  }
}

void renderTtyMem() {
  for(int y=0; y < ttyHeight; y++) {
    renderTtyMemLine(y);
  }
  _applyTTYcursor();
}

void scrollUp() {
  memmove( &ttyMem[0], &ttyMem[ttyWidth], (ttyWidth * (ttyHeight-1)) );
  memset( &ttyMem[ ttyWidth * (ttyHeight-1)], 0x00, ttyWidth );

  memmove( &ttyMemLines[0], &ttyMemLines[1], (ttyHeight-1) );
  ttyMemLines[(ttyHeight-1)] = 0; // beware w/ that -> may need to set dirty ?

  renderTtyMem();
}


// =======================================
// || Console Section                   ||
// =======================================

void gotoXY_tft(int x, int y) {
  ttyCursX = x;
  ttyCursY = y;
  _applyTTYcursor();
}

void home_tft() {
  gotoXY_tft(0,0);
}

void cls_tft() {
  tft.fillScreen(bgColor);
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
  drawBlankChar();
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
    setTtyMemChar(ttyCursX, ttyCursY, ch);
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


