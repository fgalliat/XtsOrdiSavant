/**
 * Speaker hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 * 
 * PWM driven Buzzer
 */

#define BUZZER 12
#define BUZ_channel 0
#define BUZ_resolution 8

void setupBuzzer() {                             
    pinMode( BUZZER, OUTPUT );                    
    digitalWrite( BUZZER, LOW );                  
    int freq = 2000;                              
    ledcSetup(BUZ_channel, freq, BUZ_resolution); 
    ledcAttachPin(BUZZER, BUZ_channel);           
}                                                 


void buzz_tone(int freq) {
    int vol = 125;
    vol = 200;
    ledcWrite(BUZ_channel, vol); // volume
    ledcWriteTone(BUZ_channel, freq); // freq
}

void buzz_noTone() {
    ledcWrite(BUZ_channel, 0); // volume
}

void beep(int freq, int duration) {
    buzz_tone(freq);
    delay(duration);
    buzz_noTone();
}

void bell() { beep(440, 200); }