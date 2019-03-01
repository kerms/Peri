#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

#define NBMAX_LED 32


static int leds[NBMAX_LED];
static int nbled = 0;
module_param_array(leds, int, &nbled, 0);

static int btn = 18;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

static int __init mon_module_init(void)
{
   printk(KERN_DEBUG "Hello YAS and KAI!\n");
   printk(KERN_DEBUG "btn=%d !\n", btn);
   printk(KERN_DEBUG "nbled=%d !\n", nbled);
   int i=0;
   for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
   return 0;
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye YAS and KAI!\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);