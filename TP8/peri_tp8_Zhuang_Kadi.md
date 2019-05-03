# TP8: Serveur WEB (Appache-PHP-MySQL)

```
Yasma KADI
Kailun ZHUANG
```

### Objectif

Cette séance de TP est une initiation aux bases de données. 

##### Data Definition Language (DDL)

On travaille avec `mysql-workbench` pour réaliser une base de données d'un `zoo`.

CREATE TABLE 
ALTER TABLE
DROP TABLE

###### Remarque : 
Notre base de données est constituée de 7 tables : enclos, animal, type_animal, capteur, mesure, type_capteur, soigneur. 
Les tables contiennent des lignes (enregistrements) et des colonnes (champs). 

Primary key : la clé primaire est une colonne qui identifie chaque ligne d'une table. Elle est déclarée comme :

- Unique
- AUTO INCREMENT
- NOT NULL

Foreign key : le clé étrangère est une référence (un pointeur) vers une clé primaire.

Quelques tables sont reliées entre elles avec des relations: 

- 1:1 chaque objet d'une table est relié à un seul objet de l'autre table.
- 1:n un objet d'une table est relié à plusieurs objets de l'autre.
- m:n un groupe d'objets `m` d'une table est associé à un groupe d'objets `n` de l'autre table.

Une relation peut être `identifying` ou `non-identifying`. Pour les relations `non-identifying`, l'objet référencé peut ne pas exister, alors que les enregistrements avec des relations `identifying` nécessite l'existance de tous les objets référencés pour être créés.

##### Data Manipulation Language (DML)

On peut aussi manipuler les enregistrements de nos tables en `SQL`

```sql
#selectionner des enregistrements
SELECT colomn 1, colomn 2 ... FROM table 1, table 2 ...
[WHERE condition]
[ORDER BY DESC|ASC ];

#inserer de nouveaux enregistrements
INSERT INTO table (column 1, column 2...) 
VALUES 
(value 1_1, value 1_2...),
(value 2_1, value 2_2...),
(value 3_1, value 3_2...),
...
;

#supprimer des enregistrements
DELETE FROM table ...
[WHERE condition];

#mise a jour des valeurs
UPDATE table
SET column 1 = new_value, column 2 = new_value
[WHERE condition];
```

###### Remarque : 

On peut utiliser l’opérateur `LIKE` dans la clause `WHERE` des requêtes `SQL`. Ce mot-clé permet d’effectuer une recherche sur un modèle particulier.

__Syntaxe__ : Si on souhaite rechercher des enregistrements dont la valeur d’une colonne suit un certain modèle, il suffit d'écrire : 

```sql
SELECT *
FROM table
WHERE colonne LIKE modele
```

##### Warning : 

On n'arrive pas à télécharger le fichier ZIP en pièce jointe [ICI](https://www-soc.lip6.fr/trac/sesi-peri/wiki/Peri19_T08#no1)