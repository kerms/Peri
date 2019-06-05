# Projet PERI

```
Yasma KADI
Kailun ZHUANG
```
 - [ ] base de donnée
 - [ ] server web en php
 - [ ] client MQTT
 - [ ] esp32 avec MQTT
   - [ ] lecture périodique de données
   - [ ] reception d'odre




### Environnement

##### Accès

- On accède à `peri:8040+N` dans un navigateur en salle SESI.
- `peri:8040+N/phpmyadmin` pour accéder 


##### Configuration
On peut ajouter des fichiers `.conf` dans `/etc/apache2/sites-available`

pour écouter sur d'autre port et d'autre répertoire racine. ie créer un autre site.

Un fichier par défaut est présent dans ce même répertorie, on peut s'y inspirer.

Pour activer et désactiver l'émission wifi de la carte rpi

```
sudo systemctl start dnsmasq
sudo systemctl start hostapd
```


##### lib

[Apprendre le web](https://www.w3schools.com/ "w3schools.com")


##### Identification

Nous avons choisit d'utiliser Twitter Bootstrap, ce framework nous permet de faire une mise en page rapide et présentable.

Pour plus de modularité, on a fait deux fichiers `header.php` et `footer.php` qui seront inclus dans toutes les pages.

##### css

On peut choisir de mettre en forme des éléments grâce aux [css selectors](https://www.w3schools.com/cssref/css_selectors.asp "w3schools.com")


##### Client Capteur ESP32
[//]: # (TODO YASMA)

##### Client MQTT

*Choix* de langage:

On a trouvé notre librairie [ici](https://www.eclipse.org/paho/downloads.php).

On a choisit d'utiliser le C, car on le connaît, de plus il présente toutes les fonctionnalités. Pour l'installer, on récupère les sources sur [github](https://github.com/eclipse/paho.mqtt.c), la page principale sur le README nous indique comment installer: 

- make
- make install

On retrouve les librairies compilées dans `paho.mqtt.c-master/build/output`, maintenant pour compiler il faut copier les .h nécessaires situés dans `paho.mqtt.c-master/src/`.

Voici un exemple de commande pour compiler:

```bash
gcc -L path-to/paho.mqtt.c-master/build/output/ myprog.c -l paho-mqtt3c -o MQTTTest
```

[//]: # (TODO Kailun)
Install mysql -> new package `default-libmysqlclient-dev` not `libmysqlclient-dev`
probleme de reception de donnee en byte -> client.beginWrite
Serial2 ne marche pas avec OLED (librairie SSD1306), mais avec le wifi
compilation mysql en C : `--cflags --libs`
add code source link for MQTT CLIENT and Mysql
MQTT Client sample path : `paho.mqtt.c-master/build/output/samples`
Mysql [example](http://zetcode.com/db/mysqlc/)

```bash
gcc -L /home/pi/Documents/kadi-zhuang/paho.mqtt.c-master/build/output/ MQTTClient_subscribe.c -l paho-mqtt3c -o MQTTTest -Wall `mysql_config --cflags --libs`
```