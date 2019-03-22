#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include <linux/ioctl.h>

#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

struct Pos
{
    int x;
    int y;
};


#define MAGIC 220
#define LCDIOCT_CLEAR _IO(MAGIC, 1)
#define LCDIOCT_SETXY _IOW(MAGIC, 2, struct Pos *)

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

//volatile struct gpio_s *gpio_regs;

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 ******************************************************************************/

#define BITN(b,n)   (((n)>>(b))&1)
#define MIN(a,b) (((a)<(b))?(a):(b))

/* commands */
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

#define CHINA_FLAG_1 { 0x01, 0x01, 0x02, 0x1C, 0x08, 0x04, 0x02, 0x05 }
#define CHINA_FLAG_2 { 0x10, 0x10, 0x08, 0x07, 0x02, 0x04, 0x08, 0x14 }
#define CHINA_FLAG_3 { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define CHINA_FLAG_4 { 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

#endif