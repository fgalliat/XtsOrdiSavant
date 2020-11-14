/**
 * Soft console definition for XtsOrdiSavant computer
 * 
 * Xtase - fgalliat @Nov 2020
 * 
 */

// =======================================================
// == Output                                            ==
// =======================================================
void con_cls() { cls_tft(); }
void con_gotoXY(int x, int y) { gotoXY_tft(x,y); }
void con_putc(char ch) { write_tft(ch); } 
void con_puts(char* str) { print_tft(str); } 

// =======================================================
// == Input                                             ==
// =======================================================
void getline_echo(char ch, int index) {
  con_putc(ch);
}

int con_getch() {
  return getch_kb();
}

int con_getche() {
  int ch = con_getch();
  if ( ch > 0 ) { getline_echo( (char)ch, 0 ); }
  return ch;
}

/** returns NULL if Ctrl-C 
 * beware the returned line ismemory shared, can't free(..) or delete(..)
 * need to strcpy() elsewhere to save content
*/
char* con_getline(bool echo=true, int maxLen=MAX_KB_LINE_LEN) {
  return getline_kb(echo, maxLen);
}

bool con_kbhit() {
  return available_kb();
}

