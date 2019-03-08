# TP3 : pilote de périphérique

```
Kailun ZHUANG
Yasma KADI
```

### Étape 1 : création et test d'un module noyau

##### Code du module

On fait `insmod` pour charger un module, la fonction exécutée est :

```cpp
static int __init mon_module_init(void);
```

Puis `rmmod` pour décharger un module, la fonction exécutée est :

```cpp
static void __exit mon_module_cleanup(void);
```

##### Compilation du module

Dans le Makefile, nous avons changé la valeur des variables `CARD_NUMB`, `LOGIN` et `LAB`.

###### Remarque:
Quand on a essayé de compiler, on a eu une erreur due au fait de confondre la commande `modules` avec le nom de notre module. Pour cela, on a renomé le module par ledKZ. 

Ensuite, on a lancé sur la raspberry les commandes suivantes:

- Pour charger le module
```bash
sudo insmod ./ledKZ.ko 	#chargement
lsmod 					#affichage des modules existants
#Module                  Size  Used by
#ledKZ                    793  0 
$dmesg 
#[611313.142348] Hello World YAS and KAI!
```

- Pour décharger le module
```bash
sudo rmmod ledKZ	#dechargement
lsmod 				#le module ledKZ n'est plus dans la liste
dmesg 
#[611563.827875] Goodbye World!
```

### Étape 2 : ajout des paramètres au module

Dans cette partie, on a rajouté des paramètres pour le module:

```cpp
/* leds est un tableau qui contient les numéros de GPIO de LEDs */
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

```
Pour vérifier que les paramètres ont été bien lus, on a utilisé une boucle `for` 

```cpp
/* Affichage des LEDs et bouton pour voir*/
   printk(KERN_DEBUG "btn=%d !\n", btn);
   printk(KERN_DEBUG "nbled=%d !\n", nbled);
   int i=0;
   for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
```

```bash
sudo insmod ledKZ.ko leds=4,17
```

```
[  895.883364] Hello YAS and KAI!
[  895.883403] btn=18 !
[  895.883416] nbled=3 !
[  895.883427] LED 0 = 4
[  895.883440] LED 1 = 17
```
### Étape 3 : création d'un driver qui ne fait rien, mais qui le fait dans le noyau

Comme on travaille à plusieurs sur la même carte Raspberry, on nomme notre module ledKZ2 pour le distinguer et éviter toute collision. 

Suivant les même étapes décrites précédemment, on compile, déplace et charge le module.

Pour récupérer le numéro du `major`, on utilise la commande: 

```bash
cat /proc/devices | grep -n ledKZ2 
# 23:244 ledKZ2
```
Maintenant on crée le device à l'aide des commandes: 

```bash
sudo mknod /dev/ledKZ2 c 244 0
sudo chmod a+rw /dev/ledKZ2
``` 
On note que le fichier ledKZ2 existe dans le répertoire dev (commande `ls`).


### Étape 4 : accès aux GPIO depuis les fonctions du pilote

Dans cette étape, on modifie le driver précédent `ledKZ2` pour accéder aux registres des GPIO.

##### Le driver complet 

```cpp
static void gpio_read(int pin, char *buf, size_t buff_size) {
    printk(KERN_DEBUG "reglev[0] : %x %x\n", gpio_regs->gplev[btn / 32], ((gpio_regs->gplev[btn / 32])>>18)&1);
    if (buff_size) {
      if (((gpio_regs->gplev[btn / 32] >>  btn)&1) == 0) { // ATTENTION AUX PARANTHESES!!!
        buf[0] = '0';
      } else {
        buf[0] = '1';
      }
      printk(KERN_DEBUG "KY - read btn value = %c \n", buf[0]);
    }
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
    gpio_write(LED0, buf[0]); // dans notre code user, on écrit dans le buf[0]
    return count;
}

``` 
##### Script du chargement

```makefile
CARD     ?= 25
ROUTER    = peri
LOGIN     = kadi-zhuang
FILE      = ./TP3/modules/test.c
USER      = zhuang

all:

# transfert de la clé ssh et création du répertoire LOGIN
key_trans:
	ssh-copy-id -i ~/.ssh/id_rsa.pub -p 50$(CARD) pi@$(ROUTER)
	ssh -p 50$(CARD) pi@peri 'mkdir -p ~/$(LOGIN)'

# Se connecter
co: 
	ssh -p 50$(CARD) pi@peri

# envoyer un fichier avec make up FILE=My_file
up: 
	scp -P50$(CARD) $(FILE) pi@$(ROUTER):$(LOGIN)/

# décompresser linux dans /misc
import_linux : /dsk/l1/misc/$(USER)
	# cp /users/enseig/zhuang/linux-rpi-3.18.y.tbz2 /dsk/l1/misc/$(USER)/linux-rpi-3.18.y.tbz2
	tar xjf /users/enseig/zhuang/linux-rpi-3.18.y.tbz2 -C /dsk/l1/misc/$(USER)

/dsk/l1/misc/$(USER) : 
	mkdir /dsk/l1/misc/$(USER)
	chmod 740 /dsk/l1/misc/$(USER)
``` 
##### Programme de validation

```cpp

#include <stdio.h>

#define NBLED 2
#define NBBP 1
char led[NBLED];
char bp[NBBP];
 
int main()
{
   int i;
   int fd = open("/dev/ledKZ2", O_RDWR);
   if (fd < 0) {
      fprintf(stderr, "Erreur d'ouverture du pilote LED et Boutons\n");
      exit(1);
   }
   for( i = 0; i < NBLED; i ++) {
      led[i] = '0';
   }
   do { 
      led[0] = (led[0] == '0') ? '1' : '0';
      write( fd, led, NBLED);
      sleep( 1);
      read( fd, bp, 1);
   } while (bp[0] == '1');
   return 0;
}
``` 
### Conclusion

Après quelques heures de debug, on s'est rendu compte qu'il fallait rajouter le mot clé `volatile *gpio_regs` pour que la valeur instantanée des registres ne soit pas optimisée par le compilateur. Ainsi, on a réussit à créer notre driver, à l'utiliser pour faire clignoter les LEDs et lire la valeur du bouton. 