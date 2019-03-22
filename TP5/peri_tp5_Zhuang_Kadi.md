



### Protocole de communication sans-fil : ZigBee

Le ZigBee est un protocole de communication sans-fil "low-cost" à courte portée et à faible consommation énergétique. Il est basé sur la norme IEEE 802.15.4. Il est principalement utilisé dans les environnements embarqués.


Caractéristiques :
```
IEEE 					802.15.4
Besoins mémoire 		4-32 ko 
Autonomie avec pile 	Années 
Nombre de nœuds 		65 000+ 	
Vitesse de transfert 	20-250 kb/s 	
Portée (environ) 		10 m 
```



### Capteur d'étirement

On peut mesurer l'état ou la distance grâce à un étirement : [cordon en caouchouc étirable conducteur](https://www.adafruit.com/product/519 "Adafruit")

Ce capteur est passif (juste du caouchouc en noir de charbon), et donc ne consome pas d'énergie.

La valeur de la résistance varie selon l'état du cordon.

L'idée est lorsqu'on tire sur le cordon, la résistance augmente, puis selon la valeur de la résistance, on peut déterminer la longueur d'étirement.

Pour installer ce capteur, il suffit à priori de mettre deux connecteur sur les deux bouts du cordon. Puis relier ces connecteurs au circuit.

###### Remarque

Malheureusement ce capteur n'est pas précis.