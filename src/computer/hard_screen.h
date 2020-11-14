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

void write_tft(char ch) {
  tft.write(ch);
}

void print_tft(char* str) {
  tft.print(str);
}

// TODO : refacto in soft_screen.h ?

void cls() {
  cls_tft();
}

void write(char ch) { write_tft(ch); } 
void print(char* str) { print_tft(str); } 

