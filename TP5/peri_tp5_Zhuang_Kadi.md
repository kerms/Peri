
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

Pour installer ce capteur, il suffit à priori de mettre deux connecteurs sur les deux bouts du cordon. Puis relier ces connecteurs au circuit.

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

#### Programmes de test sur ordinateur perso (WINDOWS)

Blink, SCAN-WIFI et SCAN-

###### OLED DEMO sur ordi perso 

On a utiliser cette [librairie](https://github.com/osresearch/esp32-ttgo "github") :

Cependant, on a eu quelques erreurs de compilation, en regardant en détail, on remarque que le compilateur a considéré certains warnings comme des erreurs : 

	- uninitiliazed variable
	- statement missing in switch-case

Puis une erreur dans une fonction qui ne retourne rien, alors que le type de retour est un `bool`.

Après ces corrections, la démo s'affiche sur notre carte.

###### Combinaison OLED et capteur ultra son 

Afin d'afficher les valeurs mesurées par le capteur sur l'écran OLED, on a "fusionné" le code de la démo OLED et un code tuto du capteur ultra son de arduino [trouvé ici](https://www.carnetdumaker.net/articles/mesurer-une-distance-avec-un-capteur-ultrason-hc-sr04-et-une-carte-arduino-genuino/ "Tuto ultrason").

```cpp
void setup() {
  /* Init ultra son */
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);

  /* Init OLED */
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();

	// The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  ui.setTargetFPS(60);
}


void loop() {
  // clear the display
  display.clear();
    /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

    /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
  long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
   
  /* 3. Calcul la distance à partir du temps mesuré */
  float distance_mm = measure / 2.0 * SOUND_SPEED;
  
  /* 4. affichage sur OLED */
  display.drawString(0, 0,  "Distance: ");
  display.drawString(0, 10, String(distance_mm) +  "mm");
  display.drawString(0, 20, String(distance_mm / 10.0, 2) + "cm");
  display.drawString(0, 30, String(distance_mm / 1000.0, 2) + "m");
  display.display();

  /* limite le nombre de rafraichissement */
  delay(500);
}


``` 

Au début, le capteur ultrason n'était pas bien connecté à l'ESP32. En effet, il y avait un problème avec le cablage physique. Donc, on a refait les connexions utilisant des fils femelle-femelle (sans la breadboard). Ainsi, on affichait bien la distance.

En fin, le code - qu'on a présenté dans ce rapport - permet d'afficher la distance mesurée par le capteur directement sur l'écran OLED.
