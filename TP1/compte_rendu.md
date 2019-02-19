# TP1 sur Raspberry 25

```
Yasma KADI
Kailun ZHUANG
```

#### Accès

Après la compilation on se connecte avec x = 25
```
$ ssh -p 50x pi@peri
```

Pour envoyer un fichier sur la `raspberry` on utilise la commande : 
`scp -P 50x fichier pi@peri:nom_fichier`

L'exécution n'a pas marché avec `./`. Il fallait lancer en mode Super User : `sudo ./`

###### Remarque
Comme c'est un Linux qui tourne, on peut même envoyer les fichiers sources pour déboguer avec `gdb`!


### Contrôle de GPIO
Les macro `GPIO_CONF_AS_INPUT` et `GPIO_CONF_AS_OUTPUT` permettent de configurer un GPIO comme entrée et sortie respectivement.\
Les macro `GPIO_SET` et `GPIO_CLR` permettent d'écrire respectivement '1' et '0' sur le `GPIO`.\


### Faire clignoter un led
Un clignotement n'est juste que: 
  - on alume
  - on attend un certain temps
  - on ferme
  - on attend un certain temps
Pour faire clignoter en permanence, il suffit de faire boucler un clignotement puis attendre.\
Pour faire clignoter la `LED0` on a rajouté dans la boucle `while`  la macro `GPIO_CLR ( gpio_base, GPIO_LED0 )` entre les  `delay`.

### Faire clignoter deux leds à fréquence différente.

Une première aproche simple est de faire de façon itérative.\
L'idée de dupliquer le code de la boucle en insérant le clignotement de `LED1`, par exemple si on veut faire clignoter `LED0` deux fois plus vite que `LED1`,:
  - on alume `LED0`
  - on alume `LED1`
  - `delay`
  - on ferme `LED0`
  - `delay`
  - on alume `LED0`
  - on ferme `LED1`
  - `delay`
  - on ferme `LED0`
  - `delay`

On peut aussi faire un compteur et tester si à chaque tour de boucle on doit fermer un LED ou l'ouvrire. Mais cela sera pas éfficasse.\

La deuxième aproche est de faire des [I'm an inline-style link](https://openclassrooms.com/fr/courses/1513891-la-programmation-systeme-en-c-sous-unix/1514567-les-threads "OpenclassRoom").\

Pour factoriser le code, on peut créer une structure qui comporte les informations nécessaires au éxécution:
 - L'adresse de début des GPIOs
 - Le numéro de GPIO
 - La fréquence de fonctionnement
On a alors besoin qu'une seule fonction pour faire clignoter N LED à fréquence différent, chacun indépendament.\
Dans le code de la fonction, on met un while qui fait tout simplement:
  - on alume
  - on attend `struct.delay`
  - on ferme
  - on attend `struct.delay`
Par exemple:\
Pour le `LED0`, on le passe son numéro qui est 4, puis `delay`\
Pour le `LED1`, on le passe son numéro qui est 17, puis `delay`/3

###### Remarque
il faut faire join avant le return du main, sinon le programme n'attend pas les threads.

### Changer la fréquence de clignotement par un bouton

On ajoute une variable global `switch` qui à 0 fonctionnement normal, puis 1 pour changement de fréquence.\

On change le code dans la fonction de threads: \
Il suffit de faire `delay*(switch + 1)` pour diminuer de moitié la fréquence.

