/**
 * Xtase - fgalliat @Nov 2020
 * 
 * XtsOrdiSavant
 * 
 * 
 */

#include "hard_screen.h"
// extern auto tft;

void setup() {
  Serial.begin(115200);
  // Serial.begin(9600);

  #if ILI_4INCH
   Serial.println("4 inch config");
  #endif

  tft.init();
  #if ILI_4INCH
  tft.setRotation(3);
  #else
  tft.setRotation(1);
  #endif
  tft.fillScreen(TFT_BLACK);
 
  tft.fillRect( 50, 50, 380, 220, TFT_BLUE );
  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor( TFT_WHITE );
  tft.println("Hello World");


}

void loop() {
    Serial.println("Hello");
    delay(500);
}