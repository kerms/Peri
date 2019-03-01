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
Quand nous avons essayé de compiler, nous avons eu une erreur due au fait de confondre la commande `modules` avec le nom de notre module. Pour cela, nous avons renomé le module par ledKZ. 

Ensuite, nous avons lancé sur la raspberry les commandes suivantes:

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

Dans cette partie, nous avons rajouté des paramètres pour le module:

```cpp
/* leds est un tableau qui contient les numéros de GPIO de LEDs */
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

```
Pour vérifier que les paramètres ont été bien lus, nous avons utilisé une boucle `for` 

```cpp
/* Affichage des LEDs et bouton pour voir*/
   printk(KERN_DEBUG "btn=%d !\n", btn);
   printk(KERN_DEBUG "nbled=%d !\n", nbled);
   int i=0;
   for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
```

```bash
sudo insmod ledKZ.ko leds=4,17,26
```

```
[  895.883364] Hello YAS and KAI!
[  895.883403] btn=18 !
[  895.883416] nbled=3 !
[  895.883427] LED 0 = 4
[  895.883440] LED 1 = 17
[  895.883450] LED 2 = 26
```
### Étape 3 : création d'un driver qui ne fait rien, mais qui le fait dans le noyau
