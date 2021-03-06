# TP2 Multitâches Arduino

```
Kailun ZHUANG
Yasma KADI
```

### Gestion des tâches standard périodiques

  * Le tableau `waitForTimer[]` contient le numéro de la période enregistrée lors du dernier appel à `waitFor()`.

  * La fonction `waitFor()` renvoie 2 quand on n'a pas appelé `waitFor()` depuis deux périodes.

  * L'affichage de salut a le même comportement générique que Bonjour, donc on va réutiliser les fonctions de `Mess`. 
    - On a ajouté `struct Mess_st`  pour `Mess_salut`
    - On a modifié la macro `MAX_WAIT_FOR_TIMER` pour avoir maintenant 3 timers.
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
La lecture de `T1` de la boîte à lettre se fait toute les 0,5 sec, on lui conditionne avec un `waitFor`.\

On a `T2` qui écrit dans la boîte à lettres dès qu'il peut.\

On crée d'abord une structure :

```cpp
struct GPIO_t {
  int timer;
  unsigned long period;
  int pin;
  int etat;
};
```

Pour conditionner `T1`, on a mis dans une fonction `loop_photo()`, et `T2` avant waitFor() pour récupérer la lettre dès qu'il pourra.

```cpp
void loop_photo(struct GPIO_t *photo) {
  loop_T2(&mb0);
  if (!waitFor(photo->timer, photo->period)) return;
  loop_T1(&mb0);
}
```

###### Dans `T1`:

```cpp
//write
void loop_T1(struct mailbox *mb) {
  if (mb->state != EMPTY) return; // attend que la mailbox soit vide
  mb->val = analogRead(photo_id);
  mb->state = FULL;
}
```


###### Dans `T2`:

```cpp
//read
void loop_T2(struct mailbox *mb) {
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  // usage de mb->val
  led.period = 1000000/(1000*5/mb->val);
  mb->state = EMPTY;
}
```

###### On ajoute la tâche du LED

```cpp
void loop_led(struct GPIO_t *led) {
  if (!waitFor(led->timer, led->period)) return;
  digitalWrite(led->pin, led->etat);
  led->etat = (led->etat+1)%2; // change d'etat ON/OFF
}
```

###### Variation de LED dépendant de la photorésistance

Le soucis avec cette implémentation est que lorsque la période change (la valeur de photorésistance change), le nombre de période depuis le dernier appel à `waitFor()` n'est plus cohérent.

Une résolution est d'avoir une période fixe et un compteur qui dépend de la photorésistance. 
  - Si le compteur arrive à 0, on change le state(ON/OFF) de la LED
  - Le compteur se décrémente à chaque période.

  * Si le compteur a une valeur de `10`, mais que la nouvelle valeur multiplicative est 8, le compteur devrait avoir 8. Donc compteur = mult_val;
  * Si le compteur a une valeur de `8`, et la nouvelle valeur de `mult_val` = 10, la valeur du compteur ne doit pas être changée.

Donc lors d'un changement valeur de photorésistance, on prend la valeur `min(compteur_courrant, val_photoresistance)`.

Maintenant, deux nouveaux champs sont ajoutés, une nouvelle structure est nécessaire.

```cpp
struct led_t {
  GPIO_t gpio;       // gpio information
  int cpt;           // remaining time to blink
  unsigned mult_val; // total time need to blink
};

//read
void loop_T2(struct mailbox *mb) {
  if (mb->state != FULL) return; // attend que la mailbox soit pleine
  // usage de mb->val
  led.cpt = min(mb->val, led.cpt);
  led.mult_val = mb->val; // change total time to blink
  mb->state = EMPTY;
}

void loop_led(struct led_t *led) {
  if (!waitFor(led->gpio.timer, led->gpio.period)) return;
  if (0 > led->cpt){  //cpt 0 or neg
    led->gpio.etat = (led->gpio.etat+1)%2; // switch state
    digitalWrite(led->gpio.pin, led->gpio.etat);
    led->cpt = led->mult_val; // reset cpt
  } else {  // cpt > 0
    --led->cpt;
  }
}
```

On obtient le résultat attendu :)

### Gestion des interruptions

On a essayé de compiler avec `Serial.onReceive(my_func)`, mais le compilateur ne le reconnait pas.

Notre ami `Google` nous a montré qu'il existe la fameuse fonction :

```cpp
void serialEvent();
```

Cette fonction est appelée automatiquement s'il existe des données entrées.

```cpp
void serialEvent() {
  int i, c;
  for (i = 0; i < Serial.available(); i++) {
    c = Serial.read();
    if (c == 115) {    // 115 = s en ASCII
      key_s = (key_s+1)%2; // on switch l'appuie sur s
    }
  }
}
```

parallèlement dans `loop_led()`, on remplace 

```cpp
led->gpio.etat = (led->gpio.etat+1)%2;
// par
led->gpio.etat = (led->gpio.etat+( 1 >> key_s) )%2 ;
```

Ainsi, l'état de la LED ne change pas si le switch_s est activé (=1).
