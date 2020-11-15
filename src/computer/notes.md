# Notes



**TODO**

- keyboard
  - key-repeat (IDLE or HOLD)
- Console
  - tty mem / attr / scrolling 
    - 80x40 chars -> 3200 bytes
  - VT100 handler
    - @least for : cursor, cls
    - later for accentColor, reverse video
  - try to keep functions that can be emulated on regular VT100 bash console ('nux box) 



**Deprecated**

ILI9486 - hardware scrolling

https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/TFT_Terminal/TFT_Terminal.ino

// We are using a hardware feature of the display, so we can only scroll in portrait orientation

**DEF** cmd -> https://www.displayfuture.com/Display/datasheet/controller/ILI9486L.pdf

0x33



**ADD** cmd VSCRSADD (Vertical Scrolling Start Address) 

0x37

