# Projet PERI : Réseau de capteurs minimaliste

```
Yasma KADI
Kailun ZHUANG
```

### Objectif

Notre objectif consiste à implémenter un système de communication entre un utilisateur et un réseau de capteurs. Les informations provenantes de ce dernier sont transférées à travers un serveur MQTT. Nous devrons aussi implémenter un mécanisme de stockage daté, i.e. une base de données, permettant ainsi à l'utilisateur d'y accéder à des temps flexibles à travers le serveur web qui est connecté à cette base de données. 

### Outils

Nous avons utilisé deux composants essentiels, un ESP32 et une Raspberry Pi 3.

##### L'ESP32

L'ESP32 est un [nano-computer](https://www.espressif.com/en/products/hardware/esp32/overview) à 32 bits qui possède une interface Wi-Fi. Dans ce projet, il est utilisé pour piloter l'ensemble de capteurs (le capteur ultrason).

##### La Raspberry PI 3

La Raspberry Pi 3 est aussi un nano-computer [nano-computer](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/) que nous utilisons pour la partie serveur et base de données. 

###### Accès et configuration

- Taper `peri:8040+N` dans un navigateur en salle SESI pour accéder à notre site web.
- `peri:8040+N/phpmyadmin` pour accéder à la base de données.

*Remarque :* 
On peut ajouter des fichiers `.conf` dans `/etc/apache2/sites-available` afin d'écouter sur d'autres ports et d'autres répertoires racines, i.e. créer un autre site.
Un fichier par défaut est présent dans ce même répertorie, on peut s'y inspirer.

Commandes pour activer/désactiver l'émission wifi de la carte rpi

```bash
sudo systemctl start dnsmasq
sudo systemctl start hostapd
```

### Implémentation

#### Client Capteur ESP32

Au niveau de l'ESP32, nous souhaitons acquérir les données du capteur ultrason, qui mesure la distance en milimètre, centimètre voire mètre, et enfin les envoyer à la Raspberry Pi3 par un réseau WiFi. 
Dans ce sens, nous avons écrit un client en langage C++. Celui-ci permet de récupérer la valeur de la distance en milimètres et l'envoyer toutes les 3 secondes. Les deux fonctions `loop_ultrason_val()` et `loop_ultrason_send(float distance_mm)` qui assurent ce comportement sont implémentées à base de la fonction `waitFor()` fournie par notre professeur.

Nous avons pensé à utiliser trois `TouchPins` de l'ESP32 pour afficher la valeur de la distance en trois unités différentes. Cepandent, nous n'avons pas pu régler les seuils variables des `TouchPins` et ainsi nous avons abandonné cette idée vu la contrainte de temps. 

*Remarque :*
Il est possible de rajouter d'autres capteurs, par exemple, la photorésistance que nous avons avec l'arduino. Toutefois, en raison de simplicité, nous nous sommes contentés du capteur ultrason.

##### Optimisation de transfert MQTT

Nous avons choisit d'envoyer les données sous forme de byte, et non de chaînes de caractères, cela permet de réduire l'utilisation de la bande passante, réduire le temps de transmission, donc réduction de coût énergitique. Cependant on a eut un souci avec la méthode `publish` de la class `PubSubClient`, car elle lit la chaîne jusqu'au `\0`, ce qui envoie un message de taille 0 si le message contient seulement un entier avec une petite valeur, tel qu'un `1`.

On s'est documenté sur la class [`PubSubClient`](https://pubsubclient.knolleary.net/api.html). On a trouvé le couple de méthode `beginPublish()`/`endPublish()` qui nous permet d'envoyer des octets brutes.

Les données du capteur sont maintenant envoyées sous ce format:

```
<- 4 octets-> <- 4o  ->
+------------+--------+
| id capteur | valeur |
+------------+--------+
```

#### Raspberry PI 3

##### Serveur MQTT : Mosquitto


