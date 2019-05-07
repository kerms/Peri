#include "WiFi.h"
#include <U8x8lib.h>

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

char mess[30];

void setup()
{
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop()
{
  int v=analogRead(36);
  sprintf(mess,"m=%4.4d",v);
  u8x8.drawString(0, 0, mess);
    
  delay(100);
}
