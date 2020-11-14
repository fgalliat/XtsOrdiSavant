/**
 * Xtase - fgalliat @Nov 2020
 * 
 * XtsOrdiSavant
 * 
 * 
 */

#include "hard_globals.h"


void getline_echo(char ch, int index) {
  write(ch);
}




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
    // pollKeyb();
    // // ... < code >
    // int k = readKeyb();
    // if ( k > -1 ) {
    //   if ( k == 27 ) {
    //     tft.print("<Esc>");
    //   } else {
    //     tft.write( (char)k );
    //   }
    // }
    print("Strike any key : ");
    int key = getch();
    write( (char)key );

    beep(440, 200);

    print("login : ");
    char* sharedLine = getline(true, 8);
    print("You typed : "); print(sharedLine); print("\n");

    print("password : ");
    led();
    sharedLine = getline(false, 8);
    led(false);
    print("You typed : "); print(sharedLine); print("\n");



}