[MQTT](https://www.ass-security.fr/blog/protocole-mqtt-iot-objets-connectes-cyberattaque) est un protocole de communication ouvert déployé sur la couche réseau TCP/IP. Fonctionnant sur le principe de « publication/abonnement » Publish/Suscribe, MQTT est un protocole à l’opposé du standard Web HTTP(S) Hyper Texte Transfer Protocole conçu pour la navigation internet et fonctionnant sur le principe des « questions/réponses - Request/réponse ». 

```
+-------------------+            +-------------------+         +-------------------+        
|   Publisher       |            |    Broker         |         |    Subscriber     | 
|                   |            |                   |         |                   | 
|    Capteur        | -------->  |    Mosquitto      | ------->|    Client MQTT    |
|    ultrason       |            |    Raspberry 3    |         |    Raspberry 3    | 
|                   |            |                   |         |                   | 
+-------------------+            +-------------------+         +-------------------+ 
```

*Installation :* 

```bash
sudo apt-get install mosquitto mosquitto-clients python-mosquitto
```

##### Base de données : MySQL

Nous avons vu en cours deux types de bases de données : MariaDB et MySQL. Nous avons opté pour MySQL. Ce choix est justifié par ce [benchmark](https://mariadb.com/resources/blog/mariadb-5-3-optimizer-benchmark/). 

La structuration de notre base de données est la suivante :
```
+----------+
| type_cap |
+----------+
| id       |
| nom      |
+----------+
     |
    /|\
+------------+
| capteur    |
+------------+
| id : INT   |
| nom: STR   |
| id_type    |
+------------+
     |
     |
    /|\
+---------------+
| capteur_value |
+---------------+
| id            |
| value         |
| capteur_id    |
| time          |
+---------------+
```

##### Client MQTT

Le client MQTT est un souscripteur (subscriber) d'un même sujet (topic) où notre ESP32 publie (publish). Ainsi, il reçoit les données de notre capteur qu'il insérera, ensuite, dans notre base de données. Nous avons implémenté ce client en lagage C. 

*Choix de langage :*

On a choisit d'utiliser le C, car on le connaît, de plus il présente toutes les [fonctionnalités](https://www.eclipse.org/paho/downloads.php).

Pour l'installer, on récupère les sources sur [github](https://github.com/eclipse/paho.mqtt.c), la page principale sur le README nous indique comment installer: 

- make
- make install

On retrouve les librairies compilées dans `paho.mqtt.c-master/build/output`. Maintenant, pour compiler, il faut copier les `.h` nécessaires situés dans `paho.mqtt.c-master/src/`.

Voici un exemple de commande pour compiler :

```bash
gcc -L path-to/paho.mqtt.c-master/build/output/ myprog.c -l paho-mqtt3c -o MQTTTest
```

###### Installation de mysql client
Install mysql : la librairie `libmysqlclient-dev` n'existe plus pour cause d'obscolecence donc on utilise ~~`libmysqlclient-dev`~~ > `default-libmysqlclient-dev`;

compilation mysql en C : `mysql_config --cflags --libs`

On a utilisé les fichiers examples de MQTT Client dans : `paho.mqtt.c-master/build/output/samples`

Mysql [example](http://zetcode.com/db/mysqlc/)


```bash
gcc -L /home/pi/Documents/kadi-zhuang/paho.mqtt.c-master/build/output/ MQTTClient_subscribe.c -l paho-mqtt3c -o MQTTTest -Wall `mysql_config --cflags --libs`
```

##### Serveur HTTP : Apache

Pour la partie `web` nous nous sommes servis de la documentation de [w3schools](https://www.w3schools.com/ "w3schools.com").

On peut choisir de mettre en forme des éléments grâce aux [css selectors](https://www.w3schools.com/cssref/css_selectors.asp "w3schools.com")

Nous avons utilisé le framework : [Twitter Bootstrap](https://getbootstrap.com/docs/4.0/components/alerts/). Ce framework nous permet de faire une mise en page rapide et présentable.

Pour plus de modularité, nous avons rajouté deux fichiers `header.php` et `footer.php` qui seront inclus dans toutes les pages.

En raison de simplicité, notre site contient une seule page pour l'instant. Cette page récupère les données du capteur n° `1` dans la base de donées, puis les représente sous forme de [graphe](https://canvasjs.com/php-charts/).

### Problèmes rencontrés

Nous avons d'abord téléchargé le code source de la librairie pré-compilé sur un ordinateur de la salle SESI. Les libraries `.so` dans cette archive produisent `undefined function xxx`. On a résolue en téléchargeant le code source depuis github.

Ensuite, toujours avec notre client MQTT, il nous a fallu prendre du temps pour comprendre que la librairie `libmysqlclient-dev` n'existe plus, et est remplacée par la nouvelle librairie `default-libmysqlclient-dev`;

Le `canvaschart` n'affiche rien au début, car nous n'avons pas utilisé des apostrophes autour de `rows["time"]`.


##### Amélioration 

Le graphe de notre site affiche un graphe statique, il faut la raffraîchir pour visualiser les nouvelles valeurs, le mieux c'est d'utiliser le `javascript` pour rendre ce processus dynamique.

Un réseau de capteurs avec qu'un seul capteur n'est pas un réseau! Donc, pour un "vrai" réseau, il est indisponsable de rajouter plus de capteurs.

### Conclusion

À l'issu de ce projet, nous avons globalement réussi notre réseau de capteurs : l'utilisateur, qui se connecte à notre page web, arrive à visualiser la courbe des valeurs du capteur ultrason. En outre, le travail effectué nous a permis de mettre en oeuvre tous les concepts et connaissances vus et acquis lors des derniers cours de l'UE PERI, notamment, le protocole MQTT et les bases de données. 

Nous n'avons toutefois pas réussi à mettre en place l'OLED. L'écran n'affiche rien pour une raison inconnue.

Enfin, il n'est pas possible de conclure sans remercier Monsieur Franck WAJSBURST pour son enseignement aisni que pour son encadrement. 