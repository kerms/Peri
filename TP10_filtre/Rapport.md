# TP10: Filtrage

```
Kailun ZHUANG
Yasma KADI
```

### Objectif

Le but de cette séance est d'implémenter des filtres en virgule fixe et évaluer leur qualité du filtrage par rapport aux filtres en virgule flottante.

### Filtres non récursifs

##### fir_average_i16

Il ne faut pas oublier d'arrondir le résultat avec diviseur/2 i.e.2\*radius+1.

##### fir_average_q16

Il faut remplacer la troncature par l'arrondi.

##### fir_gauss_f32

###### Comparaison 

Les tableaux ci-dessous représentent le rapport signal bruit (psnr) des différents filtres avec des paramètres variés.

| sigma noise | radius | F32    | I16    | Q8    | Q10   |
|-------------|--------|--------|--------|-------|-------|
| 10          | 2      | 42.37  | 42.37  | 42.28 | 42.28 |
| 5           | 2      | 85.15  | 85.15  | 76.91 | 76.91 |
| 10          | 4      | 49.11  | 49.11  | 43.84 | 48.78 |
| 5           | 4      | 112.63 | 112.63 | 58.73 | 88.46 |


| sigma noise | sigma gauss | G     |
|-------------|-------------|-------|
| 10          | 0.5         | 29.00 |
|             | 1.0         | 38.98 |
|             | 1.4         | 43.08 |
| 5           | 0.5         | 44.34 |
|             | 1.0         | 70.42 |
|             | 1.4         | 86.99 |

### Filtres non récursifs

##### iir_q16

Il ne faut pas oublier de rediviser par Q.

##### iir_q32

Dans le calcul de Y0, les opérandes sont multipliés par Q^2, il faut alors le diviser par Q. Puis encore par Q i.e. Q^2 pour avoir le résultat final y0.

###### Comparaison

Le psnr des filtres en virgule fixe est très proche voire identique au psnr en virgule flottante. 

| sigma  |       |          |          |          |
| noise  | alpha |  IIR f32 |  IIR q16 |  IIR q32 |
|--------|-------|----------|----------|----------|
| 10     | 0.8   | 41.58    | 40.18    | 41.71    |
|        | 0.6   | 44.67    | 44.05    | 44.49    |
|        | 0.4   | 47.92    | 48.24    | 47.92    |
| 5      | 0.8   | 81.05    | 89.49    | 79.94    |
|        | 0.6   | 94.65    | 87.96    | 91.11    |
|        | 0.4   | 100.12   | 123.68   | 100.12   |

### Conclusion 

À travers des manipulations mathématiques, nous avons montré qu'il est possible d'effectuer des calculs en virgule fixe tout en gardant la même précision des calculs en virgule flottante. L'avantage de cela est d'accélerer les opérations de calcul.
