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

``` 
##### Script du chargement

```cpp

``` 
##### Programme de validation

```cpp

``` 

