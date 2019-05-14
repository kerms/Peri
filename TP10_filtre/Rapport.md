# TP10: Filtrage

```
Kailun ZHUANG
Yasma KADI
```
### fir_average_i16

Il ne faut pas oublier d'arrondir le résultat avec diviseur/2 i.e.2\*radius+1.

### fir_average_q16

Il faut remplacer la troncature par l'arrondi.

### fir_gauss_f32

Comparaison des performances des filtres:

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

### iir_q16

Il ne faut pas oublier de rediviser par Q.

### iir_q32

Dans le calcul de Y0, les opérandes sont multipliées par Q^2, il faut alors le diviser par Q. Puis encore par Q i.e. Q^2 pour avoir le résultat final y0.

### Comparaison

Les filtres en virgule fixe sont très proches voir identique au psnr en virgule flottante. Voilà, comme quoi les calculs en virgule fixe peut être aussi précis que les calculs en virgule flottante, en plus c'est plus rapide.