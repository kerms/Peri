# TP2 Multitâches Arduino

```
Kailun ZHUANG
Yasma KADI
```

### Gestion des tâches standard périodiques

  * Le tableau `waitForTimer[]` contient le numéro de la période enregistrée lors du dernier appel à `waitFor()`.

  * La fonction `waitFor()` renvoie 2 quand le nombre de périodes qui s'est écoulé depuis le dernier appel à waitFor() est //TODO

  * L'affichage de salut a le même comportement générique que Bonjour, donc on va réutiliser les fonctions de `Mess`. 
    - On a ajouté `struct Mess_st`  pour `Mess_salut`
    - On a modifié le macro `MAX_WAIT_FOR_TIMER` pour avpir maintenant 3 timers.
    - Le `setup` est similaire à celui de `Bonjour`, mais qui a un id = `2` et 1,5 sec
      ```c
      setup_Mess(&Mess_salut, 2, 1500000, "salut");
      ```
    - En fin, on a ajouté dans le main la tâche 
    ```cpp 
    loop_Mess(&Mess_salut);
    ```


### Utilisation de l'écran

  - On peut extraire `testdrawchar()` qui correspond à l'affichage d'un caractère.
  - `display.setCursor(x,y)` qui sert à positionner le curseur sur l'écran, sinon les caractères s'affichent à des positions successives. (aussi l'affichage déborde!)

```cpp
void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  for(int16_t i=0; i<10; i=(i+1)%10) {
    display.setCursor(0,0);
    display.clearDisplay();
    display.write(i+48); // 48 est le décalage par rapport à son numéro ASCII
    display.display();
    delay(1000);
  }

  display.display();
  delay(2000);
}
```


### Communications inter-tâches
On veut lire avec `T1` dans la boîte à lettre toute les 0,5 sec, donc on va lui conditionner avec un `waitFor`.\

On a `T2` qui écrit dans la boîte à lettre dès qu'il peut.\

Pour conditionner `T1`, on a mis dans une fonction `loop_photo()`, mais aussi `T2`
```cpp
void loop_photo(struct GPIO_t *photo) {
  loop_T2(&mb0);
  if (!waitFor(photo->timer, photo->period)) return;
  loop_T1(&mb0);
}
```


### Gestion des interruptions

