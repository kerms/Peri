#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <asm/delay.h>
#include <linux/uaccess.h>

#include "lcd_driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");


#define RPI_BLOCK_SIZE  0xB4
#define RPI_GPIO_BASE   0x20200000

struct gpio_s {
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
} volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

static const int lines[] = { 0, 0x40, 0x14, 0x54 };
static struct Pos current_pos = {0, 0};
static int count = 0;

#define MOD_NAME "lcd_driverkz"


void gpio_write(int gpio, int value)
{
    int regnum = gpio / 32;
    int offset = gpio % 32;
    if (value)
        gpio_regs->gpset[regnum] = (0x1 << offset);
    else
        gpio_regs->gpclr[regnum] = (0x1 << offset);
}

void gpio_config(int gpio, int value)
{
    int regnum = gpio / 10;
    int offset = (gpio % 10) * 3;
    gpio_regs->gpfsel[regnum] &= ~(0x7 << offset);
    gpio_regs->gpfsel[regnum] |= ((value & 0x7) << offset);
}






/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

/* generate E signal */
void lcd_strobe(void)
{
    gpio_write(E, 1);
    udelay(1);
    gpio_write(E, 0);
}

/* send 4bits to LCD : valable pour les commande et les data */
void lcd_write4bits(int data)
{
    /* first 4 bits */
    gpio_write(D7, BITN(7, data));
    gpio_write(D6, BITN(6, data));
    gpio_write(D5, BITN(5, data));
    gpio_write(D4, BITN(4, data));
    lcd_strobe();

    /* second 4 bits */
    gpio_write(D7, BITN(3, data));
    gpio_write(D6, BITN(2, data));
    gpio_write(D5, BITN(1, data));
    gpio_write(D4, BITN(0, data));
    lcd_strobe();
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd);
    udelay(2000);               // certaines commandes sont lentes 
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character);
    udelay(50);
}

void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);
    current_pos.x = 0;
    current_pos.y = 0;
}



void lcd_init(void)
{
    gpio_write(E, 0);
    lcd_command(0b00110011);    /* initialization */
    lcd_command(0b00110010);    /* initialization */
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF);
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);
}

void lcd_set_cursor(int x, int y)
{   
    // condition invalide
    if (x > 19 || y > 3 || x < 0 || y < 0) {
        printk("bad cursor\n");
        return;
    }
    lcd_command(LCD_SETDDRAMADDR + lines[y] + x);
    current_pos.x = x;
    current_pos.y = y;
}

void lcd_set_custom(int num, char buff[8]) {
    int i;

    if (num > 8 || num < 8) {
        printk(KERN_DEBUG "KY - custom : error num = %d\n", num);
        return;
    }
    lcd_command(LCD_SETCGRAMADDR + num );
    for ( i= 0; i < 8; ++i)
    {
        lcd_data(buff[i]);
    }
}

static int 
open_lcd_kz(struct inode *inode, struct file *file) {
    count ++;
    printk(KERN_DEBUG "KY - open() : cpt = %d\n", count);
    return 0;
}


static ssize_t 
write_lcd_kz(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	int i = 0, line = 0;

    line = current_pos.y;

    lcd_command(LCD_SETDDRAMADDR + current_pos.x + lines[line%4] );

    for ( ; i < count; ++i)
    {
        if ( current_pos.x % 20 == 0 ) {
            current_pos.x = 0;
            current_pos.y = (current_pos.y+1) % 4;
            lcd_command(LCD_SETDDRAMADDR + lines[line]);
        }
        lcd_data(buf[i]);
        ++current_pos.x;
    }

    return count;
}

static int 
release_lcd_kz(struct inode *inode, struct file *file) {
	count --;
    printk(KERN_DEBUG "KY - open() : cpt = %d\n", count);
    return 0;
}

static long my_ioctl(struct file *filep,unsigned int cmd, unsigned long arg) {
	struct Pos p;
    struct Text t;
    struct CusChar c;
    int sz, err;
    switch(cmd)
    {
  		case LCDIOCT_CLEAR :
	      	lcd_clear();
	      	return 0;
    	case  LCDIOCT_SETXY :
	        err = copy_from_user(&p, (struct Pos*)arg, sizeof(struct Pos));
	        lcd_set_cursor(p.x, p.y);
      	    return 0;
        case LCDIOCT_SETCUSTOM:
            err = copy_from_user(&c, (struct CusChar*)arg, sizeof(struct CusChar));
            lcd_set_custom(c.num, c.str);
            return 0;
        case LCDIOCT_WRITE :
            err = copy_from_user(&t, (struct Text*)arg, sizeof(struct Text));
            sz = t.sz;
            err = copy_from_user(&t, (struct Text*)arg, sizeof(struct Text) + t.sz);
            return write_lcd_kz(NULL, t.str, sz, NULL);
        default :
        return -EINVAL;     /* Invalid argument   */
    }
}

struct file_operations fops_lcd_kz =
{
    .open       	= open_lcd_kz,
    .write      	= write_lcd_kz,
    .unlocked_ioctl = my_ioctl,
    .release    	= release_lcd_kz 
};

static int __init mon_module_init(void)
{
	if( register_chrdev(MAGIC, MOD_NAME, &fops_lcd_kz) ){
    	return EBUSY;
  	}

    /* Setting up GPIOs to output */
    
    gpio_config(RS, GPIO_OUTPUT);
    gpio_config(E,  GPIO_OUTPUT);
    gpio_config(D4, GPIO_OUTPUT);
    gpio_config(D5, GPIO_OUTPUT);
    gpio_config(D6, GPIO_OUTPUT);
    gpio_config(D7, GPIO_OUTPUT);
    
    
    lcd_init();
    lcd_clear();
	

   	printk(KERN_DEBUG "KY - module "MOD_NAME" added!\n");
   	return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(MAGIC, MOD_NAME);
    printk(KERN_DEBUG "KY - module "MOD_NAME" removed!\n");
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);


