/**
 * FileSystem hardware definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 * 
 * SPI driver SDCard reader
 * 
 * 
 */


// ==== Custom SPI SdCard ==== 
// beware if "WiFi.h" is called after : need to redeclare file access flags 
// else can't CREATE new file !!!
#include <SdFatmoa.h>
#define SD_CS 13
#define SD_MISO 2
#define SD_MOSI 4
#define SD_SCK 14

// in SdFatConfig.h (of SdFatLib)
//  If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
//  will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
//  the class SdFatSoftSpiEX will be defined.
//  These classes used extended multi-block SD I/O for better performance.
//  the SPI bus may not be shared with other devices in this mode.

SdFatSoftSpiEX<SD_MISO, SD_MOSI, SD_SCK> SD; // MISO, MOSI, SCK 

bool setupFs() {
    if ( !SD.begin(SD_CS) ) {
        // Serial.println("SD Failed to begin !");
        // con_puts("SD Failed to begin !");
        return false;
    }
    return true;
}