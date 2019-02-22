# TP1 sur Raspberry 25

```
Yasma KADI
Kailun ZHUANG
```
## Raspberry Pi 

### Accès

Après la compilation on se connecte avec x = 25
```
$ ssh -p 50x pi@peri
```

Pour envoyer un fichier sur la `raspberry` on utilise la commande : 
`scp -P 50x fichier pi@peri:nom_fichier`

L'exécution n'a pas marché avec `./`. Il fallait lancer en mode Super User : `sudo ./`

###### Remarque
Comme c'est un Linux qui tourne, on peut même envoyer les fichiers sources pour déboguer avec `gdb`!


### Contrôle de GPIO
Les macro `GPIO_CONF_AS_INPUT` et `GPIO_CONF_AS_OUTPUT` permettent de configurer un GPIO comme entrée et sortie respectivement.\
Les macro `GPIO_SET` et `GPIO_CLR` permettent d'écrire respectivement '1' et '0' sur le `GPIO`.\


### Faire clignoter une led
Un clignotement n'est juste que: 
  - on alume
  - on attend un certain temps
  - on ferme
  - on attend un certain temps
Pour faire clignoter en permanence, il suffit de faire boucler un clignotement puis attendre.\
Pour faire clignoter la `LED0` on a rajouté dans la boucle `while`  la macro `GPIO_CLR ( gpio_base, GPIO_LED0 )` entre les  `delay`.

### Faire clignoter deux leds à des fréquences différentes

Une première aproche simple est de faire de façon itérative.\
L'idée de dupliquer le code de la boucle en insérant le clignotement de `LED1`, par exemple si on veut faire clignoter `LED0` deux fois plus vite que `LED1`,:
  - on alume `LED0`
  - on alume `LED1`
  - `delay`
  - on ferme `LED0`
  - `delay`
  - on alume `LED0`
  - on ferme `LED1`
  - `delay`
  - on ferme `LED0`
  - `delay`

On peut aussi faire un compteur et tester si à chaque itération de la boucle si on doit éteindre une LED ou l'allumer. Mais cela ne sera pas efficace.\

La deuxième approche est de faire des [threads](https://openclassrooms.com/fr/courses/1513891-la-programmation-systeme-en-c-sous-unix/1514567-les-threads "OpenclassRoom").\

Pour factoriser le code, on peut créer une structure qui comporte les informations nécessaires au exécution:
 - L'adresse de début des GPIOs
 - Le numéro de GPIO
 - La fréquence de fonctionnement
On a alors besoin d'une seule fonction pour faire clignoter N LED à des fréquences différentes, chacune indépendament.\
Dans le code de la fonction, on met un while qui fait tout simplement:
  - on allume
  - on attend `struct.delay`
  - on éteint
  - on attend `struct.delay`
Par exemple:\
Pour le `LED0`, on lui passe son numéro qui est 4, puis `delay`\
Pour le `LED1`, on lui passe son numéro qui est 17, puis `delay`/3

Un thread qui fait clignoter une led:

```cpp
void* clignote_led(void *arg) {
    struct gpio_info *info = (struct gpio_info *) arg;
    printf("threads led created\n");
    print_info(info);
    
    while (1) {
        GPIO_SET(info->gpio_base, info->id_gpio);
        delay ( info->period + 2*swt*info->period );
        GPIO_CLR (info->gpio_base, info->id_gpio );
        delay ( info->period + 2*swt*info->period );
    }
}
```
Un thread qui gère le bouton:

```cpp

void* listen_button(void *arg) {
    struct gpio_info * info = (struct gpio_info *) arg;
    printf("threads button created\n");
    print_info(info);
    while(1) {
        if ( (button = GPIO_VALUE(info->gpio_base, info->id_gpio) == 0) ) {
            swt = !swt;
            printf("button pressed now swt = %d\n", swt);
            delay(info->period);
        }
    }
}

```
Dans le main:

```cpp
int main ( int argc, char **argv )
{
    int                 result;
    int                 period, half_period;
    uint32_t volatile * gpio_base = 0;
    struct gpio_info gled0, gled1, gbutton;
    pthread_t thread_led0, thread_led1, thread_button;

    /* Retreive the mapped GPIO memory. */
    result = setup_gpio_mmap ( &gpio_base );

    if ( result < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    period = 500; /* 2Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;

    /* Setup GPIO of LED0 to output. */
    GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED0 );
    GPIO_CONF_AS_OUTPUT ( gpio_base, GPIO_LED1 );
    GPIO_CONF_AS_INPUT  ( gpio_base, GPIO_BTN0 );

    /* INIT struct */
    gled0.gpio_base = gpio_base;
    gled0.id_gpio = GPIO_LED0;
    gled0.period = half_period;

    gled1.gpio_base = gpio_base;
    gled1.id_gpio = GPIO_LED1;
    // deux fois plus vite que LED0
    gled1.period = half_period/2; 
    

    gbutton.gpio_base = gpio_base;
    gbutton.id_gpio = GPIO_BTN0;
    gbutton.period = half_period;


    printf ( "-- info: start blinking.\n" );

    if(pthread_create(&thread_led0, NULL, clignote_led, (void*)&gled0) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_create(&thread_led1, NULL, clignote_led, (void*)&gled1) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_create(&thread_button, NULL, listen_button, (void*)&gbutton) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    /* Blink led at frequency of 1Hz. */

    sleep(10000000);

    if (pthread_join(thread_led0, NULL)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    pthread_join(thread_led1, NULL);
    pthread_join(thread_button, NULL);


    return 0;
}
```

###### Remarque
Pour utiliser les `threads`, on inclue la librairie `pthread.h`. Il faut aussi faire `pthread_join` avant le return du main, sinon le programme n'attend pas les `threads`.

## Arduino

### Changer la fréquence de clignotement par un bouton

On ajoute une variable global `switch_on` qui désigne le fonctionnement normal quand elle est mise à 0 et le changement de fréquence quand elle est mise à 1.\

On change le code dans la fonction de threads: \
Il suffit de faire `delay*(switch_on + 1)` pour diminuer de moitié la fréquence.

```cpp
#include <Wire.h>


const int led_id = 13;
const int photo_id = A0;
int val = 0;           // variable to store the value read
int switch_on = 0;
int prev = 0;


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  while (!Serial);
  pinMode(led_id, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    val = analogRead(photo_id);     // read the input pin
    if (val > 700) {
        switch_on = 1;
        if ( prev == 0 ) {
            Serial.println("+++++HIGH+++++");
            prev = 1;
        }
    }
    else {
      switch_on = 0;
      prev = 0; 
    }
    Serial.print("valeur de photoresistance : ");
    Serial.println(val);             // debug value
    digitalWrite(led_id, LOW);
    // fait une pause de 1 seconde ou 250 en mode HIGH
    delay(250/(switch_on*2+1));
    // éteint la LED
    digitalWrite(led_id, HIGH);
    // fait une pause de 1 seconde ou 250 en mode HIGH
    delay(250/(switch_on*2+1));
}
```
###### Remarque
La librairie Wire permet la communication avec 	les composants utilisant le protocol I2C / TWI.
