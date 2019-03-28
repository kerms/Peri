
# TP5: Périphériques et ESP32

```
Yasma KADI
Kailun ZHUANG
```


### Protocole de communication sans-fil : ZigBee

Le ZigBee est un protocole de communication sans-fil à courte portée et à faible consommation énergétique. Il est basé sur la norme IEEE 802.15.4. Il est principalement utilisé dans les environnements embarqués.


Caractéristiques :
```
IEEE 			802.15.4
Besoins mémoire 	4-32 ko 
Autonomie avec pile 	Années 
Nombre de nœuds 	65 000+
Vitesse de transfert 	20-250 kb/s
Portée (environ) 	10 m 
```
Pour plus de details sur ce protocole, cliquez [ici](https://connect.ed-diamond.com/MISC/MISC-086/Tout-tout-tout-vous-saurez-tout-sur-le-ZigBee "ZigBee")

**FUN FACT**
 
La communication chez les abeilles permet la cohésion de la ruche, la reconnaissance entre individus, la diffusion des alertes, mais également le repérage des sources de nourriture, d'eau, de résines, ou des emplacements possibles d'implantation. Elle est basée sur des échanges tactiles à l'aide des antennes et sur un comportement remarquable: les danses des abeilles en zigzag. C'est de cette dance que le ZigBee tient son nom. 


### Capteur d'étirement

On peut mesurer l'état ou la distance grâce à un étirement : [cordon en caouchouc étirable conducteur](https://www.adafruit.com/product/519 "Adafruit")

Ce capteur est passif (juste du caouchouc en noir de charbon), et donc ne consome pas d'énergie.

La valeur de la résistance varie selon l'état du cordon.

L'idée est lorsqu'on tire sur le cordon, la résistance augmente, puis selon la valeur de la résistance, on peut déterminer la longueur d'étirement.

Pour installer ce capteur, il suffit à priori de mettre deux connecteur sur les deux bouts du cordon. Puis relier ces connecteurs au circuit.

###### Remarque

Malheureusement ce capteur n'est pas précis.


### Manipulation de l'ESP32

#### Programmes de test sur ordinateur de SESI

###### Blink : OK

On voit bien la LED de la carte qui clignote.

###### SCAN-WIFI : FAIL 

**Résolution** : 

On a trouvé une solution [ici](https://github.com/espressif/arduino-esp32/issues/333#issuecomment-319237961 "github") :

	- Il fallait rester appuyer sur le bouton `boot` au moment de téléversement.
	- Lâcher une fois le téléversement fini.
	- Appuyer sur le bouton reset `RST`

Maintenant, on voit tous les réseaux wifi disponibles (ex: eduroam, eduspot...).

###### SCAN-BLE  : OK

On voit les réseaux bluetooth.

###### OLED DEMO

On a utiliser cette [librairie](https://github.com/osresearch/esp32-ttgo "github") :
