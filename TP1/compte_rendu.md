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
Les macro `GPIO_CONF_AS_INPUT` et `GPIO_CONF_AS_OUTPUT` permettent de configurer un GPIO comme entrée et sortie respectivement. \

Les macro `GPIO_SET` et `GPIO_CLR` permettent d'écrire respectivement '1' et '0' sur le `GPIO`.

Pour faire clignoter la `LED0` on a rajouté dans la boucle `while`  la macro `GPIO_CLR ( gpio_base, GPIO_LED0 )` entre les  `delay`.

