/**
 * Xtase - fgalliat @Nov 2020
 * 
 * XtsOrdiSavant
 * 
 * 
 */

#include "hard_globals.h"


void setup() {
  Serial.begin(115200);
  // Serial.begin(9600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  led();
  #if ILI_4INCH
   Serial.println("4 inch config");
  #endif

  setupScreen();
  tft.fillRect( 50, 50, 380, 220, TFT_CYAN );
  tft.drawRect( 50, 50, 380, 220, TFT_WHITE );
  tft.setCursor(0,0);
  tft.setTextColor( TFT_WHITE );
  tft.println("Hello World");

  setupKeyb();
  led(false);
}

void loop() {
    pollKeyb();


    int k = readKeyb();
    if ( k > -1 ) {
      if ( k == 27 ) {
        tft.print("<Esc>");
      } else {
        tft.write( (char)k );
      }
    }
}