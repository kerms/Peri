#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

#define NBMAX_LED 32
#define LED0 4
#define LED1 17

static int major;
static int leds[NBMAX_LED];
static int nbled = 2;
static int btn = 18;

struct gpio_s
{
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
}
*gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

static int count = 0;

static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

/* Do no fill with '\n' */
static void gpio_read(int pin, char *buf, size_t buff_size) {
    if (buff_size) {
        buf[0] = gpio_regs->gplev[btn / 32];
        buf[1] = gpio_regs->gplev[btn / 32] >> 8;
        buf[2] = gpio_regs->gplev[btn / 32] >> 16;
        buf[3] = gpio_regs->gplev[btn / 32] >> 24;
    }
}


static int 
open_ledbp(struct inode *inode, struct file *file) {
    count ++;
    printk(KERN_DEBUG "KY - open() : cpt = %d\n", count);
    return 0;
}

static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "KY - read()\n");
    gpio_read(LED0, buf, count);
    return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "KY - write()\n");
    gpio_write(LED0, buf[0]);
    return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
    count --;
    printk(KERN_DEBUG "KY - close() : cpt = \n", count);
    return 0;
}
struct file_operations fops_ledbp =
{
    .open       = open_ledbp,
    .read       = read_ledbp,
    .write      = write_ledbp,
    .release    = release_ledbp 
};

static int __init mon_module_init(void)
{
   major = register_chrdev(0, "ledKZ2", &fops_ledbp);
   printk(KERN_DEBUG "KY - Hello YAS and KAI!\n");
   printk(KERN_DEBUG "KY - btn=%d !\n", btn);
   printk(KERN_DEBUG "KY - nbled=%d !\n", nbled);
   int i=0;
   for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
   gpio_fsel(btn, 0);
   return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "ledKZ2");
    printk(KERN_DEBUG "KY - Goodbye YAS and KAI!\n");
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);

