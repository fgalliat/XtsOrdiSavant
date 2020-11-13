/**
 * Globals hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 */

#include "hard_screen.h"

#include "hard_keyboard.h"


#define LED 27

void led(bool state=true) { digitalWrite(LED, state ? HIGH : LOW); }