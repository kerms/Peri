#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>



#define NBMAX_LED 32

#define LED0 = 4;
#define LED1 = 17;

static int major;
static int leds[NBMAX_LED];
static int nbled = 2;
static int btn = 18;

led[0] = LED0;
led[1] = LED1;

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

// 0 est le numéro majeur qu'on laisse choisir par linux

static int 
open_ledbp(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "KY - open()\n");
    return 0;
}

static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "KY - read()\n");
    return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "KY - write()\n");
    return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "KY - close()\n");
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
   return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "ledKZ2");
    printk(KERN_DEBUG "KY - Goodbye YAS and KAI!\n");
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);

