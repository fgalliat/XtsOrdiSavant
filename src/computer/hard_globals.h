/**
 * Globals hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 */

#include "hard_screen.h"

#include "hard_keyboard.h"

#include "hard_speaker.h"


#define LED 27

void led(bool state=true) { digitalWrite(LED, state ? HIGH : LOW); }