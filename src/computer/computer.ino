/**
 * Xtase - fgalliat @Nov 2020
 * 
 * XtsOrdiSavant
 * 
 * 
 */

#include "hard_globals.h"


void getline_echo(char ch, int index) {
  con_putc(ch);
}




void setup() {
  Serial.begin(115200);
  // Serial.begin(9600);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  led();
  setupBuzzer();

  // do IS true !
  #if ILI_4INCH
   Serial.println("4 inch config");
  #endif

  setupScreen();
  tft.fillRect( 50, 50, 380, 220, TFT_CYAN );
  tft.drawRect( 50, 50, 380, 220, TFT_WHITE );
  con_gotoXY(0,0);
  con_puts("Hello World\n");

  setupKeyb();
  led(false);
}

void loop() {
  
    con_puts("Strike any key : ");
    int key = getche();
    bell();

    con_puts("login : ");
    char* sharedLine = getline(true, 8);
    con_puts("You typed : "); con_puts(sharedLine); con_puts("\n");

    con_puts("password : ");
    led();
    sharedLine = getline(false, 8);
    led(false);
    con_puts("You typed : "); con_puts(sharedLine); con_puts("\n");

    con_puts("\n\n\n == EOF ==\n");

}