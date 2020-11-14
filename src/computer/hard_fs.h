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

SdFile file;
void con_puts(char* str);
void con_putc(char c);

bool fs_ls(char* path) {
  uint32_t pos = 0;
  SD.chdir(path);

  char name[13]; memset(name, 0x00, 13);
  while (1) {
    SD.vwd()->seekSet(pos);
    if (!file.openNext(SD.vwd(), O_READ)) break;
    pos = SD.vwd()->curPosition();
    bool z = file.isDir();
    // file.getFilename(name);
    file.getName(name, 13);
    con_puts(name); con_puts("\n");
    file.close();
    // if (z) ls(name);
  }

  return true;
}

bool fs_cat(char* path) {
    File f = SD.open( path, O_READ );
    if (!f) {
        return false;
    }
    // FIXME : read in a buffer instead of byte per byte
    while( f.available() ) {
        con_putc( f.read() );
    }
    return true;
}