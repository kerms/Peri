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

Primary key : clé primaire consiste une colonne qui identifie chaque ligne d'une table. Elle est déclarée comme :

- Unique
- AUTO INCREMENT
- NOT NULL

Foreign key : clé étrangère qui est une référence (un pointeur) vers une clé primaire.

Quelques tables sont reliées entre elles avec des relations: 

- 1:1 chaque objet d'une table est relié à un seul objet de l'autre table.
- 1:n un objet d'une table est relié à plusieurs objets de l'autre.
- m:n un groupe d'objets `m` d'une table est associé à un groupe d'objets `n` de l'autre table.

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
