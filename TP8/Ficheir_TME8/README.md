# TME 8
#### François Pécheux

# Vocabulaire SQL

* SQL = Structured Query Language
* DDL Data Definition Language : 
	- create table 
	- drop schema
* DML Data Manipulation Language :
	- faire des recherches 				    select
	- insérer des données, enregistrement	insert 10%
	- supprimer des enregistrements			delete 2%
	- modifier un enregistrement			update 5%

## Commandes d'insertion

```sql
INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('mufasa', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "panthera leo"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Temp1', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "temp%"));

INSERT INTO `mesure` (`valeur`, `date`, `capteur_idcapteur`) VALUES (23, '2016-04-12 00:00:00', (SELECT idcapteur FROM `capteur` c,`type_capteur` tc,`enclos` e WHERE e.nom like "félins" and e.idenclos=c.enclos_idenclos and tc.nom like "temp%" and c.type_capteur_idtype_capteur=tc.idtype_capteur));
```

# Construction de la base de données

1. le fichier zoo7.mwb contient le modèle relationnel de notre base pour mysql-workbench

2. le fichier ddlzoo.sql contient les ordres de création de la base pour MariaDB. Ce fichier est obtenu lorsque dans le menu Database de mysql-workbench on choisit l'option "Forward engineer..." et que l'on sauve avant de se connecter à MariaDB.

3. La commande DML suivante peuple la table type_animal
```sql
INSERT INTO `type_animal` (`idtype_animal`, `nom`) VALUES
(1, 'panthera leo'),
(2, 'panthera tigris'),
(3, 'pinguinus'),
(4, 'primates'),
(5, 'proboscidea'),
(6, 'hyeanidae');
```

4. La commande suivante peuple la table enclos

```sql
INSERT INTO `enclos` (`idenclos`, `nom`, `superficie`, `date_de_construction`, `description`) VALUES
(1, 'félins', 5000, '2019-04-12 00:00:00', 'pour les lions et les tigres'),
(2, 'orang outan', 300, '2019-04-03 00:00:00', 'pour les hommes des forêts'),
(3, 'pingouins', 3000, '2019-04-05 00:00:00', 'pour les pingouins'),
(4, 'hyènes', 4000, '2019-04-06 00:00:00', 'pour les hyènes'),
(5, 'éléphants', 5000, '2019-04-01 00:00:00', 'pour les éléphants');
```

5. Maintenant que ces deux tables importantes sont créées, on peut peupler la table animal. La création des deux tables précédentes doit nécécessairement ête faite avant, car la table animal contient deux colonnes de type clé étrangère qui doivent être remplies pour que l'enregistrement d'un nouvel animal soit valable. Les commandes suivantes sont des exemples de commandes SQL "enchassées". Ici un INSERT repose sur deux SELECT

```sql
INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('mufasa', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "panthera leo"));

INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('nala', '2018-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "panthera leo"));

INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('simba', '2018-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "panthera leo"));

INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('shere khan', '2018-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "panthera tigris"));

INSERT INTO `animal` (`petitnom`, `date_naissance`, `enclos_idenclos`, `type_animal_idtype_animal`) VALUES ('adolf', '2010-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "hyènes"), (SELECT idtype_animal FROM `type_animal` WHERE nom like "hyeanidae")); 
````

6. On peuple maintenant la table type_capteur avec des enregistrements

```sql
INSERT INTO `type_capteur` (`idtype_capteur`, `nom`) VALUES
(1, 'température'),
(2, 'ph'),
(3, 'qualité de l\'air'),
(4, 'qualité de l\'eau'),
(6, 'humidité'),
(7, 'lumière'),
(8, 'son');
```

7. et on ajoute des capteurs aux enclos

```sql
INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Temp1', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "temp%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Hum1', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "félins"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "hum%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Temp orang outan', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "orang%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "temp%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Hum orang', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "orang%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "hum%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Temp hyènes', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "hy%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "temp%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Hum hyènes', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "hy%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "hum%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('Tem pingouins', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "ping%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "temp%"));

INSERT INTO `capteur` (`nom`, `description`, `enclos_idenclos`, `type_capteur_idtype_capteur`) VALUES ('eau pingouins', '2016-04-12 00:00:00', (SELECT idenclos FROM `enclos` WHERE nom like "ping%"), (SELECT idtype_capteur FROM `type_capteur` WHERE nom like "%eau"));
````

8. on peut maintenant ajouter des mesures sur un capteur. L'important est de comprendre qu'on identifie ce capteur à partir de son type et de son enclos. La clause where fait intervenir la notion de jointure, qui permet de filtrer le produit cartésien des table de manière très efficace.

```sql
INSERT INTO `mesure` (`valeur`, `date`, `capteur_idcapteur`) VALUES (23, '2016-04-12 00:00:00', (SELECT idcapteur FROM `capteur` c,`type_capteur` tc,`enclos` e WHERE e.nom like "félins" and e.idenclos=c.enclos_idenclos and tc.nom like "temp%" and c.type_capteur_idtype_capteur=tc.idtype_capteur));
```

# Les fichiers du serveur

* Les fichiers accueil.html, enclos.html et mesure.html, et monstyle.css forment la base du site web

* Les fichiers login.php, listerenclos.php, listermesure.php sont des exemples de programmes PHP exécutés par le serveur web pour envoyer à l'utilisateur final des données extraites de la base

* Les fichiers zoo-miami.jpg zoo.js zoo.php et getData.php montrent un exemple AJAX/JSON permettant à une application tournant côté user (navigateur web) d'accéder à la base de données en récupérant les dernières
températures.